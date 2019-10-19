#include <iostream>
#include "NES.h"

using namespace std;

NES::NES(){
	memory = new int[0x10000];
	CPU.nes = this;
	CPU.ppuReg2000 = &memory[0x2000];
	CPU.ppuReg2002 = &memory[0x2002];
	PPU.nes = this;
	PPU.ppuReg2000 = &memory[0x2000];
	PPU.ppuReg2001 = &memory[0x2001];
	PPU.ppuReg2002 = &memory[0x2002];
	CART.nes = this;
}

NES::~NES(){
}

void NES::reset(){
	for (int i=0;i<0x10000;i++){
		memory[i] = 0;
	}
	CPU.reset();
	PPU.reset();
	CART.reset();
}

int NES::readMemory(int addr){
	int mAddr = addr;
	if (addr >= 0x0800 && addr < 0x2000)
		mAddr %= 0x0800;
	else if (addr >= 0x2008 && addr < 0x4000)
		mAddr = addr%8+0x2008;
	else if (addr >= 0x6000){
			return CART.readPrgMem(addr);
	}

	if (mAddr == 0x2004)
		return PPU.OAM[memory[0x2003]];
	
	return memory[mAddr];
}

int NES::PPURead(int addr) {
	int mAddr = addr;
	if (addr >= 0x3f20)
		mAddr = addr%0x3f20+0x3f00;
	else if (addr >= 0x3000 && addr < 0x3F00)
		mAddr = addr%0x3000+0x2000;
	else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
		mAddr = addr - 0x0010;

	if (mAddr <= 0x1FFF)
		return CART.readChrMem(mAddr);
	else if (mAddr >= 0x2000 && mAddr <= 0x2FFF){
		int mMode = CART.getMirrorMode();
		if (!mMode)
			return PPU.ntRAM[mAddr%0x400];
		else if (mMode == 1){
			if (mAddr>=0x2800 && mAddr <= 0x2BFF)
				return PPU.ntRAM[(mAddr%0x400)];
			else if (mAddr >= 0x2C00)
				return PPU.ntRAM[0x400+(mAddr%0x400)];
			else
				return PPU.ntRAM[mAddr%0x800];
		}
		else if (mMode == 2){
			if (mAddr<=0x27FF)
				return PPU.ntRAM[mAddr%0x400];
			else if (addr >= 0x2800)
				return PPU.ntRAM[0x400 + mAddr%0x400];
			else
				return PPU.ntRAM[mAddr%0x400];
		}	
	}
	
	return PPU.memory[mAddr];
}

void NES::PPUWrite(int addr, int value){
	int mAddr = addr;
	if (addr >= 0x3f20)
		mAddr = addr%0x3f20+0x3f00;
	else if (addr >= 0x3000 && addr < 0x3F00)
		mAddr = addr%0x3000+0x2000;
	else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
		mAddr = addr - 0x0010;

	if (mAddr <= 0x1FFF)
		CART.cartWrite(mAddr,value);
	else if (mAddr >= 0x2000 && mAddr <= 0x2FFF){
		int mMode = CART.getMirrorMode();
		if (!mMode)
			PPU.ntRAM[mAddr%0x400] = value;
		else if (mMode == 1){
			if (mAddr>=0x2800 && mAddr <= 0x2BFF)
				PPU.ntRAM[mAddr%0x400] = value;
			else if (mAddr >= 0x2C00)
				PPU.ntRAM[0x400+(mAddr%0x400)] = value;
			else
				PPU.ntRAM[mAddr%0x800] = value;
		}
		else if (mMode == 2){
			if (mAddr<=0x27FF)
				PPU.ntRAM[mAddr%0x400] = value;
			else if (mAddr >= 0x2800)
				PPU.ntRAM[0x400 + mAddr%0x400] = value;
			else
				PPU.ntRAM[mAddr%0x400] = value;
		}	
	}

	PPU.memory[mAddr] = value;
	PPU.memory[addr] = value;
}

void NES::writeMemory(int addr, int value){
		int mAddr = addr;
		if (addr >= 0x0800 && addr < 0x2000)
			mAddr = addr%0x0800;
	  else if (addr >= 0x2008 && addr < 0x4000)
			mAddr = addr%8+0x2008;

		else if (addr >= 0x6000){
			CART.cartWrite(addr,value);
		}

	
	  if (mAddr == 0x2000){
			PPU.t &= (0xFFFF-0x0C00);
			PPU.t |= ((value & 3)<<10);
		}
		
		else if (mAddr == 0x2004){
			PPU.OAM[memory[0x2003]] = value;			
			memory[0x2003]++;
		}

		else if (mAddr == 0x2005){
			if(!PPU.w){
				PPU.t &= (0xFFFF-0x1F);
				PPU.t |= ((value & 0b11111000)>>3);
				PPU.x = (value & 0x7);
				PPU.w = 1;
			}
			else {
				PPU.t &= (0xFFFF-0x73e0);
				PPU.t |= ((value & 0x7)<<12);
				PPU.t |= ((value & 0b11111000)<<2);
				PPU.w = 0;
			}
			
		}

		else if (mAddr == 0x2006){
			if (!PPU.w){
				PPU.t &= 0x00FF;	
				PPU.t |= ((value & 0b00111111)<<8);
				PPU.w = 1;
			}
			else{
				PPU.t &= 0xFF00;
				PPU.t |= value;
				PPU.v = PPU.t;
				PPU.w = 0;
			}
		}

		else if (mAddr == 0x2007){
			PPUWrite(PPU.v,value);
			if (readMemory(0x2000) & 0b00000100)
				PPU.v += 32;
			else if ((readMemory(0x2000) & 0b00000100) == 0)
				PPU.v++;	
		}

		else if (mAddr == 0x4014){
			for (int i=0;i<256;i++){
				PPU.OAM[i] = memory[(value*256)+i];
			}
			PPU.cycles += (513*3);
		}
		
		memory[addr] = value;
		memory[mAddr] = value;
}
