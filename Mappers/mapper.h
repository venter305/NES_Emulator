#pragma once

#include <fstream>

class Mapper{
	public:

		int numPrgBanks;
		int numChrBanks;

		bool prgRAM = false;

		Mapper(int prgBanks, int chrBanks){
				numPrgBanks = prgBanks;
				numChrBanks = chrBanks;
		}

		//Clock
		virtual void Clock(){}
		//read
		virtual int GetPrgAddr(int addr) = 0;
		virtual int GetChrAddr(int addr) = 0;
		virtual int PeekPrgAddr(int addr){return GetPrgAddr(addr);}
		virtual int PeekChrAddr(int addr){return GetChrAddr(addr);}
		//write
		virtual int Write(int addr, int value) = 0;
		virtual int PpuWrite(int addr, int value) = 0;
		virtual int GetNtMirrorMode() = 0;
		virtual bool PollInterrupts(){return false;}
		//Save/Load
		virtual void SaveMapState(std::ofstream &file) = 0;
		virtual void LoadMapState(std::ifstream &file) = 0;
};
