// CAN_Component.h
#pragma once
#ifndef CAN_Component_H
#define CAN_Component_H

class CAN_Component{
	public:
	int connect_component(CAN_Component* new_comp);
	int recv_msg(bool nextbit);
};

#endif // !CAN_Component_H