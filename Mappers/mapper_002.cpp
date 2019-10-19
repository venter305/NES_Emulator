#include "mapper_002.h"

using namespace std;

mapper_002::mapper_002(int prgBanks, int chrBanks): mapper(prgBanks,chrBanks){
	prgBank = 0;
}

int mapper_002::getPrgAddr(int addr){
	if (addr >= 0x8000 && addr <= 0xBFFF)
		return ((addr-0x8000)+((prgBank&0xF)*0x4000));
	return ((addr-0xC000)+((numPrgBanks-1)*0x4000));
}

int mapper_002::getChrAddr(int addr){
	return addr;
}

int mapper_002::write(int addr,int value){
	if (addr >= 0x8000)
		prgBank = value;
	
	return -1;
}

int mapper_002::ppuWrite(int addr,int value){
	return addr;
}

int mapper_002::getNtMirrorMode(){
	return -1;
}

void mapper_002::saveMapState(std::ofstream *file,char *x) {
	x[0] = numPrgBanks;
	file->write(x,1);
	x[0] = numChrBanks;
	file->write(x,1);

	x[0] = prgBank;
	file->write(x,1);	
}

void mapper_002::loadMapState(std::ifstream *file,char *x) {
	file->read(x,1);
	numPrgBanks = (int)(unsigned char)x[0];
	file->read(x,1);
	numChrBanks = (int)(unsigned char)x[0];

	file->read(x,1);	
	prgBank = (int)(unsigned char)x[0];
}
