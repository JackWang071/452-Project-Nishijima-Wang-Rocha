#include <iostream>
#include <thread>
#include <chrono>
#include <ctime> 
#include "ECU.h"
#include "Bus.h"

int main(){
	
	bool do_encrypt = true;
	int num_msg = 100;
	
	Bus* bus = new Bus();
	ECU* ecu1 = new ECU(bus, do_encrypt);
	ECU* ecu2 = new ECU(bus, do_encrypt);
	ECU* ecu3 = new ECU(bus, do_encrypt);
	bus->connect_node(ecu1);
	bus->connect_node(ecu2);
	bus->connect_node(ecu3);
	
	if(do_encrypt){
		std::cout<<"Encrypted messaging."<< "\n===" << std::endl;
	}
	else{
		std::cout<<"Unencrypted messaging for control testing."<< "\n===" << std::endl;
	}
	
	auto start = std::chrono::system_clock::now();
	
	std::thread e1_send(&ECU::sending, ecu1, num_msg);
	std::thread e2_send(&ECU::sending, ecu2, num_msg);
	std::thread e3_send(&ECU::sending, ecu3, num_msg);
	e1_send.join();
	e2_send.join();
	e3_send.join();
	
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "===" << "\nElapsed time: " << elapsed_seconds.count() << "s\n";
	
	return 0;
}