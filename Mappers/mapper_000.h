#ifndef MAPPER_000_
#define MAPPER_000_
#include "mapper.h"

class mapper_000 : public mapper{
	
	public:
	mapper_000(int,int);
	
	int getPrgAddr(int) override;
	int getChrAddr(int) override;
	int write(int,int) override;
	int ppuWrite(int,int) override;
	int getNtMirrorMode() override;
	void saveMapState(std::ofstream*,char*) override;
	void loadMapState(std::ifstream*,char*) override;
	
};

#endif
