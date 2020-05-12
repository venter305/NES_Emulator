#ifndef PPU_H_
#define PPU_H_

#include "./GraphicsEngine/GUI/panel.h"

class NES;

class ppu{

	public:
		NES *nes;
		//Memory
		int memory[0x4000];
		int OAM[0x100];
		int ntRAM[0x800];
		
		//PPU counters
		int cycles;
		int scanlines;
		int tmpScanline;
		int currScanline;
		int frames;
		bool frameComplete;
		
		//Graphics
		GLubyte *pallet[64];
		GLubyte pixelVal[240][256][3];
		
		//PPU internal registers
		int v,t,x,w;
		int vBuffer;
		bool vBlank;
	
		//pointers to PPU registers
		int *ppuReg2000;
		int *ppuReg2001;
		int *ppuReg2002;	
		
		//Rendering
		//Background registers
		int bgShiftReg16[2];
		int bgShiftReg8[2];
		int bgLatch[2];
		//Sprite registers
		int secOAM[8];
		int sShiftReg8_1[8];
		int sShiftReg8_2[8];
		int sActiveLatch;
		int sLatch[8];
		int sCount[8];
		int numSprites;
		int sMask;
		bool s0Hit;

		int ntAddr;
		int attrAddr,attrAddr1;
		int attrQuad,attrQuad1;
		int chrAddr,chrAddr1;

		bool locked = false;
		

	ppu();
	
	void reset();
	void drawBackground(Panel*);
	void drawSprites();
	void drawChars(Panel*);
	void drawPixel();

	void runCycles(int);
	void clock(int);
};
#endif
