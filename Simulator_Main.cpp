
#include <thread>
#include "ECU.h"
#include "Bus.h"

int main(){
	
	Bus* bus = new Bus();
	ECU* ecu1 = new ECU(bus);
	ECU* ecu2 = new ECU(bus);
	
	
	std::thread e1(&ECU::sending, ecu1);
	std::thread e2(&ECU::sending, ecu2);
	
	e1.join();
	e2.join();
	//*/
	
	return 0;
}