#include <iostream>
#include "NES.h"
#include <thread>

using namespace std;

NES::NES(){
	//Set up components
	CPU.nes = this;
	PPU.nes = this;
	CART.nes = this;
	APU.nes = this;
	dTime = 0;

	audioSampleRate = 48000;
	//NTSC
	systemClockTime = 21477272.0f;
	ppuClockTime = systemClockTime/4;
	//PAL
	// systemClockTime = 26601712.0f;
	// ppuClockTime = systemClockTime/5;
}

NES::~NES(){
}

//Reset Emulator
void NES::reset(){

	dTime = 0;
	//Reset components
	CART.reset();
	PPU.Reset();
	APU.reset();
	CPU.reset();

	//Reset memory to 0
	for (int i=0;i<0x10000;i++){
		memory[i] = 0;
	}
}

bool NES::clock(int cycles){
	if (CPU.pc == debugBreakAddr){
		return false;
	}

	if (needReset){
		reset();
		needReset = false;
		return false;
	}

	while(cycles){
		int extraCycles = 0;
		if (cpuTmpCycles % 3 == 0){
			extraCycles = CPU.runInstructions();
			CART.mapper->Clock();
			APU.clock(1);
		}
		PPU.Clock(1);
		cpuTmpCycles++;

		dTime += 1.0/(ppuClockTime*runSpeed);
		if (dTime > 1.0/audioSampleRate){
			dTime = 0;
			return true;
		}
		cycles--;
	}
	return false;
}

//read from memory without side effects
int NES::peekMemory(int addr){
	//Set up memory mirrors
	int mAddr = addr;
	//Internal RAM
	if (addr >= 0x0800 && addr < 0x2000)
		mAddr %= 0x0800;
	//PPU Registers
	else if (addr >= 0x2008 && addr < 0x4000)
		mAddr = addr%8+0x2008;
	else if (addr >= 0x6000){
		return CART.PeekPrgMem(addr);
	}

	if (mAddr == 0x2004)
		return PPU.OAM[memory[0x2003]];

	if (mAddr == 0x2007){
		//Read from VBuffer
		if (PPU.v < 0x3f00) return 0xFF&PPU.vBuffer;
		//Read from VRAM
		else return 0xFF&PPURead(PPU.v%0x4000);
	}

	if (mAddr == 0x4015){
		uint8_t out = 0x00;
		out |= (APU.pulse1.lenCounter > 0) ? 0x01 : 0x00;
		out |= (APU.pulse2.lenCounter > 0) ? 0x02 : 0x00;
		out |= (APU.triangle.lengthCounter > 0) ? 0x04 : 0x00;
		out |= (APU.noise.lenCounter > 0) ? 0x08 : 0x00;
		out |= (APU.dmc.bytesRemaining > 0) ? 0x10 : 0x00;
		out |= (APU.frameCounter.irqFlag) ? 0x40 : 0x00;
		out |= (APU.dmc.irqFlag) ? 0x80 : 0x00;
		return out;
	}

	return memory[mAddr];

}

//Read from memory
int NES::readMemory(int addr){
	//Set up memory mirrors
	int mAddr = addr;
	//Internal RAM
	if (addr >= 0x0800 && addr < 0x2000)
		mAddr %= 0x0800;
	//PPU Registers
	else if (addr >= 0x2008 && addr < 0x4000)
		mAddr = addr%8+0x2008;
	//Cartridge
	else if (addr >= 0x6000){
		return CART.readPrgMem(addr);
	}

	//PPU registers

	if (mAddr == 0x2002){
		//Clear MSB after read
		int tmpVal = memory[0x2002];
		writeMemory(0x2002,memory[mAddr]&0x7F);
		PPU.nmiOccured = false;
		PPU.w = 0;
		return tmpVal;
	}
	//Read from OAM memory
	if (mAddr == 0x2004)
		return PPU.OAM[memory[0x2003]];
	if (mAddr == 0x2007){
		//Read from VBuffer
		int tmpVal = 0;
		if (PPU.v < 0x3f00){
			tmpVal = 0xFF&PPU.vBuffer;
			PPU.vBuffer = PPURead(PPU.v%0x3f00);
		}
		//Read from pallet
		else{
			tmpVal = 0xFF&PPURead(PPU.v%0x4000);
			PPU.vBuffer = PPURead(PPU.v%0x100 + 0x2f00);
		}
		//Increment PPU VRAM by value in 0x2000
		if (memory[0x2000] & 0b00000100)
			PPU.v += 32;
		else
			PPU.v++;

		//clock mmc3 mapper
		CART.readChrMem(PPU.v);

		return tmpVal;
	}


	if (mAddr == 0x4015){
		uint8_t out = 0x00;
		out |= (APU.pulse1.lenCounter > 0) ? 0x01 : 0x00;
		out |= (APU.pulse2.lenCounter > 0) ? 0x02 : 0x00;
		out |= (APU.triangle.lengthCounter > 0) ? 0x04 : 0x00;
		out |= (APU.noise.lenCounter > 0) ? 0x08 : 0x00;
		out |= (APU.dmc.bytesRemaining > 0) ? 0x10 : 0x00;
		out |= (APU.frameCounter.irqFlag) ? 0x40 : 0x00;
		out |= (APU.dmc.irqFlag) ? 0x80 : 0x00;
		APU.frameCounter.irqFlag = false;
		return out;
	}

	//controllers

	if (mAddr == 0x4016)
		return CONTRL.readController(0);
	if (mAddr == 0x4017)
		return CONTRL.readController(1);

	return memory[mAddr];
}

