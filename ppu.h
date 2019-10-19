#ifndef PPU_H_
#define PPU_H_

#include <GL/glut.h>
#include <GL/gl.h>

class NES;

class ppu{

	public:
		NES *nes;
		int *memory;
		int *OAM;
		int *ntRAM;
		//int *cpuReg;
		//bool addrLO;
		int cycles;
		int scanlines;
		int frames;
		GLubyte **pallet;
		float *pixelVal;
		int v,t,x,w;
		int vBuffer;
		bool vBlank;
		int tmpScanline;
		int *ppuReg2000;
		int *ppuReg2001;
		int *ppuReg2002;	

		int *bgShiftReg16;
		int *bgShiftReg8;
		int *bgLatch;
		int *secOAM;
		int *sShiftReg8_1;
		int *sShiftReg8_2;
		int sActiveLatch;
		int *sLatch;
		int *sCount;
		bool s0Hit;

		int currScanline;

		bool frameComplete;

	ppu();
	
	void reset();
	void drawBackground();
	void drawSprites();
	void drawFrame();
	void drawScanline();
	void drawPixel();
	void setWriteAddr(int);
	void write(int);

	void cycle(int);

};
#endif
