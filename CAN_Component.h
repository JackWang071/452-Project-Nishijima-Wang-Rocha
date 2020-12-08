// CAN_Component.h
#pragma once
#ifndef CAN_Component_H
#define CAN_Component_H

#include <iostream>

class CAN_Component{
	public:
	CAN_Component();
	virtual int recv_msg(int nextbit) = 0;
	virtual int test_conn() = 0;
};

#endif // !CAN_Component_H