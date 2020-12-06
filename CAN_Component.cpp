// CAN_Component.cpp
#ifndef CAN_Component_C
#define CAN_Component_C

#include "CAN_Component.h"

CAN_Component::CAN_Component(){}

int CAN_Component::connect_component(CAN_Component& new_comp){
	return 0;
}
int CAN_Component::recv_msg(bool nextbit){
	return 0;
}
int CAN_Component::test_conn(){
	std::cout<<"CAN component is connected."<<std::endl;
	return 0;
}

#endif // !CAN_Component_C