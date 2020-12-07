// Bus.h
#pragma once
#ifndef Bus_H
#define Bus_H

#include <vector>		// for the vector data structure
#include <iostream>		// for printing to screen
#include <thread>		// for threads

#include "CAN_Component.h"

class Bus {
	private:
	std::vector<CAN_Component*> ecu_list;
	int bus_ID;
	
	static int bus_ID_vals;
	
	public:
	Bus();
	int connect_node(CAN_Component* new_comp);
	int rebroadcast(bool nextbit);
	int test_conn();
};

#endif // !Bus_H