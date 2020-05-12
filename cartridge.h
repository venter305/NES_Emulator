#ifndef CART_H_
#define CART_H_
#include <string>

#include "./Mappers/mapper_000.h"
#include "./Mappers/mapper_001.h"
#include "./Mappers/mapper_002.h"

class NES;

class cartridge{
	public:
		//Memory
		int *chrMemory;
		int *prgMemory;

		//Header 
		struct HEADER{
			uint8_t data[16];
			uint8_t test = 0;							//Size of PRG ROM
			uint8_t sPRGROM = 0;
			uint8_t sCHRROM = 0;							//Size of CHR ROM
			uint8_t FLAGS6 = 0;								//Flags 6
			bool MIRROR = 0;						//type of Mirroring
			bool PRGRAM= 0;			//Enable PGR RAM
			bool TRAINER= 0;			//Enable Trainer
			bool mIGNORE = 0;			//Ignore Mirroring
			uint8_t lMAPPER = 0;		//Lower nibble of mapper
			uint8_t FLAGS7 = 0;							//Flags 7
			bool VSUNI = 0;						//VS Unisystem
			bool PC10 = 0;				//PlayChoice-10
			bool NES2 = 0;				//NES 2.0
			uint8_t hMAPPER = 0;		//Upper nibble of mapper
			uint8_t FLAGS8 = 0;								//Size of PRG RAM
			uint8_t FLAGS9 = 0;								//Flags 9
			bool TV = 0;								//Tv system
			
			void write(uint8_t d[16]){
				for (int i=0;i<16;i++)
					data[i] = d[i];
				sPRGROM = data[4];						
				sCHRROM = data[5];							
				FLAGS6 = data[6];								
				MIRROR = data[6]&0x1;						
				PRGRAM = (data[6]&0x2) >> 1;			
				TRAINER = (data[6]&0x4) >> 2;			
				mIGNORE = (data[6]&0x8) >> 3;			
				lMAPPER = (data[6]&0xF0) >> 4;		
				FLAGS7 = data[7];								
				VSUNI = data[7]&0x1;						
				PC10 = (data[7]&0x2) >> 1;				
				NES2 = (data[7]&0xC) >> 2;				
				hMAPPER = (data[7]&0xF0) >> 4;		
				FLAGS8 = data[8];								
				FLAGS9 = data[9];								
				TV = data[9]&0x1;								
				}
		} header;
		
		//Info
		int prg_size;
		int chr_size;
		/*
			0: one-screen
			1: Vertical
			2: Horizontal
		*/
		int ntMirrorMode;
	
		std::string name;
		mapper *Mapper;

		NES *nes;
		
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
