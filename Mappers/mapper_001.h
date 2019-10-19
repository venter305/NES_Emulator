#ifndef MAPPER_001_
#define MAPPER_001_
#include "mapper.h"

class mapper_001: public mapper{
	public:
		int shiftReg;
		int control;
		int chrBank0;
		int chrBank1;
		int prgBank;
		
		
		
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
