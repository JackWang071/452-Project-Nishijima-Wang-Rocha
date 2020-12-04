#include <stdint.h>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "xxtea.cpp"

string bus_address = "0.0.0.0"	// hardcoded string of the bus VM
int bus_port = 9090;			// hardcoded port of the bus VM
string messages_file;

uint32_t const teakey[4] = {2712847316, 3858147256, 3385909746, 2746533334};

vector<bool> generate_msg(){
	bool encrypt = false;
	
	// Generate two random unsigned integers for our data field
	uint32_t data1 = (uint32_t) rand();
	uint32_t data2 = (uint32_t) rand();
	
	// If encrypt is true:
	if(encrypt){
		xxtea::btea(&data1, 2, teakey);
		xxtea::btea(&data2, 2, teakey);
	}
	
	// Convert certain fields to bitset format
	std::bitset<11> arbitration (9999);
	std::bitset<4> dlc (8);
	std::bitset<32> data32_0 (data1);
	std::bitset<32> data64_32 (data2);
	string full_data = data64_32.to_string() + data32_0.to_string();
	std::bitset<15> crc (9999);
	
	
	
	
	// Create a vector of booleans to represent this message
	vector<bool> frame = vector<>(108);
	frame[0] = 0;	// Start of frame
	for (int i = 1; i <= 11; i++){	// 1:11 Identifier
		frame[i] = arbitration
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
	bus_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in address_struct;
	address_struct.sin_family = AF_INET;
    address_struct.sin_addr.s_addr = inet_addr(bus_address);
    address_struct.sin_port = htons(port);
	bind(bus_socket, (SOCKADDR*) &address_struct, sizeof(address_struct))
	
	server_length = sizeof(struct sockaddr_in);
	
	message = generate_msg();
	
	// NOT complete. Need to figure out arbitration.
	for (int i = 0; i < message.size(); i++){
		sendto(bus_socket, message[i], 1, 0, (SOCKADDR*) &address_struct, server_length);
	}
	
	
	return 1
}
