#ifndef MAPPER_001_
#define MAPPER_001_
#include "mapper.h"

class mapper_001: public mapper{
	public:
		//Control Registers
		int shiftReg;
		int control;
		int chrBank0;
		int chrBank1;
		int prgBank;
		
		//Control 
		enum {
			CTL_MIRROR =  0b00000011,
			CTL_PRGBANK = 0b00001100,
			CTL_CHRBANK = 0b00010000,	

			PRG_BANK = 0b00001111,
			PRG_RAM =  0b00010000,
	
			CHR_BANK = 0b00011110
		};
		
		
		
		mapper_001(int,int);
		int getPrgAddr(int) override;
		int getChrAddr(int) override;
		int write(int,int) override;
		int ppuWrite(int,int) override;
		int getNtMirrorMode() override;
		void saveMapState(std::ofstream*,char*) override;
		void loadMapState(std::ifstream*,char*) override;

};

#endif
