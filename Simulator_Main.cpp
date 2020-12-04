
#include <thread>
#include "ECU.h"
#include "Bus.h"

int main(){
	
	Bus* bus = new Bus();
	ECU ecu1 = new ECU(bus);
	ECU ecu2 = new ECU(bus);
	
	std::thread e1(&ECU::run, &ecu1);
	e1.join();
	std::thread e2(&ECU::run, &ecu2);
	e2.join();
	
	return 0;
}