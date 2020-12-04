all: Simulator_Main.cpp CAN_Component.cpp Bus.cpp ECU.cpp xxtea.cpp 
	g++ -pthread -o sim Simulator_Main.cpp CAN_Component.cpp Bus.cpp ECU.cpp xxtea.cpp -std=c++11

clean: 
	$(RM) sim