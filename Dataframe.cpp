#ifndef Dataframe_C
#define Dataframe_C
#include "Dataframe.h"

std::vector<bool> Dataframe::frame;

Dataframe::Dataframe(){
	frame = vector<>(44);
	frame[0] = 0;	// Start of frame
	// frame[1:11]	// Identifier
	frame[12] = 0;	// Remote transmission request
	frame[13] = 0;	// Identifier extension bit
	frame[14] = 0;	// Reserved bit
	// frame[15:18]	// Data length code
	// frame[19:82]	// Data field
	
	// frame[-25:-11]	// Cyclic redundancy check 
	frame[-10] = 0;	// CRC delimiter
	frame[-9] = 1;	// ACK slot
	frame[-8] = 1;	// ACK delimiter
	// frame[-7:-1]	// End of frame
	for (int i = -7; i <= -1; i++){
		frame[i] = 1;
	}
}

bool Dataframe:insert_data(unsigned long data){
	dlc_start = 15
	dlc_end = 18
	
	data_start = 19
	data_limit = 82
}