//Write to memory
void NES::writeMemory(int addr, int value){
		//Set up memory mirrors
		int mAddr = addr;
		//Internal RAM
		if (addr >= 0x0800 && addr < 0x2000)
			mAddr = addr%0x0800;
		//PPU registers
	  else if (addr >= 0x2008 && addr < 0x4000)
			mAddr = addr%8+0x2008;
		//Cartridge
		else if (addr >= 0x6000){
			CART.cartWrite(addr,value);
		}

		if ((mAddr >= 0x2000 && mAddr <= 0x2007 && mAddr != 0x2002)){
			writeMemory(0x2002, memory[0x2002] & 0b11100000);
			writeMemory(0x2002, memory[0x2002] | (value & 0b00011111));
		}

		//PPU Control
	  if (mAddr == 0x2000){
			PPU.nmiOutput = value&0b10000000;
			PPU.t &= 0xF3FF;
			PPU.t |= ((value & 3)<<10);
		}

		//OAM Data
		else if (mAddr == 0x2004){
			PPU.OAM[memory[0x2003]] = value;
			memory[0x2003]++;
		}

		//PPU Scroll
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
		//PPU Address
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
				//clock mmc3 mapper
				CART.readChrMem(PPU.v);
			}
		}
		//PPU Data
		else if (mAddr == 0x2007){
			PPUWrite(PPU.v,value);
			if (memory[0x2000] & 0b00000100)
				PPU.v += 32;
			else if ((memory[0x2000] & 0b00000100) == 0)
				PPU.v++;
			//clock mmc3 mapper
			CART.readChrMem(PPU.v);
		}
		//OAM DMA
		else if (mAddr == 0x4014){
			int oamAddr = memory[0x2003];
			for (int i=0;i<256;i++){
				int index = oamAddr+i;
				if (index >= 256)
					index = index%256;
				PPU.OAM[index] = memory[(value*256)+i];
			}
			//PPU.Clock(513*3);
		}

		//Pulse
		auto pulseWrite = [this](Pulse *p,int mAddr, int value,int addrOffset){
			if (mAddr == 0x4000+addrOffset){
				p->duty = (value&0xC0)>>6;
				p->envelopeLoop = value&0x20;
				p->constantVolume = value&0x10;
				p->volume = value&0x0F;
			}
			else if (mAddr == 0x4001+addrOffset){
				p->sweepEnable = value&0x80;
				p->sPeriod = (value&0x70)>>4;
				p->sNegate = (value&0x08);
				p->sShift = value&0x07;
				p->sReload = true;
			}
			else if (mAddr == 0x4002+addrOffset){
				p->timer = (p->timer&0x700) + (value&0xFF);
				p->freq = 1789773  / (16 * (double)(p->timer+1));
			}
			else if (mAddr == 0x4003+addrOffset){
				//cout << APU.pulse1.lenCounter << endl;
				p->timer = (p->timer&0xFF) + ((value&0x7)*0x100);
				p->freq = 1789773  / (16 * (double)(p->timer+1));
				if (p->enabled){
					p->lenCounter = APU.lenCounterLookup[(value&0xF8)>>3];
				}
				p->envelopeStart = true;
			}
		};

		pulseWrite(&APU.pulse1,mAddr,value,0);
		pulseWrite(&APU.pulse2,mAddr,value,4);


		//Triangle
		if (mAddr == 0x4008){
			APU.triangle.linControl = value&0x80;
			APU.triangle.linReloadVal = value&0x7F;
		}
		else if (mAddr == 0x400A){
			APU.triangle.timer = (APU.triangle.timer&0x700) + (value&0xFF);
			APU.triangle.freq = 1789773  / (32 * (double)(APU.triangle.timer+1));
			if (value == 0 || value == 1)
				APU.triangle.freq = 0;
		}
		else if (mAddr == 0x400B){
			APU.triangle.timer = (APU.triangle.timer&0xFF) + ((value&0x7)*0x100);
			APU.triangle.freq = 1789773  / (32 * (double)(APU.triangle.timer+1));
			if (value == 0 || value == 1)
				APU.triangle.freq = 0;
			if (APU.triangle.enabled)
				APU.triangle.lengthCounter = APU.lenCounterLookup[(value&0xF8)>>3];
			APU.triangle.linReloadFlag = true;
		}
		//Noise
		else if (mAddr == 0x400C){
			APU.noise.envelopeLoop = value&0x20;
			APU.noise.constantVolume = value&0x10;
			APU.noise.volume = value&0x0F;
		}
		else if (mAddr == 0x400E){
			APU.noise.mode = value&0x80;
			APU.noise.period = value&0x0F;
		}
		else if (mAddr == 0x400F){
			if (APU.noise.enabled)
				APU.noise.lenCounter = APU.lenCounterLookup[(value&0xF8)>>3];
			APU.noise.envelopeStart = true;
		}
		//DMC
		else if (mAddr == 0x4010){
			APU.dmc.irqEnabled = value&0x80;
			APU.dmc.irqFlag &= value&0x80;
			APU.dmc.loopFlag = value&0x40;
			APU.dmc.timer = APU.dmcRateLookup[value&0xf]-1;
		}
		else if (mAddr == 0x4011){
			APU.dmc.output = value&0x7F;
		}
		else if (mAddr == 0x4012){
			APU.dmc.sampleAddress = 0xC000 + (value*64);
		}
		else if (mAddr == 0x4013){
			APU.dmc.sampleLength = (value*16) + 1;
			//cout << APU.dmc.sampleLength << endl;
		}
		//APU enable
		else if (mAddr == 0x4015)
			APU.setChannelEnables(value);
		else if (mAddr == 0x4016){
			CONTRL.pollController(value,0);
			CONTRL.pollController(value,1);
		}
		else if (mAddr == 0x4017){
			APU.frameCounter.count = 0;
			APU.frameCounter.mode = value&0x80;
			APU.frameCounter.irqInhibit = value&0x40;
			if (value&0x40)
				APU.frameCounter.irqFlag = false;

			if (value&0x80){
				APU.halfFrame();
				APU.quartFrame();
			}
		}

		memory[addr] = value;
		memory[mAddr] = value;

		// if (mAddr == 0x2001)
		// 	std::cout << memory[mAddr] << ' ' << PPU.scanlines << ' ' << PPU.cycles << std::endl;
}

