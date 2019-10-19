#include "mapper_000.h"
#include <iostream>
using namespace std;

//Mapper 0
mapper_000::mapper_000(int prgBanks, int chrBanks): mapper(prgBanks,chrBanks){

}

int mapper_000::getPrgAddr(int addr){
	int mAddr = addr;
	
	if (addr >= 0x8000){
		return (numPrgBanks==1)?(addr%0x4000):(addr-0x8000);
	}
	
	return addr;
	
}

int mapper_000::getChrAddr(int addr){
	return addr;
}	

int mapper_000::write(int addr, int value){
	return 0;
}

int mapper_000::ppuWrite(int addr,int value){
	return addr;
}

int mapper_000::getNtMirrorMode(){
	return -1;
}

void mapper_000::saveMapState(std::ofstream *file,char *x) {
	x[0] = numPrgBanks;
	file->write(x,1);
	x[0] = numChrBanks;
	file->write(x,1);
}

void mapper_000::loadMapState(std::ifstream *file,char *x) {
	file->read(x,1);
	numPrgBanks = (int)(unsigned char)x[0];
	file->read(x,1);
	numChrBanks = (int)(unsigned char)x[0];
}
