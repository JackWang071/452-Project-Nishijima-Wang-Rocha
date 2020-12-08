#include <iostream>
#include <thread>
#include "ECU.h"
#include "Bus.h"

int main(){
	
	std::cout<<"Proof of concept"<<std::endl;
	
	Bus* bus = new Bus();
	ECU* ecu1 = new ECU(bus);
	ECU* ecu2 = new ECU(bus);
	bus->connect_node(ecu1);
	bus->connect_node(ecu2);
	
	std::thread e1_send(&ECU::sending, ecu1);
	e1_send.join();
	
	return 0;
}