int NES::PeekPPUMemory(int addr){
	int mAddr = addr;
	if (addr >= 0x3f20)
		mAddr = addr%0x3f20+0x3f00;
	else if (addr >= 0x3000 && addr < 0x3F00)
		mAddr = addr%0x3000+0x2000;
	else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
		mAddr = addr - 0x0010;

	//Pattern tables
	if (mAddr <= 0x1FFF)
		return CART.PeekChrMem(mAddr);
	//Nametables
	else if (mAddr >= 0x2000 && mAddr <= 0x2FFF){
		int mMode = CART.getMirrorMode();
		//One Screen mirroring
		if (!mMode)
			return PPU.ntRAM[mAddr%0x400];
		//Vertical mirroring
		else if (mMode == 1){
			if (mAddr>=0x2800 && mAddr <= 0x2BFF)
				return PPU.ntRAM[(mAddr%0x400)];
			else if (mAddr >= 0x2C00)
				return PPU.ntRAM[0x400+(mAddr%0x400)];
			else
				return PPU.ntRAM[mAddr%0x800];
		}
		//Horizontal mirroring
		else if (mMode == 2){
			if (mAddr<=0x27FF)
				return PPU.ntRAM[mAddr%0x400];
			else if (addr >= 0x2800)
				return PPU.ntRAM[0x400 + mAddr%0x400];
			else
				return PPU.ntRAM[mAddr%0x400];
		}
	}
	//pallet
	else if (mAddr >= 0x3F00 && mAddr <= 0x3F1F)
		return PPU.palletRAM[mAddr%0x20];

	return -1;//PPU.memory[mAddr];
}

