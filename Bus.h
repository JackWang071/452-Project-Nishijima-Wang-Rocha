// Bus.h
#pragma once
#ifndef Bus_H
#define Bus_H

#include <vector>		// for the vector data structure
#include <iostream>		// for printing to screen
#include <thread>		// for threads

#include "ECU.h"
#include "CAN_Component.h"

class Bus : public CAN_Component{
	private:
	std::vector<CAN_Component*> ecu_list;
	
	public:
	Bus();
	int connect_component(CAN_Component* new_comp);
	int recv_msg(bool nextbit);
};

#endif // !Bus_H