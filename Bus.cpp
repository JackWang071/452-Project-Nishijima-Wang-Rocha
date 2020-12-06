// Bus.cpp
#ifndef Bus_C
#define Bus_C

#include "Bus.h"

Bus::Bus() : CAN_Component::CAN_Component() {}

int Bus::connect_component(CAN_Component& new_comp){
	ecu_list.push_back(new_comp);
	new_comp.test_conn();
	return 0;
}

int Bus::test_conn(){
	std::cout<<"Bus is connected."<<std::endl;
	return 0;
}

int Bus::recv_msg(bool nextbit){
	
	std::cout<<nextbit<<std::endl;
	
	for(int i = 0; i < ecu_list.size(); i++){
		ecu_list[i].recv_msg(nextbit);
	}
	
	return 0;
}

#endif // !Bus_C