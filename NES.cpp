#include <iostream>
#include "NES.h"
#include <thread>

using namespace std;

NES::NES(){
	//Set up components
	CPU.nes = this;
	CPU.ppuReg2000 = &memory[0x2000];
	CPU.ppuReg2002 = &memory[0x2002];
	PPU.nes = this;
	PPU.ppuReg2000 = &memory[0x2000];
	PPU.ppuReg2001 = &memory[0x2001];
	PPU.ppuReg2002 = &memory[0x2002];
	CART.nes = this;
	APU.nes = this;
}

NES::~NES(){
}

//Reset Emulator
void NES::reset(){
	//Reset memory to 0
	for (int i=0;i<0x10000;i++){
		memory[i] = 0;
	}
	//Reset components
	CPU.reset();
	PPU.reset();
	APU.reset();
	CART.reset();
}

void NES::clock(int cycles){
	while(cycles){
		cpuTmpCycles = CPU.runInstructions();
		PPU.clock(3*cpuTmpCycles);		
		APU.clock(cpuTmpCycles);
		cycles--;
	}
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
	
	//Read from OAM memory
	if (mAddr == 0x2004)
		return PPU.OAM[memory[0x2003]];
	
	return memory[mAddr];
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
	
	return PPU.memory[mAddr];
}

//Write to PPU memory
void NES::PPUWrite(int addr, int value){
	int mAddr = addr;
	//Set up memory mirroring
	if (addr >= 0x3f20)
		mAddr = addr%0x3f20+0x3f00;
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

	PPU.memory[mAddr] = value;
	PPU.memory[addr] = value;
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

		//PPU Control
	  if (mAddr == 0x2000){
			PPU.t &= (0xFFFF-0x0C00);
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
			}
		}
		//PPU Data
		else if (mAddr == 0x2007){
			PPUWrite(PPU.v,value);
			if (readMemory(0x2000) & 0b00000100)
				PPU.v += 32;
			else if ((readMemory(0x2000) & 0b00000100) == 0)
				PPU.v++;	
		}
		//OAM DMA
		else if (mAddr == 0x4014){
			for (int i=0;i<256;i++){
				PPU.OAM[i] = memory[(value*256)+i];
			}
			PPU.cycles += (513*3);
		}
		//Pulse 1
		else if (mAddr == 0x4000){
			APU.pulse1.duty = (value&0xC0)>>6;
			APU.pulse1.envelopeLoop = value&0x20;
			APU.pulse1.constantVolume = value&0x10;
			APU.pulse1.volume = value&0x0F;
		}
		else if (mAddr == 0x4001){
			APU.pulse1.sweepEnable = value&0x80;
			APU.pulse1.sPeriod = (value&0x70)>>4;
			APU.pulse1.sNegate = (value&0x08);
			APU.pulse1.sShift = value&0x07;
			APU.pulse1.sReload = true;
		}
		else if (mAddr == 0x4002){
			APU.pulse1.timer = (APU.pulse1.timer&0x700) + (value&0xFF);
			APU.pulse1.freq = 1789773  / (16 * (double)(APU.pulse1.timer+1));
		}
		else if (mAddr == 0x4003){
			APU.pulse1.timer = (APU.pulse1.timer&0xFF) + ((value&0x7)*0x100);
			APU.pulse1.freq = 1789773  / (16 * (double)(APU.pulse1.timer+1));
			APU.pulse1.lenCounter = APU.lenCounterLookup[(value&0xF8)>>3];
			APU.pulse1.seqMask = 1;
			APU.pulse1.seqOffset = 0;
			APU.pulse1.envelopeStart = true;
		}
		//Pulse 2
		else if (mAddr == 0x4004){
			APU.pulse2.duty = (value&0xC0)>>6;
			APU.pulse2.envelopeLoop = value&0x20;
			APU.pulse2.constantVolume = value&0x10;
			APU.pulse2.volume = value&0x0F;
		}
		else if (mAddr == 0x4005){
			APU.pulse2.sweepEnable = value&0x80;
			APU.pulse2.sPeriod = (value&0x70)>>4;
			APU.pulse2.sNegate = (value&0x08);
			APU.pulse2.sShift = value&0x07;
			APU.pulse2.sReload = true;
		}
		else if (mAddr == 0x4006){
			APU.pulse2.timer = (APU.pulse2.timer&0x700) + (value&0xFF);
			APU.pulse2.freq = 1789773  / (16 * (double)(APU.pulse2.timer+1));
		}
		else if (mAddr == 0x4007){
			APU.pulse2.timer = (APU.pulse2.timer&0xFF) + ((value&0x7)*0x100);
			APU.pulse2.freq = 1789773  / (16 * (double)(APU.pulse2.timer+1));
			APU.pulse2.lenCounter = APU.lenCounterLookup[(value&0xF8)>>3];
			APU.pulse2.seqMask = 1;
			APU.pulse2.seqOffset = 0;
			APU.pulse2.envelopeStart = true;
		}
		//Triangle
		else if (mAddr == 0x4008){
			APU.triangle.linControl = value&0x80;
			APU.triangle.linReloadVal = value&0x7F;
		}
		else if (mAddr == 0x400A){
			APU.triangle.timer = (APU.triangle.timer&0x700) + (value&0xFF);
			APU.triangle.freq = 1789773  / (32 * (double)(APU.triangle.timer+1));
		}
		else if (mAddr == 0x400B){
			APU.triangle.timer = (APU.triangle.timer&0xFF) + ((value&0x7)*0x100);
			APU.triangle.freq = 1789773  / (32 * (double)(APU.triangle.timer+1));
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
			APU.noise.lenCounter = APU.lenCounterLookup[(value&0xF8)>>3];
			APU.noise.envelopeStart = true;
		}
		//APU enable
		else if (mAddr == 0x4015)
			APU.setChannelEnables(value);
		else if (mAddr == 0x4016)
			CONTRL.pollController(value,0);
		else if (mAddr == 0x4017){
			APU.frameCounter.mode = value&0x80;
			APU.frameCounter.irqFlag = value&0x40;
		}
		
		memory[addr] = value;
		memory[mAddr] = value;
}

