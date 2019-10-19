#include <iostream>
#include "NES.h"
#include "cartridge.h"
#include <fstream>


using namespace std;

cartridge::cartridge(){
	header = new int[16];
	chrMemory = 0;
	prgMemory = 0;
	Mapper = 0;
	ntMirrorMode = 1;
}

cartridge::~cartridge(){
	saveGame();
	delete[] header;
}

void cartridge::reset() {
	saveGame();
	delete[] prgMemory;
	delete[] chrMemory;
	delete Mapper;
};

void cartridge::readRom(string fileName){
	ifstream rom;
	rom.open(fileName, ios::binary);
	
	name = fileName;
	
	for (int i=0;i<16;i++){
		char x;
		rom.read(&x,1);
		header[i] = (int)(unsigned char)x;
	}
	
	ntMirrorMode = (header[6]&1)?1:2;

	switch(((header[6]&0xF0)>>4)+(header[7]&0xF0)){
		case 0: 
			Mapper = new mapper_000(header[4],header[5]);
			break;
		case 1:
			Mapper = new mapper_001(header[4],header[5]);
			break;
		case 2:
			Mapper = new mapper_002(header[4],header[5]);
			break;
	}
	
	int prg_rom = 0;
	prg_size = 0x4000*header[4]+(0x2000*(header[6]&2));
	prgMemory = new int[prg_size];
	while (rom && prg_rom < (prg_size)) {
		char x;
		if (prg_rom < 0x2000 && header[6]&2)
			x = 0;
		else
			rom.read(&x,1);
		prgMemory[prg_rom] = (int)(unsigned char)x;
		prg_rom++;
	}
	
	if (header[5] == 0){
		chr_size = 0x4000;
		chrMemory = new int[0x4000];
		for (int i=0;i<0x4000;i++)
			chrMemory[i] = 0;
	}
	else{
		chr_size = 0x2000*header[5];
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
}

void cartridge::cartWrite(int addr,int value){
	int wAddr = 0;
	if (addr <= 0x1FFF){
		wAddr = Mapper->ppuWrite(addr,value);
		chrMemory[wAddr] = value;
	}
	else {
		wAddr = Mapper->write(addr,value);
		if (wAddr != -1)
			prgMemory[wAddr] = value;

	}
}

int cartridge::readPrgMem(int addr){
	return prgMemory[Mapper->getPrgAddr(addr)];
}

int cartridge::readChrMem(int addr){
	return chrMemory[Mapper->getChrAddr(addr)];
}

int cartridge::getMirrorMode(){
	if (Mapper->getNtMirrorMode() != -1)
		ntMirrorMode = Mapper->getNtMirrorMode();
	
	return ntMirrorMode;
}

void cartridge::saveGame(){
	if (!(header[6]&2))
		return;
	
	ofstream saveFile;
	string fileName = "./Saves/"+name;
	fileName.replace(fileName.end()-3,fileName.end(),"sav");
	saveFile.open(fileName,ios::binary);
	
	for (int i=0x6000;i<0x8000;i++){
		char x = nes->readMemory(i);
		saveFile.write(&x,1);
	}
	
	saveFile.close();
}

void cartridge::loadGame(){
	if (!(header[6]&2))
		return;

	ifstream saveFile;
	string fileName = "./Saves/"+name;
	fileName.replace(fileName.end()-3,fileName.end(),"sav");
	saveFile.open(fileName,ios::binary);
	if (!saveFile)
		return;
	for (int i=0x6000;i<0x8000;i++){
		char x;
		saveFile.read(&x,1);
		nes->writeMemory(i,(int)(unsigned char)x);
	}

	saveFile.close();
		
}
