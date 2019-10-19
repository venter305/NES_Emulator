#ifndef NES_H_
#define NES_H_
#include <iostream>
#include <GLFW/glfw3.h>
#include "cpu.h"
#include "ppu.h"
#include "controller.h"
#include "cartridge.h"

class NES{
	
	public:
		cpu CPU;
		ppu PPU;
		controller CONTRL;
		cartridge CART;
		GLFWwindow *window;
		

		int *memory;
		
		NES();
		~NES();
		void reset();
		int readMemory(int addr);
		int PPURead(int addr);
		void writeMemory(int addr, int value);
		void PPUWrite(int addr, int value);
	

};
#endif