//Read from PPU Memory
int NES::PPURead(int addr) {
	//Set up memory mirrors
	int mAddr = addr;
	if (addr >= 0x3f20)
		mAddr = addr%0x3f20+0x3f00;
	else if (addr >= 0x3000 && addr < 0x3F00)
		mAddr = addr%0x3000+0x2000;
	else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
		mAddr = addr - 0x0010;

	//Pattern tables
	if (mAddr <= 0x1FFF)
		return CART.readChrMem(mAddr);
	//Nametables
	else if (mAddr >= 0x2000 && mAddr <= 0x2FFF){
		int mMode = CART.getMirrorMode();
		//One Screen mirroring
		if (!mMode)
			return PPU.ntRAM[mAddr%0x400];
		//Vertical mirroring
		else if (mMode == 1){
			if (mAddr>=0x2800 && mAddr <= 0x2BFF)
				return PPU.ntRAM[(mAddr%0x400)];
			else if (mAddr >= 0x2C00)
				return PPU.ntRAM[0x400+(mAddr%0x400)];
			else
				return PPU.ntRAM[mAddr%0x800];
		}
		//Horizontal mirroring
		else if (mMode == 2){
			if (mAddr<=0x27FF)
				return PPU.ntRAM[mAddr%0x400];
			else if (addr >= 0x2800)
				return PPU.ntRAM[0x400 + mAddr%0x400];
			else
				return PPU.ntRAM[mAddr%0x400];
		}
	}
	//pallet
	else if (mAddr >= 0x3F00 && mAddr <= 0x3F1F)
		return PPU.palletRAM[mAddr%0x20];

	return -1;//PPU.memory[mAddr];
}

//Write to PPU memory
void NES::PPUWrite(int addr, int value){
	int mAddr = addr;
	//Set up memory mirroring
	if (addr >= 0x3f20)
		mAddr = addr%0x20+0x3f00;
	else if (addr >= 0x3000 && addr < 0x3F00)
		mAddr = addr%0x3000+0x2000;
	else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
		mAddr = addr - 0x0010;

	//Pattern tables
	if (mAddr <= 0x1FFF)
		CART.cartWrite(mAddr,value);
	//NameTables
	else if (mAddr >= 0x2000 && mAddr <= 0x2FFF){
		int mMode = CART.getMirrorMode();
		//One Screen mirroring
		if (!mMode)
			PPU.ntRAM[mAddr%0x400] = value;
		//Vertical mirroring
		else if (mMode == 1){
			if (mAddr>=0x2800 && mAddr <= 0x2BFF)
				PPU.ntRAM[mAddr%0x400] = value;
			else if (mAddr >= 0x2C00)
				PPU.ntRAM[0x400+(mAddr%0x400)] = value;
			else
				PPU.ntRAM[mAddr%0x800] = value;
		}
		//Horizontal mirroring
		else if (mMode == 2){
			if (mAddr<=0x27FF)
				PPU.ntRAM[mAddr%0x400] = value;
			else if (mAddr >= 0x2800)
				PPU.ntRAM[0x400 + mAddr%0x400] = value;
			else
				PPU.ntRAM[mAddr%0x400] = value;
		}
	}
	//pallet
	else if (mAddr >= 0x3F00 && mAddr <= 0x3F1F){
		PPU.palletRAM[mAddr%0x20] = value;
	}

	// PPU.memory[mAddr] = value;
	// PPU.memory[addr] = value;
}

void NES::saveState(){
	ofstream file;
	string fileName = "./SaveStates/"+CART.name;
	fileName.erase(fileName.length()-4,4);
	file.open(fileName,ios::binary);
	char x[4];
	//CPU
	CPU.SaveState(file);
	//Memory
	file.write(reinterpret_cast<char*>(&memory[0]),0x10000*sizeof(int));
	//PPU
	PPU.SaveState(file);
	//APU
	APU.SaveState(file);
	//Cartridge
	CART.SaveState(file);

	file.close();
}

void NES::loadState(){
	ifstream file;
	string fileName = "./SaveStates/"+CART.name;
	fileName.erase(fileName.length()-4,4);
	file.open(fileName,ios::binary);
	if (!file){
		cout << "No SaveState" << endl;
		return;
	}
	char *x = new char[4];
	x[0] = 0;
	x[1] = 0;
	x[2] = 0;
	x[3] = 0;
	//CPU
	CPU.LoadState(file);
	//Memory
	file.read(reinterpret_cast<char*>(&memory[0]),0x10000*sizeof(int));
	//PPU
	PPU.LoadState(file);
	//APU
	APU.LoadState(file);
 	//Cartridge
	CART.LoadState(file);

	file.close();
}
