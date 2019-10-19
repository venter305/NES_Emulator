#include "mapper_001.h"
#include <iostream>
#include <bitset>
using namespace std;

mapper_001::mapper_001(int prgBanks, int chrBanks): mapper(prgBanks,chrBanks){
	shiftReg = 0x10;
	control = 0xC;
	chrBank0 = 0;
	chrBank1 = 0;
	prgBank = 0;

}

int mapper_001::getPrgAddr(int addr){
	if (addr >= 0x6000 && addr <= 0x7FFF)
		return addr-0x6000;

	//cout << (prgBank&0xF) << endl;
	
	if ((control&0xc)>>2 <= 1){
		//32kb bank switching
		return ((addr-0x8000)+((prgBank&0xF)*0x4000))+0x2000;
	}

	if ((control&0xc)>>2 == 2){
		//fix first bank (0x8000), switch second bank(0xC000)
		if (addr >= 0x8000 && addr <= 0xBFFF)
			return (addr-0x8000)+0x2000;
		return ((addr-0xC000)+((prgBank&0xF)*0x4000))+0x2000;
	}

	if ((control&0xc)>>2 == 3){	
		//fix last bank (0xC000), switch first bank(0x8000)
		if (addr >= 0x8000 && addr <= 0xBFFF)
			return ((addr-0x8000)+((prgBank&0xF)*0x4000))+0x2000;
		return ((addr-0xC000)+((numPrgBanks-1)*0x4000))+0x2000;
	}
	return addr;
}

int mapper_001::getChrAddr(int addr){
	if ((control&0x10)==0){
		return addr+((chrBank0&0x1E)*0x1000);
	}
	
	if (addr <= 0xFFF)
		return addr+(chrBank0*0x1000);
	
	return addr+(chrBank1*0x1000);
}	

int mapper_001::write(int addr, int value){
	if (addr < 0x8000 && addr >= 0x6000) {
		return addr-0x6000;
	}
	if (value&0x80){
		shiftReg = 0x10;
		control |= 0xC;
	}
	else if (shiftReg&1){
		int tmpVal = ((shiftReg&0x1e)>>1) + (value&1)*0x10;
		if (addr >= 0x8000 && addr <= 0x9FFF)
			control = tmpVal;
		else if (addr >= 0xA000 && addr <= 0xBFFF)
			chrBank0 = tmpVal;
		else if (addr >= 0xC000 && addr <= 0xDFFF)
			chrBank1 = tmpVal;
		else if (addr >= 0xE000 && addr <= 0xFFFF)
			prgBank = tmpVal;
		shiftReg = 0x10;
	}
	else{
		shiftReg>>=1;
		shiftReg |= (value&1)*0x10;
	}
	return -1;
}

int mapper_001::ppuWrite(int addr, int value){
	return addr;
}

int mapper_001::getNtMirrorMode(){
	int mMode = control&3;
	if (mMode <= 1)
		mMode = 0;
	else if (mMode == 2)
		mMode = 1;
	else if (mMode == 3)
		mMode = 2;
	return mMode;
}

void mapper_001::saveMapState(std::ofstream *file,char *x) {
	x[0] = numPrgBanks;
	file->write(x,1);
	x[0] = numChrBanks;
	file->write(x,1);
	
	x[0] = shiftReg;
	file->write(x,1);
	x[0] = control;
	file->write(x,1);
	x[0] = chrBank0;
	file->write(x,1);
	x[0] = chrBank1;
	file->write(x,1);
	x[0] = prgBank;
	file->write(x,1);
}

void mapper_001::loadMapState(std::ifstream *file,char *x) {
	file->read(x,1);
	numPrgBanks = (int)(unsigned char)x[0];
	file->read(x,1);
	numChrBanks = (int)(unsigned char)x[0];
	
	file->read(x,1);
	shiftReg = (int)(unsigned char)x[0];
	file->read(x,1);
	control = (int)(unsigned char)x[0];
	file->read(x,1);
	chrBank0 = (int)(unsigned char)x[0];
	file->read(x,1);
	chrBank1 = (int)(unsigned char)x[0];
	file->read(x,1);
	prgBank = (int)(unsigned char)x[0];
}

