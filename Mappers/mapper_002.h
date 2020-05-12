#ifndef MAPPER_002_
#define MAPPER_002_

#include "mapper.h"
	
class mapper_002: public mapper{
	public:
		//Registers
		int prgBank;
	
		mapper_002(int,int);
		int getPrgAddr(int) override;
		int getChrAddr(int) override;
		int write(int,int) override;
		int ppuWrite(int,int) override;
		int getNtMirrorMode() override;
		void saveMapState(std::ofstream*,char*) override;
		void loadMapState(std::ifstream*,char*) override;
};

#endif
