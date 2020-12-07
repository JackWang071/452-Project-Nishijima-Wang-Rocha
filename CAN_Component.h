// CAN_Component.h
#pragma once
#ifndef CAN_Component_H
#define CAN_Component_H

#include <iostream>

class CAN_Component{
	public:
	CAN_Component();
	virtual int recv_msg(bool nextbit) = 0;
	virtual int test_conn() = 0;
	/*
	virtual int recv_msg(bool nextbit){
		std::cout<<"CAN component receives "<<nextbit<<std::endl;
		return 0;
	}
	virtual int test_conn(){
		std::cout<<"CAN component is connected."<<std::endl;
		return 0;
	}
	*/
};

#endif // !CAN_Component_H