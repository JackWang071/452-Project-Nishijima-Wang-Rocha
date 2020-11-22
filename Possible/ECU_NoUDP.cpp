#include <stdint.h>
#include <vector>
#include <iostream>
#include <bitset>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "xxtea.cpp"

std::string bus_file = "bus";
uint32_t const teakey[4] = {2712847316, 3858147256, 3385909746, 2746533334};

std::vector<bool> generate_msg(){
	bool encrypt = false;
	
	// Generate two random unsigned integers for our data field
	uint32_t data1 = (uint32_t) rand();
	uint32_t data2 = (uint32_t) rand();
	int arb = rand() % 30;
	
	// If encrypt is true:
	if(encrypt){
		btea(&data1, 2, teakey);
		btea(&data2, 2, teakey);
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

int main(){
	std::fstream bus_stream (bus_file, std::fstream::in|std::fstream::app); //fstream object open for reading and appending
	
	char nextbit;
	std::vector<bool> mynextmsg;
	int msg_idx = 0;			// tracks the current bit in mynextmsg
	std::vector<bool> othermsg;
	
	std::cout << "Node active." << std::endl;
	
	while (true){
		
		// If bus is not currently at the end of file, get the next character
		if(!bus_stream.eof()){
			nextbit = (bool) ((int) bus_stream.get());
			othermsg.push_back(nextbit);
			
			// If I'm currently sending a message and I receive someone else's bit, check whether the other person has priority or not
			if (msg_idx < mynextmsg.size()){
				if (mynextmsg[msg_idx] == 1 && nextbit == 0){
					std::cout << "\n" << "Pausing for higher priority transmission." << std::endl; 
					usleep(500000);		// Pause for 500,000 microseconds (0.5 seconds)
				}
			}
		}
		
		/** Send the next bit of my message if all of these are true:
		 *	Nobody else is sending
		 *	I have priority
		 *	There are unsent bits in my message
		 */
		if (msg_idx < mynextmsg.size()){
			// If nobody else is sending, send next bit
			if (bus_stream.eof()){
				bus_stream << (int) mynextmsg[msg_idx];
				bus_stream.ignore(1);
				std::cout << (int) mynextmsg[msg_idx];	// Print the sent bit
				msg_idx++;
			}
		}
		
		// If not currently sending a message, generate a new message at random time intervals
		if (msg_idx >= mynextmsg.size() && rand() % 5 == 1){
			mynextmsg = generate_msg();
			msg_idx = 0;
			std::cout << "New message." << std::endl;
		}
	}
	
	if(bus_stream.is_open()){
		bus_stream.close();
	}
	return 1;
}