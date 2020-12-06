
#include <thread>
#include "ECU.h"
#include "Bus.h"

int main(){
	
	CAN_Component bus = Bus();
	CAN_Component ecu1 = ECU(bus);
	CAN_Component ecu2 = ECU(bus);
	bus.connect_component(ecu1);
	bus.connect_component(ecu2);
	
	//std::thread e1_send(&ECU::sending, ecu1);
	//std::thread e2_send(&ECU::sending, ecu2);
	
	//e1_send.join();
	//e2_send.join();
	//*/
	
	return 0;
}