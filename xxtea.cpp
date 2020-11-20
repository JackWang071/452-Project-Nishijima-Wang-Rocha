#include <stdint.h>
#include <vector>
#include <iostream>

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

/**
BTEA will encode or decode n words as a single block where n > 1

v is the n word data vector
k is the 4 word key (used only to calculate MX)
n is negative for decoding
if n is zero result is 1 and no coding or decoding takes place, otherwise the result is zero
assumes 32 bit (4 byte) words and same endian coding and decoding
minimum 64 bit (8 byte) block size
*/
void btea(uint32_t *v, int n, uint32_t const key[4]) {
// y: 
// z: 
// sum: 
uint32_t y, z, sum;
// p: tracks iteration across words in v
// rounds: self-explanatory
// e: 
unsigned p, rounds, e;
if (n > 1) {          /* Coding Part */
  rounds = 6 + 52/n;
  sum = 0;
  z = v[n-1];  // z = the last word in v
  do {
	sum += DELTA;  // sum incremented by DELTA
	e = (sum >> 2) & 3; // e = (sum with bits shifted 2 right) bitwise and'd with 3
	for (p=0; p<n-1; p++) {  // iterating across first to second-to-last word
	  y = v[p+1]; // y = next word from word p
	  z = v[p] += MX;  // z = word p in v incremented by MX
	  //v[p] += MX;
	}
	y = v[0];  // y = first word (done to avoid error?)
	z = v[n-1] += MX;  // last word incremented by MX
	//v[n-1] += MX;
  } while (--rounds);
} 
else if (n < -1) {  /* Decoding Part */
  n = -n;
  rounds = 6 + 52/n;
  sum = rounds*DELTA;
  y = v[0];
  do {
	e = (sum >> 2) & 3;
	// Iterating backwards from last word in v to second word
	for (p=n-1; p>0; p--) {
	  z = v[p-1]; // z = word positioned before the word at p
	  y = v[p] -= MX; // y = current word decremented by MX
	  //v[p] -= MX;
	}
	z = v[n-1];
	y = v[0] -= MX;
	//v[p] -= MX;
	sum -= DELTA;
  } while (--rounds);
}
}

/*
int main(){
	std::vector<uint32_t> blocks;
	blocks.push_back(2882343476);  // ABCD1234 hex
	blocks.push_back(3455023248);  // CDEF7890 hex

	// Before encryption
	std::cout << blocks[0] << std::endl;
	std::cout << blocks[1] << std::endl;
	
	uint32_t const teakey[4] = {2712847316, 3858147256, 3385909746, 2746533334};  // [A1B2C3D4, E5F6A7B8, C9D0E1F2, A3B4C5D6]
	
	btea(&blocks[0], 2, teakey);
	btea(&blocks[1], 2, teakey);
	
	// After encryption
	std::cout << blocks[0] << std::endl;
	std::cout << blocks[1] << std::endl;
	
	return 0;
}
*/