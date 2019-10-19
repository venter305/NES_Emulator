#ifndef CART_H_
#define CART_H_
#include <string>

#include "./Mappers/mapper_000.h"
#include "./Mappers/mapper_001.h"
#include "./Mappers/mapper_002.h"

class NES;

class cartridge{
	public:
		int *header;
		int *chrMemory;
		int *prgMemory;

		int prg_size;
		int chr_size;
	
		std::string name;

		NES *nes;
		mapper *Mapper;
		/*
			0: one-screen
			1: Vertical
			2: Horizontal
		*/
		int ntMirrorMode;
		
		cartridge();
		~cartridge();
		void reset();
		void readRom(std::string);
		int readPrgMem(int);
		int readChrMem(int);
		void cartWrite(int,int);
		int getMirrorMode();

		void saveGame();
		void loadGame();

		
};

#endif
