#pragma once

#include "mapper.h"

class Mapper_000 : public Mapper{

	public:
		Mapper_000(int prgBanks, int chrBanks): Mapper(prgBanks,chrBanks){}

		int GetPrgAddr(int addr) override {
			int mAddr = addr;
			//Choose 16k or 32k mapping
			if (addr >= 0x8000){
				return (numPrgBanks==1)?(addr%0x4000):(addr-0x8000);
			}
			return addr;
		}

		int GetChrAddr(int addr) override {return addr;}
		int Write(int addr,int value) override {return 0;}
		int PpuWrite(int addr,int value) override {return addr;}
		int GetNtMirrorMode() override {return -1;}
		void SaveMapState(std::ofstream &file) override {
			file.write(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.write(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
		}
		void LoadMapState(std::ifstream &file) override {
			file.read(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.read(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
		}
};
