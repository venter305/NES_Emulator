#ifndef MAPPER
#define MAPPER

#include <fstream>

class mapper{
	public:

		int numPrgBanks;
		int numChrBanks;
		
		mapper(int,int);
		virtual int getPrgAddr(int addr) = 0;
		virtual int getChrAddr(int addr) = 0;
		virtual int write(int addr, int value) = 0;
		virtual int ppuWrite(int addr, int value) = 0;
		virtual int getNtMirrorMode() = 0;
		virtual void saveMapState(std::ofstream *file,char *x) = 0;
		virtual void loadMapState(std::ifstream *file,char *x) = 0;
};

#endif
