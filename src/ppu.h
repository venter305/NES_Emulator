#pragma once

#include <fstream>

class NES;

class ppu{

	public:
		NES *nes;
		//Memory
		int OAM[0x100];
		int ntRAM[0x800];
		int palletRAM[0x20];

		//PPU counters
		int cycles;
		int scanlines;
		int frames;

		//Graphics
		uint8_t pallet[64][3] {
			{84,84,84},
			{0,30,116},
			{8,16,144},
			{48,0,136},
			{68,0,100},
			{92,0,48},
			{84,4,0},
			{60,24,0},
			{32,42,0},
			{8,58,0},
			{0,64,0},
			{0,60,0},
			{0,50,60},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{152,150,152},
			{8,76,196},
			{40,50,236},
			{92,30,228},
			{136,20,176},
			{160,20,100},
			{152,34,32},
			{120,60,0},
			{84,90,0},
			{40,114,0},
			{8,124,0},
			{0,118,40},
			{0,102,120},
			{0,0,0},
			{0,0,0},
			{0,0,0},
			{236,238,236},
			{76,154,236},
			{120,124,236},
			{176,98,236},
			{228,84,236},
			{236,88,180},
			{236,106,100},
			{212,136,32},
			{160,170,0},
			{116,196,0},
			{76,208,32},
			{56,204,108},
			{56,180,204},
			{60,60,60},
			{0,0,0},
			{0,0,0},
			{236,238,236},
			{168,204,236},
			{188,188,236},
			{212,178,236},
			{236,174,236},
			{236,174,212},
			{236,180,176},
			{228,196,144},
			{204,210,120},
			{180,222,120},
			{168,226,144},
			{152,226,180},
			{160,214,228},
			{160,162,160},
			{0,0,0},
			{0,0,0}
		};

	 	uint8_t lookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

		uint8_t pixelVal[240][256][3];

		//PPU internal registers
		int v,t,x,w;
		int vBuffer;
		bool vBlank;

		bool nmiOccured;
		bool nmiOutput;

		//Rendering
		//Background registers
		int bgShiftReg16[2];
		int bgShiftReg8[2];
		int bgLatch[2];
		//Sprite registers
		//int secOAM[8];
		int sShiftReg8_1[8];
		int sShiftReg8_2[8];
		int sActiveLatch;
		int sLatch[8];
		int sCount[8];
		int numSprites;
		//int sMask;
		int firstSpriteOAMAddr = 0;

		int bgTile;
		int attrByte;
		int bgPatternLo,bgPatternHi;


	ppu();

	void Reset();
	void DrawNametable(int num, uint8_t *buffer);
	void DrawSprites(uint8_t *buffer);
	void DrawChars(int num,uint8_t *buffer,bool grayscale = false);
	void DrawPixel();
	void Clock(int);

	void EvaluateSprites();

	void SaveState(std::ofstream &file);
	void LoadState(std::ifstream &file);
};
