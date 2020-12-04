// ECU.cpp
#ifndef ECU_C
#define ECU_C

#include "ECU.h"

ECU::ECU(CAN_Component* new_bus){
	ECU::bus = new_bus;
	ECU::ones_counter = 0;
	ECU::bus->connect_component(this);
}

int ECU::connect_component(CAN_Component* new_comp){
	bus = new_comp;
	return 0;
}

// Return true if arbitration indicates that this ECU's message has priority
bool ECU::arbitration(std::vector<bool> my_msg){
	int last_delimiter = ECU::msg_transitions.back();
	
	// Check that the other message is still in arbitration phase.
	if(ECU::recv_buffer.size() - last_delimiter < 11){
		for(int i = 0; i < ECU::recv_buffer.size() - last_delimiter; i++){
			// If my_msg has a 0 where the other message has a 1, then this ECU has priority
			if(my_msg[i] < ECU::recv_buffer[i+last_delimiter]){
				return true;
			}
			if(my_msg[i] > ECU::recv_buffer[i+last_delimiter]){
				return false;
			}
		}
	}
	
	return false;
}

/*
{1, 1, 1, 0, 0, 1, 0, 1}
{0, 1, 0, 0}
*/

int ECU::recv_msg(bool nextbit){
	ECU::recv_buffer.push_back(nextbit);
	// If a transition to another message is detected, then note the transition's position in recv_buffer
	if(ECU::ones_counter==9 && nextbit==0){
		ECU::msg_transitions.push_back(ECU::recv_buffer.size());
	}
	if(nextbit==1){
		ECU::ones_counter++;
	}
	else{
		ECU::ones_counter = 0;
	}
	return 0;
}

int ECU::run(){
	std::vector<bool> mynextmsg;
	int msg_idx = 0;			// tracks the current bit in mynextmsg
	
	// Print message to show that this ECU is active.
	std::cout << "Node active." << std::endl;
	
	while(true){
		/** Send the next bit of my message if all of these are true:
		 *	Nobody else is sending
		 *	I have priority
		 *	There are unsent bits in my message
		 */
		if (msg_idx < mynextmsg.size() && ECU::arbitration(mynextmsg) == true){
			ECU::bus->recv_msg(mynextmsg[msg_idx]);
			std::cout << mynextmsg[msg_idx];
		}
		else{
			std::cout<<std::endl;
		}
		
		// If not currently sending a message, generate a new message at random time intervals
		if (msg_idx >= mynextmsg.size() && rand() % 5 == 1){
			mynextmsg = generate_msg();
			msg_idx = 0;
			std::cout << "New message." << std::endl;
		}
	}
	return 0;
}

// Generate a random CAN data frame and encrypt the data field using XXTEA, if so specified.
std::vector<bool> ECU::generate_msg(){
	bool encrypt = false;
	
	// Generate two random unsigned integers for our data field
	uint32_t data1 = (uint32_t) rand();
	uint32_t data2 = (uint32_t) rand();
	int arb = rand() % 30;
	
	// If encrypt is true:
	if(encrypt){
		btea(&data1, 2, ECU::teakey);
		btea(&data2, 2, ECU::teakey);
	}
	
	// Convert certain fields to bitset format
	std::bitset<11> arbitration (arb);
	std::bitset<4> dlc (8);
	std::bitset<32> data32_0 (data1);
	std::bitset<32> data64_32 (data2);
	std::string full_data = data64_32.to_string() + data32_0.to_string();
	std::bitset<15> crc (9999);
	
	// Create a vector of booleans to represent this message
	std::vector<bool> frame = std::vector<bool>(108);
	frame[0] = 0;	// Start of frame
	for (int i = 1; i <= 11; i++){	// 1:11 Identifier
		frame[i] = arbitration[i-1];
	}
	frame[12] = 0;	// 12 Remote transmission request
	frame[13] = 0;	// 13 Identifier extension bit
	frame[14] = 0;	// 14 Reserved bit
	for (int i = 15; i <= 18; i++){	// 15:18 Data length code
		frame[i] = dlc[i-15];
	}
	for (int i = 19; i <= 82; i++){	// 19:[82] Data field
		frame[i] = full_data.at(i-19);
	}
	for (int i = -25; i <= -11; i++){	// -25:-11 Cyclic redundancy check 
		frame[i] = crc[i+25];
	}
	frame[-10] = 0;	// -10 CRC delimiter
	frame[-9] = 1;	// -9 ACK slot
	frame[-8] = 1;	// -8 ACK delimiter
	for (int i = -7; i <= -1; i++){		// -7:-1 End of frame
		frame[i] = 1;
	}
	
	return frame;
}

#endif // !ECU_C