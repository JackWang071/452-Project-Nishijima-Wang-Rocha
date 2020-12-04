#pragma once
#ifndef Dataframe_H
#define Dataframe_H
#include <vector>
#include <iostream>

class Dataframe{
	/*
	Start-of-frame						1					Denotes the start of frame transmission, must be dominant (0)
	Identifier							11					A (unique) identifier which also represents the message priority
	Remote transmission request (RTR)	1					Must be dominant (0) for data frames and recessive (1) for remote request frames (see Remote Frame, below)
	Identifier extension bit (IDE)		1					Must be dominant (0) for base frame format with 11-bit identifiers
	Reserved bit (r0)					1					Reserved bit. Must be dominant (0), but accepted as either dominant or recessive.
	Data length code (DLC)				4					Number of bytes of data (0–8 bytes)[a]
	Data field (red)					0–64 (0-8 bytes)	Data to be transmitted (length in bytes dictated by DLC field)
	CRC									15					Cyclic redundancy check
	CRC delimiter						1					Must be recessive (1)
	ACK slot							1					Transmitter sends recessive (1) and any receiver can assert a dominant (0)
	ACK delimiter						1					Must be recessive (1)
	End-of-frame (EOF)					7					Must be recessive (1)
	
	Total								44-108 bits
	*/
	private:
		std::vector<bool> frame;
	public:
		Dataframe();
		bool insert_data(unsigned long data);
}
