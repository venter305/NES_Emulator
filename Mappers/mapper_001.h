#pragma once

//MMC1

#include "mapper.h"

class Mapper_001: public Mapper{
	public:
		//Control Registers
		int shiftReg = 0x10;
		int control = 0x0C;
		int chrBank0 = 0;
		int chrBank1 = 0;
		int prgBank = 0;

		//Control
		enum {
			CTL_MIRROR =  0b00000011,
			CTL_PRGBANK = 0b00001100,
			CTL_CHRBANK = 0b00010000,

			PRG_BANK = 0b00001111,
			PRG_RAM =  0b00010000,

			CHR_BANK = 0b00011110
		};



		Mapper_001(int prgBanks, int chrBanks): Mapper(prgBanks,chrBanks){}

		int GetPrgAddr(int addr) override {
			//RAM
			if (addr >= 0x6000 && addr <= 0x7FFF){
				return addr-0x6000;
			}
			if ((control&CTL_PRGBANK)>>2 <= 1){
				//32kb bank switching
				return ((addr-0x8000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
			}
			if ((control&CTL_PRGBANK)>>2 == 2){
				//fix first bank (0x8000), switch second bank(0xC000)
				if (addr >= 0x8000 && addr <= 0xBFFF)
					return (addr-0x8000)+0x2000*(prgRAM);
				return ((addr-0xC000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
			}
			if ((control&CTL_PRGBANK)>>2 == 3){
				//fix last bank (0xC000), switch first bank(0x8000)
				if (addr >= 0x8000 && addr <= 0xBFFF)
					return ((addr-0x8000)+((prgBank&PRG_BANK)*0x4000))+0x2000*(prgRAM);
				return ((addr-0xC000)+((numPrgBanks-1)*0x4000))+0x2000*(prgRAM);
			}
			return addr;
		}

		int GetChrAddr(int addr) override {
			//8KB switching
			if ((control&CTL_CHRBANK)==0){
				return addr+((chrBank0&CHR_BANK)*0x1000);
			}
			//4KB switching
			if (addr <= 0x0FFF){
				return addr+(chrBank0*0x1000);
			}

			return (addr%0x1000)+(chrBank1*0x1000);
		}

		int Write(int addr,int value) override{
			//RAM, return offset address
			if (addr < 0x8000 && addr >= 0x6000) {
				return addr-0x6000;
			}

			//Load register
			//reset shift Reg and lock top PRG bank
			if (value&0x80){
				shiftReg = 0x10;
				control |= 0xC;
			}
			//After 5 bits have been shifted in to register
			else if (shiftReg&1){
				int tmpVal = ((shiftReg&0x1e)>>1) + (value&1)*0x10;
				if (addr >= 0x8000 && addr <= 0x9FFF)
					control = tmpVal;
				else if (addr >= 0xA000 && addr <= 0xBFFF)
					chrBank0 = tmpVal;
				else if (addr >= 0xC000 && addr <= 0xDFFF)
					chrBank1 = tmpVal;
				else if (addr >= 0xE000 && addr <= 0xFFFF)
					prgBank = tmpVal;
				shiftReg = 0x10;
			}
			//Shift value into shift register
			else{
				shiftReg>>=1;
				shiftReg |= (value&1)*0x10;
			}
			return -1;
		}

		int PpuWrite(int addr,int value) override {return GetChrAddr(addr);}
		int GetNtMirrorMode() override {
			int mMode = (control&CTL_MIRROR) - 1;
			if (mMode < 0)
				mMode = 0;
			return mMode;
		}

		void SaveMapState(std::ofstream &file) override {
			//Number of banks
			file.write(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.write(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
			//Registers
			file.write(reinterpret_cast<char*>(&shiftReg),sizeof(int));
			file.write(reinterpret_cast<char*>(&control),sizeof(int));
			file.write(reinterpret_cast<char*>(&chrBank0),sizeof(int));
			file.write(reinterpret_cast<char*>(&chrBank1),sizeof(int));
			file.write(reinterpret_cast<char*>(&prgBank),sizeof(int));
		}

		void LoadMapState(std::ifstream &file) override {
			//Number of banks
			file.read(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
			file.read(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
			//Registers
			file.read(reinterpret_cast<char*>(&shiftReg),sizeof(int));
			file.read(reinterpret_cast<char*>(&control),sizeof(int));
			file.read(reinterpret_cast<char*>(&chrBank0),sizeof(int));
			file.read(reinterpret_cast<char*>(&chrBank1),sizeof(int));
			file.read(reinterpret_cast<char*>(&prgBank),sizeof(int));
		}

};
