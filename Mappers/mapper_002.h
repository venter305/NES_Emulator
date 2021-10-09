#pragma once
#include "mapper.h"

class Mapper_002: public Mapper{
	public:
		//Registers
		int prgBank = 0;

		Mapper_002(int prgBanks, int chrBanks): Mapper(prgBanks,chrBanks){}
		~Mapper_002() = default;
		int GetPrgAddr(int addr) override {
			//Lower bank
			if (addr >= 0x8000 && addr <= 0xBFFF)
				return ((addr-0x8000)+((prgBank&0xF)*0x4000))+0x2000;
			//Upper bank, fixed to last bank
			return ((addr-0xC000)+((numPrgBanks-1)*0x4000))+0x2000;
		}

		int GetChrAddr(int addr) override {return addr;}
		int Write(int addr,int value) override {
			if (addr >= 0x8000)
				prgBank = value;

			return -1;
		}

		int PpuWrite(int addr,int value) override {return addr;}
		int GetNtMirrorMode() override {return -1;}
		void SaveMapState(std::ofstream &file) override {
			file.write(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.write(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
			file.write(reinterpret_cast<char*>(&prgBank),sizeof(int));
		}
		void LoadMapState(std::ifstream &file) override {
			file.read(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.read(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
			file.read(reinterpret_cast<char*>(&prgBank),sizeof(int));
		}
};
