// ECU.cpp
#ifndef ECU_C
#define ECU_C

#include "ECU.h"

int ECU::ID_vals = 0;

ECU::ECU(Bus* new_bus, bool encrypt_opt) : CAN_Component::CAN_Component() {
	ECU::bus = new_bus;
	ECU::bus->test_conn();
	
	ECU::msg_idx = 0;
	ECU::bitclock = 0;
	ECU::until_transmit_start = 0;
	ECU::ECU_ID = ++ECU::ID_vals;
	ECU::ones_count = 0;
	ECU::msg_ends.push_back(0);
	ECU::encrypt = encrypt_opt;
}

int ECU::test_conn(){
	std::cout << "C ECU-" << ECU::ECU_ID << " connected." << std::endl;
	return 0;
}

// Interface to allow the bus to add a new bit to ECU's recvbuffer
int ECU::recv_msg(int nextbit){
	ECU::recv_buffer.push_back(nextbit);
	
	// Receiving a new bit from another ECU may cause this ECU to stop sending
	if(ECU::bitclock >= ECU::until_transmit_start){
		// If this ECU is still sending:
		if(ECU::msg_idx < ECU::send_buffer.size()){
			// If nextbit different from the current bit being sent, force transmission to pause for a while
			// Theoretically should avoid the ECU from stopping transmission because of its own bits
			if(nextbit != (int)ECU::send_buffer[ECU::msg_idx] - 48){
				ECU::until_transmit_start = ECU::bitclock + 120;
				std::cout << "W ECU-" << ECU::ECU_ID << " waits" << std::endl;
			}
		}
		// If this ECU is not sending but still receiving outside bits, then force transmission to pause for a while
		else{
			ECU::until_transmit_start = ECU::bitclock + 120;
			std::cout << "W ECU-" << ECU::ECU_ID << " waits" << std::endl;
		}
	}
	
	// If an EOF field is detected, the ECU attempts to parse the newest message
	if(nextbit == 1){
		ECU::ones_count++;
		if(ECU::ones_count == 9){
			ECU::ones_count = 0;
			if(decrypt(ECU::msg_ends.back(), ECU::recv_buffer.size()) == 0){
				ECU::msg_ends.push_back(ECU::recv_buffer.size());
			}
		}
	}
	else{
		ECU::ones_count = 0;
	}
	return 0;
}

// Parse and decrypt completed messages
int ECU::decrypt(int msg_begin, int msg_end){
	// Extract the newest message from the receive buffer
	std::string bitstream = "";
	for(int i = msg_begin; i < msg_end; i++){
		bitstream += (char) (ECU::recv_buffer[i] + 48);
	}
	
	// Make sure that the received message isn't incomplete
	if(bitstream.size() < 83){
		return 1;
	}
	
	std::cout << "R ECU-" << ECU::ECU_ID << " " << bitstream << std::endl;
	
	// Extract the data field
	std::bitset<32> data64_32(bitstream.substr(19, 32));
	std::bitset<32> data32_0(bitstream.substr(51, 32));
		
	// Decrypt the data field
	uint32_t data_arr[2];
	data_arr[0] = (uint32_t) data32_0.to_ulong();
	data_arr[1] = (uint32_t) data64_32.to_ulong();
	
	std::cout<<"P ECU-"<<ECU::ECU_ID<<" "<<data_arr[1]<<" "<<data_arr[0];
	
	if(ECU::encrypt == true){
		btea(data_arr, -2, ECU::teakey);
		std::cout<<" decrypts " << data_arr[1] <<" "<<data_arr[0];
	}
	std::cout<<std::endl;
	return 0;
}

// Infinite loop to allow ECU to generate and send messages
int ECU::sending(int max_num_msgs){
	// Print message to show that this ECU is active.
	std::cout << "ECU-" << ECU::ECU_ID << " active." << std::endl;

	int num_messages = 0;
	while(num_messages < max_num_msgs){
		ECU::bitclock++;
		/** Send the next bit of my message if all of these are true:
		 *	Nobody else is sending
		 *	I have priority
		 *	There are unsent bits in my message
		 */
		
		if (ECU::msg_idx < ECU::send_buffer.size() && ECU::bitclock >= ECU::until_transmit_start){
			//std::cout << ECU::send_buffer[ECU::msg_idx];
			ECU::bus->rebroadcast((int) ECU::send_buffer[ECU::msg_idx] - 48);
			ECU::msg_idx++;
		}
		if (ECU::msg_idx == ECU::send_buffer.size()) {
			//std::cout<<std::endl;
			msg_idx++;
			num_messages++;
		}
		
		// If not currently sending a message, generate a new message at random time intervals
		if (ECU::msg_idx >= ECU::send_buffer.size() && rand() % 5 == 1){
			generate_msg();
			ECU::msg_idx = 0;
			
			std::cout << "S ECU-" << ECU::ECU_ID << " " << ECU::send_buffer << std::endl;
		}
	}
	return 0;
}

// Generate a random CAN data frame and encrypt the data field using XXTEA, if so specified.
int ECU::generate_msg(){
	// Generate two random unsigned integers for our data field
	uint32_t data_arr[2];
	data_arr[0] = (uint32_t) rand();
	data_arr[1] = (uint32_t) rand();
	
	std::cout<< "G ECU-" << ECU::ECU_ID << " " <<data_arr[1]<<" "<<data_arr[0];
	
	// If encrypt is true:
	if(ECU::encrypt == true){
		btea(data_arr, 2, ECU::teakey);
		std::cout<< " encrypts " << data_arr[1]<<" "<<data_arr[0];
	}
	
	std::cout<<std::endl;
	
	// Convert certain fields to bitset format
	int arb_val = rand() % 30;
	std::bitset<11> arb_bitset (arb_val);
	std::bitset<4> dlc_bitset (8);
	std::bitset<32> data32_0 (data_arr[0]);
	std::bitset<32> data64_32 (data_arr[1]);
	std::bitset<64> data_bitset (data64_32.to_string() + data32_0.to_string());
	std::bitset<15> crc_bitset (9999);
	
	// Concatenate these strings together into a data frame
	ECU::send_buffer = "0" 						// Start of frame
					+ arb_bitset.to_string() 	// 1:11 Identifier
					+ '0'						// 12 Remote transmission request
					+ '0'						// 13 Identifier extension bit
					+ '0' 						// 14 Reserved bit
					+ dlc_bitset.to_string() 	// 15:18 Data length code
					+ data_bitset.to_string() 	// 19:[82] Data field
					+ crc_bitset.to_string() 	// -25:-11 Cyclic redundancy check 
					+ '0'						// -10 CRC delimiter
					+ '1'						// -9 ACK slot
					+ '1' 						// -8 ACK delimiter
					+ "1111111";				// -7:-1 End of frame
	
	return 0;
}

#endif // !ECU_C