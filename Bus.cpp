// Bus.cpp
#ifndef Bus_C
#define Bus_C

#include "Bus.h"

int Bus::bus_ID_vals = 0;

Bus::Bus(){
	Bus::bus_ID = ++bus_ID_vals;
}

int Bus::connect_node(CAN_Component* new_comp){
	Bus::ecu_list.push_back(new_comp);
	Bus::ecu_list.back()->test_conn();
	return 0;
}

int Bus::test_conn(){
	std::cout<<"Bus-"<<Bus::bus_ID<< " is connected."<<std::endl;
	return 0;
}

int Bus::rebroadcast(bool nextbit){
	for(int i = 0; i < ecu_list.size(); i++){
		Bus::ecu_list[i]->recv_msg(nextbit);
	}
	
	return 0;
}

#endif // !Bus_C