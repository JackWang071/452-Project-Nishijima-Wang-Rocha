// ECU.h
#pragma once
#ifndef ECU_H
#define ECU_H

#include <stdint.h>		// for additional primitive types such as uint32_t
#include <vector>		// for the vector data structure
#include <iostream>		// for printing to screen
#include <bitset>		// for the bitset data structure
#include <cstdlib>		// for rand()
#include <thread>		// for threads
#include <string>

#include "Bus.h"
#include "xxtea.cpp"
#include "CAN_Component.h"

class ECU : public CAN_Component{
	private:
	Bus* bus;
	uint32_t const teakey[4] = {2712847316, 3858147256, 3385909746, 2746533334};
	std::vector<int> recv_buffer;
	std::string send_buffer;
	std::vector<int> msg_ends;
	int msg_idx;
	int bitclock;
	int until_transmit_start;
	int ECU_ID;
	bool encrypt;
	int ones_count;

	static int ID_vals;
	
	public:
	ECU(Bus* new_bus, bool encrypt_opt=true);
	int test_conn();
	int recv_msg(int nextbit);
	int decrypt(int msg_begin, int msg_end);
	int sending(int max_num_msgs=10);
	int generate_msg();
};

#endif // !ECU_H