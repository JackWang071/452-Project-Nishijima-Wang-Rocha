// ECU.cpp
#ifndef ECU_C
#define ECU_C

#include "ECU.h"

int ECU::ID_vals = 0;

ECU::ECU(CAN_Component* new_bus){
	ECU::bus = new_bus;
	ECU::msg_idx = 0;
	ECU::bitclock = 0;
	ECU::until_transmit_start = 0;
	ECU::ECU_ID = ++ECU::ID_vals;
	bus->connect_component(this);
}

int ECU::connect_component(CAN_Component* new_comp){
	bus = new_comp;
	return 0;
}

/*
{1, 1, 1, 0, 0, 1, 0, 1}
{0, 1, 0, 0}
*/

// Interface to allow the bus to add a new bit to ECU's recvbuffer
int ECU::recv_msg(bool nextbit){
	ECU::recv_buffer.push_back(nextbit);
	// Receiving a new bit from another ECU may cause this ECU to stop sending
	if(ECU::bitclock >= ECU::until_transmit_start){
		// If this ECU is still sending:
		if(ECU::msg_idx < ECU::send_buffer.size()){
			//
			if(nextbit != ECU::send_buffer[ECU::msg_idx]){
				ECU::until_transmit_start = ECU::bitclock + 120;  // force transmission to pause for a while
				std::cout << "ECU-" << ECU::ECU_ID << " pauses." << std::endl;
			}
		}
		// If this ECU is not sending, then 
		else{
			ECU::until_transmit_start = ECU::bitclock + 120;  // force transmission to pause for a while
			std::cout << "ECU-" << ECU::ECU_ID << " pauses." << std::endl;
		}
	}
	return 0;
}

// Infinite loop to allow ECU to generate and send messages
int ECU::sending(){
	// Print message to show that this ECU is active.
	std::cout << "ECU-" << ECU::ECU_ID << " active." << std::endl;
	
	int num_messages = 0;
	while(num_messages < 20){
		ECU::bitclock++;
		/** Send the next bit of my message if all of these are true:
		 *	Nobody else is sending
		 *	I have priority
		 *	There are unsent bits in my message
		 */
		if (ECU::msg_idx < ECU::send_buffer.size() && ECU::bitclock >= ECU::until_transmit_start){
			ECU::bus->recv_msg(ECU::send_buffer[ECU::msg_idx]);
			std::cout << ECU::send_buffer[ECU::msg_idx];
			ECU::msg_idx++;
		}
		if (ECU::msg_idx == ECU::send_buffer.size()) {
			msg_idx++;
			num_messages++;
			std::cout<<std::endl;
		}
		
		// If not currently sending a message, generate a new message at random time intervals
		if (ECU::msg_idx >= ECU::send_buffer.size() && rand() % 5 == 1){
			ECU::send_buffer = generate_msg();
			ECU::msg_idx = 0;
			
			std::cout << "ECU-" << ECU::ECU_ID << ": ";
		}
	}
	return 0;
}

// Generate a random CAN data frame and encrypt the data field using XXTEA, if so specified.
std::vector<bool> ECU::generate_msg(){
	bool encrypt = true;
	
	// Generate two random unsigned integers for our data field
	uint32_t data1 = (uint32_t) rand();
	uint32_t data2 = (uint32_t) rand();
	
	std::cout<< "ECU-" << ECU::ECU_ID << ": " <<data2<<" "<<data1<<std::endl;
	
	int arb_val = rand() % 30;
	
	// If encrypt is true:
	if(encrypt == true){
		btea(&data1, 2, ECU::teakey);
		btea(&data2, 2, ECU::teakey);
	}
	
	// Convert certain fields to bitset format
	std::bitset<11> arb_bitset (arb_val);
	std::bitset<4> dlc_bitset (8);
	std::bitset<32> data32_0 (data1);
	std::bitset<32> data64_32 (data2);
	std::bitset<64> data_bitset (data64_32.to_string() + data32_0.to_string());
	std::bitset<15> crc_bitset (9999);
	
	// Create a vector of booleans to represent this message
	std::vector<bool> frame;
	frame.push_back(0);	// Start of frame
	for (int i = 0; i <= arb_bitset.size(); i++){	// 1:11 Identifier
		frame.push_back(arb_bitset[i]);
	}
	frame.push_back(0);	// 12 Remote transmission request
	frame.push_back(0);	// 13 Identifier extension bit
	frame.push_back(0);	// 14 Reserved bit
	for (int i = 0; i < dlc_bitset.size(); i++){	// 15:18 Data length code
		frame.push_back(dlc_bitset[i]);
	}
	for (int i = 0; i < data_bitset.size(); i++){	// 19:[82] Data field
		frame.push_back(data_bitset[i]);
	}
	for (int i = 0; i < crc_bitset.size(); i++){	// [82]:[96] Cyclic redundancy check 
		frame.push_back(crc_bitset[i]);
	}
	frame.push_back(0);	// -10 CRC delimiter
	frame.push_back(1);	// -9 ACK slot
	frame.push_back(1);	// -8 ACK delimiter
	for (int i = 0; i < 7; i++){		// -7:-1 End of frame
		frame.push_back(1);
	}
	
	return frame;
}

#endif // !ECU_C