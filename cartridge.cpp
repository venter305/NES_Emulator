#include <iostream>
#include "NES.h"
#include "cartridge.h"
#include <fstream>


using namespace std;

cartridge::cartridge(){
	//Init
	chrMemory = 0;
	prgMemory = 0;
	mapper = 0;
	ntMirrorMode = 1;
}

cartridge::~cartridge(){
	saveGame();
	delete[] prgMemory;
	delete[] chrMemory;
	delete mapper;
}

//Reset Cartridge
void cartridge::reset() {
	saveGame();
	delete[] prgMemory;
	delete[] chrMemory;
	delete mapper;
	readRom(path);
};

//Read a Rom from a file
bool cartridge::readRom(string fileName){
	//File input
	ifstream rom;
	rom.open(fileName, ios::binary);

	if (!rom)
		return false;

	path = fileName;

	name = fileName.substr(fileName.find('/')+1);

	//Read Header
	uint8_t tmp[16];
	rom.read((char *)&tmp,16);
	header.write(tmp);

	std::string headerTest;
	headerTest.insert(0,(char*)header.data,3);
	if(headerTest.compare("NES"))
		return false;

	ntMirrorMode = (header.MIRROR)?1:2;

	//Load Mapper
	switch(header.lMAPPER +(header.hMAPPER*0x10)){
		case 0:
			mapper = new Mapper_000(header.sPRGROM,header.sCHRROM);
			break;
		case 1:
			mapper = new Mapper_001(header.sPRGROM,header.sCHRROM);
			mapper->prgRAM = header.PRGRAM;
			break;
		case 2:
			mapper = new Mapper_002(header.sPRGROM,header.sCHRROM);
			break;
		case 4:
			mapper = new Mapper_004(header.sPRGROM,header.sCHRROM);
			break;
	  default:
			mapper = nullptr;
			std::cout << "Mapper " << (header.lMAPPER +(header.hMAPPER*0x10)) << " Not Supported" << std::endl;
			return false;
	}

	if (header.PRGRAM)
		mapper->prgRAM = true;

	//Load PRG ROM
	int prg_rom = 0;
	prg_size = 0x4000*header.sPRGROM+(0x2000*header.PRGRAM);
	prgMemory = new int[prg_size];
	while (rom && prg_rom < (prg_size)) {
		char x;
		if (prg_rom < 0x2000 && header.PRGRAM)
			x = 0;
		else
			rom.read(&x,1);
		prgMemory[prg_rom] = (int)(unsigned char)x;
		prg_rom++;
	}

  //Load CHR ROM/RAM
	//RAM
	if (header.sCHRROM == 0){
		chr_size = 0x4000;
		chrMemory = new int[0x4000];
		for (int i=0;i<0x4000;i++)
			chrMemory[i] = 0;
	}
	//ROM
	else{
		chr_size = 0x2000*header.sCHRROM;
		int chr_rom = 0;
		chrMemory = new int[chr_size];
		while (rom && chr_rom < (chr_size)) {
			char x;
			rom.read(&x,1);
			chrMemory[chr_rom] = (int)(unsigned char)x;
			chr_rom++;
		}
	}

	loadGame();

	rom.close();

	return true;
}

//Write to cartridge
void cartridge::cartWrite(int addr,int value){
	//Write to CHR ROM/RAM
	if (addr <= 0x1FFF){
		chrMemory[mapper->PpuWrite(addr,value)] = value;
	}
	//Write to PRG ROM
	else {
		int wAddr = mapper->Write(addr,value);
		if (wAddr != -1)
			prgMemory[wAddr] = value;

	}
}

int cartridge::PeekPrgMem(int addr){
	return prgMemory[mapper->PeekPrgAddr(addr)];
}

//Read from PRG ROM
int cartridge::readPrgMem(int addr){
	return prgMemory[mapper->GetPrgAddr(addr)];
}

int cartridge::PeekChrMem(int addr){
	return chrMemory[mapper->PeekChrAddr(addr)];
}

//Read from CHR ROM/RAM
int cartridge::readChrMem(int addr){
	return chrMemory[mapper->GetChrAddr(addr)];
}

//Read Mirroring Mode
int cartridge::getMirrorMode(){
	if (mapper->GetNtMirrorMode() != -1)
		ntMirrorMode = mapper->GetNtMirrorMode();

	return ntMirrorMode;
}

//Check Interrupts
bool cartridge::PollInterrupts(){
	return mapper->PollInterrupts();
}

//Write the save data to a file
void cartridge::saveGame(){
	//Return if there is no save data
	if (!(header.PRGRAM))
		return;

	//Set up output file
	ofstream saveFile;
	string fileName = "./Saves/"+name;
	fileName.replace(fileName.end()-3,fileName.end(),"sav");
	saveFile.open(fileName,ios::binary);

	//Write to file
	for (int i=0x6000;i<0x8000;i++){
		char x = nes->readMemory(i);
		saveFile.write(&x,1);
	}

	saveFile.close();
}

//Load the save data from a file
void cartridge::loadGame(){
	//Return if there is no save data
	if (!(header.PRGRAM))
		return;

	//Set up Input file
	ifstream saveFile;
	string fileName = "./Saves/"+name;
	fileName.replace(fileName.end()-3,fileName.end(),"sav");
	saveFile.open(fileName,ios::binary);
	if (!saveFile)
		return;

	//Read from file
	for (int i=0x6000;i<0x8000;i++){
		char x;
		saveFile.read(&x,1);
		nes->writeMemory(i,(int)(unsigned char)x);
	}

	saveFile.close();

}

void cartridge::SaveState(std::ofstream &file){
	file.write(reinterpret_cast<char*>(&header.data[0]),16);
	file.write(reinterpret_cast<char*>(&prgMemory[0]),prg_size*sizeof(int));
	file.write(reinterpret_cast<char*>(&chrMemory[0]),chr_size*sizeof(int));
	file.write(reinterpret_cast<char*>(&ntMirrorMode),1);
	mapper->SaveMapState(file);
}

void cartridge::LoadState(std::ifstream &file){
	file.read(reinterpret_cast<char*>(&header.data[0]),16);
	file.read(reinterpret_cast<char*>(&prgMemory[0]),prg_size*sizeof(int));
	file.read(reinterpret_cast<char*>(&chrMemory[0]),chr_size*sizeof(int));
	file.read(reinterpret_cast<char*>(&ntMirrorMode),1);
	mapper->LoadMapState(file);
}
