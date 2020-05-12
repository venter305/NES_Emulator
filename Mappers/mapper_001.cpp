#include "mapper_001.h"
#include <iostream>
#include <bitset>
using namespace std;

mapper_001::mapper_001(int prgBanks, int chrBanks): mapper(prgBanks,chrBanks){
	//Init
	shiftReg = 0x10;
	control = 0xC;
	chrBank0 = 0;
	chrBank1 = 0;
	prgBank = 0;

}

//PRG bank switching
int mapper_001::getPrgAddr(int addr){
	
	//RAM
	if (addr >= 0x6000 && addr <= 0x7FFF){
		return addr-0x6000;
	}
	
	if ((control&CTL_PRGBANK)>>2 <= 1){
		//32kb bank switching
		return ((addr-0x8000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
	}

	if ((control&CTL_PRGBANK)>>2 == 2){
		//fix first bank (0x8000), switch second bank(0xC000)
		if (addr >= 0x8000 && addr <= 0xBFFF)
			return (addr-0x8000)+0x2000*(prgRAM);
		return ((addr-0xC000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
	}

	if ((control&CTL_PRGBANK)>>2 == 3){	
		//fix last bank (0xC000), switch first bank(0x8000)
		if (addr >= 0x8000 && addr <= 0xBFFF)
			return ((addr-0x8000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
		return ((addr-0xC000)+((numPrgBanks-1)*0x4000))+0x2000*(prgRAM);
	}
	return addr;
}

//CHR bank switching
int mapper_001::getChrAddr(int addr){
	//8KB switching
	if ((control&CTL_CHRBANK)==0){
		return addr+((chrBank0&CHR_BANK)*0x1000);
	}
		

	//4KB switching
	if (addr <= 0x0FFF){
		return addr+(chrBank0*0x1000);
	}
	
	return (addr%0x1000)+(chrBank1*0x1000);
}	

//PRG write
int mapper_001::write(int addr, int value){
	//RAM, return offset address
	if (addr < 0x8000 && addr >= 0x6000) {
		return addr-0x6000;
	}

	//Load register
	//reset shift Reg and lock top PRG bank
	if (value&0x80){
		shiftReg = 0x10;
		control |= 0xC;
	}
	//After 5 bits have been shifted in to register
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
	//Shift value into shift register
	else{
		shiftReg>>=1;
		shiftReg |= (value&1)*0x10;
	}
	return -1;
}

//CHR write
int mapper_001::ppuWrite(int addr, int value){
	return addr;
}

//Mirror modes
int mapper_001::getNtMirrorMode(){
	int mMode = control&CTL_MIRROR;
	if (mMode <= 1)
		mMode = 0;
	else if (mMode == 2)
		mMode = 1;
	else if (mMode == 3)
		mMode = 2;
	return mMode;
}

//Save
void mapper_001::saveMapState(std::ofstream *file,char *x) {
	//Number of banks
	x[0] = numPrgBanks;
	file->write(x,1);
	x[0] = numChrBanks;
	file->write(x,1);
	
	//Registers
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

//Load 
void mapper_001::loadMapState(std::ifstream *file,char *x) {
	//Number of banks
	file->read(x,1);
	numPrgBanks = (int)(unsigned char)x[0];
	file->read(x,1);
	numChrBanks = (int)(unsigned char)x[0];
	
	//Registers
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

