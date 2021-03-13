#ifndef NES_H_
#define NES_H_
#include <iostream>
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "controller.h"
#include "cartridge.h"

class NES{
	
	public:
		//Components
		cpu CPU;
		ppu PPU;
		apu APU;
		controller CONTRL;
		cartridge CART;
		
		GLFWwindow *window;

		int cpuTmpCycles = 0;
		int apuTmpCycles = 0;
		
		int memory[0x10000];
		
		double dTime;
		
		NES();
		~NES();
		void reset();
		bool clock(int);
		int readMemory(int addr);
		int PPURead(int addr);
		void writeMemory(int addr, int value);
		void PPUWrite(int addr, int value);
		void saveState();
		void loadState();
	

};
#endif
