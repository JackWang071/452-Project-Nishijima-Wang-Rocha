// CAN_Component.h
#pragma once
#ifndef CAN_Component_H
#define CAN_Component_H

#include <iostream>

class CAN_Component{
	public:
	CAN_Component();
	int connect_component(CAN_Component& new_comp);
	int recv_msg(bool nextbit);
	int test_conn();
};

#endif // !CAN_Component_H