void NES::saveState(){
	ofstream file;
	string fileName = "./SaveStates/"+CART.name;
	fileName.erase(fileName.length()-4,4);
	file.open(fileName,ios::binary);
	char *x = new char[4];
	//CPU
	x[0] = CPU.a;
	file.write(x,1);
	x[0] = CPU.x;
	file.write(x,1);
	x[0] = CPU.y;
	file.write(x,1);
	x[0] = CPU.s;
	file.write(x,1);
	x[0] = CPU.p;
	file.write(x,1);
	x[0] = (CPU.pc&0xFF00)>>8;
	x[1] = CPU.pc&0xFF;
	file.write(x,2);
	//Memory
	for (int i=0;i<0x10000;i++){
		x[0] = memory[i];
		file.write(x,1);
	}
	//PPU
	x[0] = (PPU.cycles&0xFF00)>>8;
	x[1] = PPU.cycles&0xFF;
	file.write(x,2);
	x[0] = (PPU.scanlines&0xFF000000)>>24;
	x[1] = (PPU.scanlines&0xFF0000)>>16;
	x[2] = (PPU.scanlines&0xFF00)>>8;
	x[3] = PPU.scanlines&0xFF;
	file.write(x,sizeof(int));
	x[0] = (PPU.frames&0xFF000000)>>24;
	x[1] = (PPU.frames&0xFF0000)>>16;
	x[2] = (PPU.frames&0xFF00)>>8;
	x[3] = PPU.frames&0xFF;
	file.write(x,sizeof(int));
	x[0] = (PPU.v&0xFF00)>>8;
	x[1] = PPU.v&0xFF;
	file.write(x,2);
	x[0] = (PPU.t&0xFF00)>>8;
	x[1] = PPU.t&0xFF;
	file.write(x,2);
	x[0] = PPU.x;
	file.write(x,1);
	x[0] = PPU.w;
	file.write(x,1);
	x[0] = PPU.frameComplete;
	file.write(x,1);
	x[0] = PPU.s0Hit;
	file.write(x,1);
	x[0] = (PPU.currScanline&0xFF000000)>>24;
	x[1] = (PPU.currScanline&0xFF0000)>>16;
	x[2] = (PPU.currScanline&0xFF00)>>8;
	x[3] = PPU.currScanline&0xFF;
	file.write(x,sizeof(int));
	x[0] = PPU.vBlank;
	file.write(x,1);
	x[0] = (PPU.tmpScanline&0xFF000000)>>24;
	x[1] = (PPU.tmpScanline&0xFF0000)>>16;
	x[2] = (PPU.tmpScanline&0xFF00)>>8;
	x[3] = PPU.tmpScanline&0xFF;
	file.write(x,sizeof(int));
	for (int i=0;i<0x4000;i++){
		x[0] = PPU.memory[i];
		file.write(x,1);
	}
	for (int i=0;i<0x100;i++){
		x[0] = PPU.OAM[i];
		file.write(x,1);
	}
	for (int i=0;i<0x800;i++){
		x[0] = PPU.ntRAM[i];
		file.write(x,1);
	}
	x[0] = (PPU.bgShiftReg16[0]&0xFF00)>>8;
	x[1] = PPU.bgShiftReg16[0]&0xFF;
	file.write(x,2);
	x[0] = (PPU.bgShiftReg16[1]&0xFF00)>>8;
	x[1] = PPU.bgShiftReg16[1]&0xFF;
	file.write(x,2);
	x[0] = PPU.bgShiftReg8[0];
	file.write(x,1);
	x[0] = PPU.bgShiftReg8[1];
	file.write(x,1);
	x[0] = PPU.bgLatch[0];
	file.write(x,1);
	x[0] = PPU.bgLatch[1];
	file.write(x,1);

	for (int i=0;i<8;i++){
		x[0] = PPU.sShiftReg8_1[i];
		file.write(x,1);
		x[0] = PPU.sShiftReg8_2[i];
		file.write(x,1);
		x[0] = PPU.sLatch[i];
		file.write(x,1);
		x[0] = PPU.sCount[i];
		file.write(x,1);
	}
	
	//Cartridge
	for (int i=0;i<16;i++){
		x[0] = CART.header.data[i];
		file.write(x,1);
	}

	for (int i=0;i<CART.prg_size;i++){
		x[0] = CART.prgMemory[i];
		file.write(x,1);
	}

	for (int i=0;i<CART.chr_size;i++){
		x[0] = CART.chrMemory[i];
		file.write(x,1);
	}

	x[0] = CART.ntMirrorMode;
	file.write(x,1);

	CART.Mapper->saveMapState(&file,x);
	
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
	file.read(x,1);
	CPU.a = (int)(unsigned char)x[0];
	file.read(x,1);
	CPU.x = (int)(unsigned char)x[0];
	file.read(x,1);
	CPU.y = (int)(unsigned char)x[0];
	file.read(x,1);
	CPU.s = (int)(unsigned char)x[0];
	file.read(x,1);
	CPU.p = (int)(unsigned char)x[0];
	file.read(x,2);
	CPU.pc = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	//Memory
	for (int i=0;i<0x10000;i++){
		file.read(x,1);
		memory[i] = (int)(unsigned char)x[0];
	}
	//PPU
	file.read(x,2);
	PPU.cycles = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,sizeof(int));
	PPU.scanlines = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,sizeof(int));
	PPU.frames = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,2);
	PPU.v = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,2);
	PPU.t = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,1);
	PPU.x = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.w = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.frameComplete = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.s0Hit = (int)(unsigned char)x[0];
	file.read(x,sizeof(int));
	PPU.currScanline = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,1);
	PPU.vBlank = (int)(unsigned char)x[0];
	file.read(x,sizeof(int));
	PPU.tmpScanline = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	for (int i=0;i<0x4000;i++){
		file.read(x,1);
		PPU.memory[i] = (int)(unsigned char)x[0];
	}
	for (int i=0;i<0x100;i++){
		file.read(x,1);
		PPU.OAM[i] = (int)(unsigned char)x[0];
	}
	for (int i=0;i<0x800;i++){
			file.read(x,1);
			PPU.ntRAM[i] = (int)(unsigned char)x[0];
	}
	file.read(x,2);
	PPU.bgShiftReg16[0] = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,2);
	PPU.bgShiftReg16[1] = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,1);
	PPU.bgShiftReg8[0] = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.bgShiftReg8[1] = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.bgLatch[0] = (int)(unsigned char)x[0];
	file.read(x,1);
	PPU.bgLatch[1] = (int)(unsigned char)x[0];

	for (int i=0;i<8;i++){
		file.read(x,1);
		PPU.sShiftReg8_1[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		PPU.sShiftReg8_2[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		PPU.sLatch[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		PPU.sCount[i] = (int)(unsigned char)x[0];
	}
	//Cartridge
	for (int i=0;i<16;i++){
		file.read(x,1);
		CART.header.data[i] = (int)(unsigned char)x[0];
	}
	
	for (int i=0;i<CART.prg_size;i++){
		file.read(x,1);
		CART.prgMemory[i] = (int)(unsigned char)x[0];
	}

	for (int i=0;i<CART.chr_size;i++){
		file.read(x,1);
		CART.chrMemory[i] = (int)(unsigned char)x[0];
	}

	file.read(x,1);
	CART.ntMirrorMode = (int)(unsigned char)x[0];

	CART.Mapper->loadMapState(&file,x);

	file.close();
}
