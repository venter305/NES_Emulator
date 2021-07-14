#include <iostream>
#include <thread>
#include "ppu.h"
#include "NES.h"

using namespace std;

ppu::ppu(){
	//Initilize PPU
	Reset();
}

//Reset the PPU
void ppu::Reset(){
	//Memory
	for (int i=0;i<0x800;i++){
		if (i<0x100)
			OAM[i] = 0;
		if (i<0x800)
			ntRAM[i] = 0;
		if (i<0x20)
			palletRAM[i] = 0;
	}
	//Backgound
	bgShiftReg16[0] = 0;
	bgShiftReg16[1] = 0;
	bgShiftReg8[0] = 0;
	bgShiftReg8[1] = 0;
	bgLatch[0] = 0;
	bgLatch[1] = 0;

	//Sprite
	for (int i=0;i<8;i++){
		sShiftReg8_1[i] = 0;
		sShiftReg8_2[i] = 0;
		sLatch[i] = 0;
		sCount[i] = 0;
	}

	//Counters
	cycles = 30;
	scanlines = 0;
	frames = 1;

	//Registers
	v = 0;
	t = 0;
	x = 0;
	w = 0;
	vBuffer = 0;

	//Flags
	s0Hit = false;
	vBlank = false;

	nmiOccured = false;
	nmiOutput = false;

}

//Cycle the PPU
void ppu::Clock(int c){
	int tmpCycles = c;
	auto transferVertical = [&](){
		//Set v equal to t
		v &= ~0x7BE0;
		v |= (t & 0x7BE0);
	};

	auto transferHorizontal = [&](){
		v &= ~0x41f;
		v |= (t & 0x41f);
	};

	auto incrementX = [&](){
		if ((v & 0x001f) == 31){
			v &= ~0x001f;
			v ^= 0x0400;
		}
		else
			v++;
	};

	auto incrementY = [&](){
		int fineY = (v & 0x7000);
		int coarseY = (v & 0x03E0)>>5;
		if (fineY != 0x7000)
			v += 0x1000;
		else{
			v &= ~0x7000;
			if (coarseY == 29){
				coarseY = 0;
				v ^= 0x0800;
			}
			else if (coarseY == 31)
				coarseY = 0;
			else
				coarseY++;
			v = (v & (~0x03e0)) | (coarseY<<5);
		}
	};

	while(tmpCycles > 0){
		cycles++;
		int bgPatternAddr = (nes->peekMemory(0x2000)&0x10)*0x100;
		if (scanlines == -1){
			if (cycles == 0){
				//Reset Sprite 0
				nes->writeMemory(0x2002, nes->peekMemory(0x2002) & 0b10111111);

				//Set up Background Registers
			}
			//Reload the vertical scroll bits
			if ((nes->peekMemory(0x2001) & 0x18)){
				if (cycles == 257)
					transferHorizontal();
				else if (cycles >= 280 && cycles <= 304){
					//Set v equal to t
					transferVertical();
				}
				else if (cycles == 329){
					int tmpBg1 = nes->PPURead(0x2000 | ((v)&0x0FFF));
					int tmpBgLo1 = nes->PPURead(tmpBg1*0x10+bgPatternAddr+(((v)&0x7000)/0x1000));
					int tmpBgHi1 = nes->PPURead(tmpBg1*0x10+bgPatternAddr+ 8 +(((v)&0x7000)/0x1000));
					incrementX();
					int tmpBg2 = nes->PPURead(0x2000 | ((v)&0x0FFF));
					int tmpBgLo2 = nes->PPURead(tmpBg2*0x10+bgPatternAddr+(((v)&0x7000)/0x1000));
					int tmpBgHi2 = nes->PPURead(tmpBg2*0x10+bgPatternAddr+ 8 +(((v)&0x7000)/0x1000));
					incrementX();
					bgShiftReg16[0] = tmpBgLo1*0x100 | tmpBgLo2;
					bgShiftReg16[1] = tmpBgHi1*0x100 | tmpBgHi2;

					bgLatch[0] = (attrByte&0x2)>0;
					bgLatch[1] = attrByte&0x1;

					bgShiftReg8[0] = bgLatch[0]*0xFF;
					bgShiftReg8[1] = bgLatch[1]*0xFF;

					//Get next scanline Sprite data
					numSprites = 0;
					int spriteSize = (nes->peekMemory(0x2000)&0x20)>>5;
					int oamAddr = nes->peekMemory(0x2003);
					//sMask = 0x80;
					for (int i=oamAddr;i<64;i++){
						if (i==0)
							s0Hit = true;
						int palletTable = ((spriteSize)?OAM[i*4+1]&1 : (nes->peekMemory(0x2000)&8)>>3)*0x1000;
						int tileOffset = (OAM[i*4])%8;
						int tileAddr = ((!spriteSize)?OAM[i*4+1]:OAM[i*4+1]&0xFE)*0x10;
						//lower part of 8x16 sprite
						if ((OAM[i*4]) >= 8 && spriteSize)
							tileAddr += 0x10;
						//flip sprite vertically
						if (OAM[i*4+2]&0x80){
							if (spriteSize){
								if ((OAM[i*4]) >= 8)
									tileAddr -= 0x10;
								else
									tileAddr += 0x10;
							}
							tileOffset = 7-tileOffset;
						}
						int tmpPlane1 = nes->PPURead(palletTable+tileAddr+(tileOffset));
						int tmpPlane2 = nes->PPURead(palletTable+tileAddr+8+(tileOffset));

						unsigned char lookup[16] = {
							0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
							0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

						//flip sprite horizontally
						if (OAM[i*4+2]&0x40){
							tmpPlane1 = (lookup[tmpPlane1&0xf]<<4) | lookup[tmpPlane1>>4];
							tmpPlane2 = (lookup[tmpPlane2&0xf]<<4) | lookup[tmpPlane2>>4];
						}

						if (OAM[i*4] <= 0 && (OAM[i*4]+7+(8*spriteSize))>=0){
							if (numSprites >= 8)
								break;
							sShiftReg8_1[numSprites] = tmpPlane1;
							sShiftReg8_2[numSprites] = tmpPlane2;

							sCount[numSprites] = OAM[i*4+3];
							sLatch[numSprites] = OAM[i*4+2];
							numSprites++;
						}
					}

					sActiveLatch = 0;
				}
			}
			//Set the OAMADDR to 0
			if (cycles >= 257 && cycles <= 320)
				nes->writeMemory(0x2003, 0);
		}
		else if (scanlines >= 0 && scanlines <= 239){
			//std::cout << nes->peekMemory(0x2001) << ' ' << scanlines << std::endl;
			DrawPixel();

			if ((nes->peekMemory(0x2001) & 0x18)){
				//Rendering cycles
				//(cycles >= 321 && cycles <= 336) ||
				if ((cycles >= 321 && cycles <= 336) || (cycles >= 1 && cycles <= 256)) {
					//Every 8 cycles
					switch((cycles-1)%8){
						//Load Background registers
						case 0:
							bgTile = nes->PPURead(0x2000 | (v&0x0FFF));
							break;
						case 2:
							attrByte = nes->PPURead(0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
							if (v&0x40)attrByte >>= 4;
							if (v&0x2)attrByte >>= 2;
							break;
						case 4:
							bgPatternLo = nes->PPURead(bgTile*0x10+bgPatternAddr+((v&0x7000)/0x1000));
							break;
						case 6:
							bgPatternHi = nes->PPURead(bgTile*0x10+bgPatternAddr+ 8 +((v&0x7000)/0x1000));
							break;
						case 7:
							if (cycles == 328){
								bgShiftReg16[0] = (bgPatternLo*0x100);
								bgShiftReg16[1] = (bgPatternHi*0x100);
							}
							if (cycles == 336){
								bgShiftReg8[0] = bgLatch[0]*0xFF;
								bgShiftReg8[1] = bgLatch[1]*0xFF;
							}

							bgShiftReg16[0] = (bgShiftReg16[0]&0xFF00) | bgPatternLo;
							bgShiftReg16[1] = (bgShiftReg16[1]&0xFF00) | bgPatternHi;
							bgLatch[0] = (attrByte&0x2)>0;
							bgLatch[1] = (attrByte&0x1);

							if (scanlines != 239)
								incrementX();
							break;
					}
					//Increment vertical position in v
					if (cycles == 256){
						incrementY();
					}
				}
				else if (cycles == 257){
					//Copy horizontal bits from t to v
					nes->writeMemory(0x2003, 0);
					transferHorizontal();

				//Get next scanline Sprite data
					numSprites = 0;
					int spriteSize = (nes->peekMemory(0x2000)&0x20)>>5;
					int oamAddr = nes->peekMemory(0x2003);
					//sMask = 0x80;
					for (int i=oamAddr;i<64;i++){
						if (i==0)
							s0Hit = true;
						int palletTable = ((spriteSize)?OAM[i*4+1]&1 : (nes->peekMemory(0x2000)&8)>>3)*0x1000;
						int tileOffset = ((scanlines+1)-OAM[i*4])%8;
						int tileAddr = ((!spriteSize)?OAM[i*4+1]:OAM[i*4+1]&0xFE)*0x10;
						//lower part of 8x16 sprite
						if (((scanlines+1)-OAM[i*4]) >= 8 && spriteSize)
							tileAddr += 0x10;
						//flip sprite vertically
						if (OAM[i*4+2]&0x80){
							if (spriteSize){
								if (((scanlines+1)-OAM[i*4]) >= 8)
									tileAddr -= 0x10;
								else
									tileAddr += 0x10;
							}
							tileOffset = 7-tileOffset;
						}
						if(tileOffset < 0)tileOffset = 0;
						int tmpPlane1 = nes->PPURead(palletTable+tileAddr+(tileOffset));
						int tmpPlane2 = nes->PPURead(palletTable+tileAddr+8+(tileOffset));

						unsigned char lookup[16] = {
							0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
							0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

						//flip sprite horizontally
						if (OAM[i*4+2]&0x40){
							tmpPlane1 = (lookup[tmpPlane1&0xf]<<4) | lookup[tmpPlane1>>4];
							tmpPlane2 = (lookup[tmpPlane2&0xf]<<4) | lookup[tmpPlane2>>4];
						}

						if (OAM[i*4] <= scanlines+1 && (OAM[i*4]+7+(8*spriteSize))>=scanlines+1){
							if (numSprites >= 8)
								break;
							sShiftReg8_1[numSprites] = tmpPlane1;
							sShiftReg8_2[numSprites] = tmpPlane2;

							sCount[numSprites] = OAM[i*4+3];
							sLatch[numSprites] = OAM[i*4+2];
							numSprites++;
						}
					}

					sActiveLatch = 0;
				}
			}
		}

		else if (scanlines == 241 && (nes->peekMemory(0x2002) & 0b10000000) == 0 && vBlank == false){
			vBlank = true;
			nes->writeMemory(0x2002, nes->peekMemory(0x2002) | 0b10000000);
			frames++;
			nmiOccured = true;
		}

		//Reset Vblank
		else if (scanlines == 260 && cycles == 340){
			s0Hit = false;
			nmiOccured = false;
			nes->writeMemory(0x2002,nes->peekMemory(0x2002) & 0b01111111);
			vBlank = false;
		}

		if (nmiOccured && nmiOutput)
			nes->CPU.nmiLine = true;
		else
			nes->CPU.nmiLine = false;

		//Increment scanline
		if (cycles >= 340){
			cycles = -1;
			if (scanlines < 260)
				scanlines++;
			else
				scanlines = -1;
		}


		tmpCycles--;
	}
}

//Draw a Pixel
void ppu::DrawPixel(){
	if (cycles >= 1 && cycles <= 256){
		//Get background data
		int bgPlane1 = 0;
		int bgPlane2 = 0;
		int bgAttr1 = 0;
		int bgAttr2 = 0;

		//If background is enabled and left hand side is not hidden
		if ((nes->peekMemory(0x2001)&0x8) && (((nes->peekMemory(0x2001)&2)) || cycles > 8)){
			bgPlane1 = (bgShiftReg16[0]>>(15-x))&1;
			bgPlane2 = (bgShiftReg16[1]>>(15-x))&1;
			bgAttr1 = (bgShiftReg8[0] >> (7-x))&1;
			bgAttr2 = (bgShiftReg8[1] >> (7-x))&1;
		}

		//Render Sprite
		int sNum = 0;
		int sPlane1 = 0;
		int sPlane2 = 0;
		int sPallet = 0;

		if (nes->peekMemory(0x2001)&0x10)
			for (int i=0;i<numSprites;i++){
				if(sCount[i]-- == 0)
					sActiveLatch |= (128>>i);

				if (sActiveLatch & (128>>i)){
					int sTmpPlane1 = (sShiftReg8_1[i] & 0x80)>0;
					int sTmpPlane2 = (sShiftReg8_2[i] & 0x80)>0 ;
					int sTmpPallet = sLatch[i] & 3;
					sShiftReg8_1[i]<<=1;
					sShiftReg8_2[i]<<=1;
					if (sTmpPlane1|sTmpPlane2 && ((nes->peekMemory(0x2001)&0x4) || cycles > 8)){
						if (!(sPlane1|sPlane2)){
							sNum = i;
							sPlane1 = sTmpPlane1;
							sPlane2 = sTmpPlane2;
							sPallet = sTmpPallet;
						}
						//sprite zero hit
						if (bgPlane1|bgPlane2 && s0Hit && i==0 && !(nes->peekMemory(0x2002)&0x40) && cycles < 256){
							nes->writeMemory(0x2002, nes->peekMemory(0x2002) | 0x40);
						}
					}

				}
			}
		int sPalletAddr = 0x3F10 + (sPallet<<2) + ((sPlane2<<1)+sPlane1);
		int bgPalletAddr = 0x3F00 + (((bgAttr1<<1)+bgAttr2)<<2) + ((bgPlane2<<1)+bgPlane1);
		int palletAddr = bgPalletAddr;

		if (!(bgPlane1|bgPlane2) && !(sPlane1|sPlane2))
			palletAddr = 0x3F00;
		//show sprite if background is 0 or sprite has background priority, and sprite is not 0
		else if (((bgPlane1|bgPlane2)==0 || (sLatch[sNum]&0x20)==0) && (sPlane1|sPlane2))
			palletAddr = sPalletAddr;


		//Add pixel to screen
		int palletNum = nes->PPURead(palletAddr)&0b00111111;
		float redTint = 1;
		float greenTint = 1;
		float blueTint = 1;
		bool grayscale = nes->peekMemory(0x2001)&0x01;
		if (grayscale)
			palletNum &= 0x30;
		uint8_t colorTintValue = nes->peekMemory(0x2001)&0xE0;
		float attenuationRate = 0.75f;
		switch(colorTintValue){
			case 0x20:
				greenTint = attenuationRate;
				blueTint = attenuationRate;
				break;
			case 0x40:
				redTint = attenuationRate;
				blueTint = attenuationRate;
				break;
			case 0x60:
				blueTint = attenuationRate;
				break;
			case 0x80:
				redTint = attenuationRate;
				greenTint = attenuationRate;
				break;
			case 0xA0:
				greenTint = attenuationRate;
				break;
			case 0xC0:
				redTint = attenuationRate;
				break;
			case 0xE0:
				redTint = attenuationRate;
				greenTint = attenuationRate;
				blueTint = attenuationRate;
				break;

		}
		pixelVal[scanlines][cycles-1][0] = (pallet[palletNum][0]) * redTint;
		pixelVal[scanlines][cycles-1][1] = (pallet[palletNum][1]) * greenTint;
		pixelVal[scanlines][cycles-1][2] = (pallet[palletNum][2]) * blueTint;

		bgShiftReg16[0] = (bgShiftReg16[0]<<1)&0xFFFF;
		bgShiftReg16[1] = (bgShiftReg16[1]<<1)&0xFFFF;
		bgShiftReg8[0] = (bgShiftReg8[0]<<1)+bgLatch[0];
		bgShiftReg8[1]= (bgShiftReg8[1]<<1)+bgLatch[1];
	}
}

void ppu::DrawChars(int num,uint8_t *buffer,bool grayscale){
	int address = num?0x1000:0x0000;
	int numTiles = 0;

	uint8_t* colors[4];
	if (grayscale){
			uint8_t grayColor[3] = {0,0,0};
			uint8_t grayColor1[3] = {85,85,85};
			uint8_t grayColor2[3] = {170,170,170};
			uint8_t grayColor3[3] = {255,255,255};
			colors[0] = grayColor;
			colors[1] = grayColor1;
			colors[2] = grayColor2;
			colors[3] = grayColor3;
	}
	else{
		colors[0] = (pallet[nes->PeekPPUMemory(0x3F00)&0b00111111]);
		colors[1] = (pallet[nes->PeekPPUMemory(0x3F01)&0b00111111]);
		colors[2] = (pallet[nes->PeekPPUMemory(0x3F02)&0b00111111]);
		colors[3] = (pallet[nes->PeekPPUMemory(0x3F03)&0b00111111]);
	}

	while (address < (num?0x2000:0x1000)){
		int tileYBase = (numTiles/16)*128*3*8;
		int tileXBase = (numTiles%16)*8*3;
		for (int y=0;y<8;y++){
			for (int x=0;x<8;x++){
				bool plane1 = nes->PeekPPUMemory(address+y)&(128>>x);
				bool plane2 = nes->PeekPPUMemory(address+8+y)&(128>>x);
				for (int i=0;i<3;i++){
					buffer[tileYBase+y*128*3+tileXBase+x*3+i] = colors[plane1*0b01+plane2*0b10][i];
				}
			}
		}

		numTiles++;
		address += 16;
	}
}

void ppu::DrawSprites(uint8_t *buffer){
	int address = 0x0000;
	bool spriteMode = nes->peekMemory(0x2000)&0x20;

	uint8_t* colors[4];
	colors[0] = (pallet[nes->PeekPPUMemory(0x3F10)&0b00111111]);

	for(int numTiles = 0;numTiles<64;numTiles++){
		int index = OAM[numTiles*4+1];
		if (spriteMode){
			index = (index & 0xFE);
			address = (OAM[numTiles*4+1]&1)?0x1000:0x0000;
		}
		else
			address = ((nes->peekMemory(0x2000)&8)?0x1000:0x0000);

		address += index*16;
		int basePalletAddress = OAM[numTiles*4+2]&3;
		for (int i=0;i<3;i++){
			colors[1+i] = (pallet[nes->PeekPPUMemory(0x3F11+(basePalletAddress*4)+i)&0b00111111]);
		}
		int tileYBase = (numTiles/8)*64*3*16;
		int tileXBase = (numTiles%8)*8*3;
		int baseAddress = address;
		for (int y=0;y<16;y++){
			for (int x=0;x<8;x++){
				bool flipX = OAM[numTiles*4+2]&0x40;
				bool flipY = OAM[numTiles*4+2]&0x80;
				int pixelAddress = baseAddress;
				if (spriteMode){
					if (y>=8) pixelAddress += 16;
					pixelAddress += y%8;
				}
				else{
					pixelAddress += y/2;
				}

				bool plane1 = nes->PeekPPUMemory(pixelAddress)&(flipX?(1<<x):(128>>x));
				bool plane2 = nes->PeekPPUMemory(pixelAddress+8)&(flipX?(1<<x):(128>>x));
				for (int i=0;i<3;i++){
					int yVal = (flipY?15-y:y);
					buffer[tileYBase+yVal*64*3+tileXBase+x*3+i] = colors[plane1*0b01+plane2*0b10][i];
				}
			}
		}
	}
}

void ppu::DrawNametable(int num,uint8_t *buffer){
	int address = 0x2000 + 0x400*num;
	int baseAddress = address;

	uint8_t* colors[4];
	int numTiles = 0;

	colors[0] = (pallet[nes->PeekPPUMemory(0x3F00)&0b00111111]);

	while(address < baseAddress + 0x3C0){
		int row = numTiles/32;
		int colm = numTiles%32;
		int tileYBase = row*256*3*8;
		int tileXBase = colm*8*3;
		int tileAddress = (bool)(nes->peekMemory(0x2000)&0x10) * 0x1000 + nes->PeekPPUMemory(address)*16;

		int attribAddress = 0x23C0 + 0x400*num +(row/4)*8+(colm/4);
		int attributeX =  (colm%4)/2;
		int attributeY = (row%4)/2;
		bool palletOffsetHi = nes->PeekPPUMemory(attribAddress)&((0b10<<(attributeY*4))<<(attributeX*2));
		bool palletOffsetLo = nes->PeekPPUMemory(attribAddress)&((0b01<<(attributeY*4))<<(attributeX*2));
		int palletOffset = palletOffsetHi*0b10 + palletOffsetLo;

		int palletAddress = 0x3F00 + palletOffset*4;

		colors[1] = (pallet[nes->PeekPPUMemory(palletAddress+1)&0b00111111]);
		colors[2] = (pallet[nes->PeekPPUMemory(palletAddress+2)&0b00111111]);
		colors[3] = (pallet[nes->PeekPPUMemory(palletAddress+3)&0b00111111]);

		for (int y=0;y<8;y++){
			for (int x=0;x<8;x++){
				bool plane1 = nes->PeekPPUMemory(tileAddress+y)&(128>>x);
				bool plane2 = nes->PeekPPUMemory(tileAddress+8+y)&(128>>x);
				for (int i=0;i<3;i++){
					buffer[tileYBase+y*256*3+tileXBase+x*3+i] = colors[plane1*0b01+plane2*0b10][i];
				}
			}
		}

		numTiles++;
		address++;
	}
}

void ppu::SaveState(std::ofstream &file){
	file.write(reinterpret_cast<char*>(&cycles),2);
	file.write(reinterpret_cast<char*>(&scanlines),sizeof(int));
	file.write(reinterpret_cast<char*>(&frames),sizeof(int));
	file.write(reinterpret_cast<char*>(&v),2);
	file.write(reinterpret_cast<char*>(&t),2);
	file.write(reinterpret_cast<char*>(&x),1);
	file.write(reinterpret_cast<char*>(&w),1);
	file.write(reinterpret_cast<char*>(&s0Hit),1);
	file.write(reinterpret_cast<char*>(&vBlank),1);
	file.write(reinterpret_cast<char*>(&vBuffer),sizeof(int));
	file.write(reinterpret_cast<char*>(&nmiOutput),1);
	file.write(reinterpret_cast<char*>(&nmiOccured),1);
	file.write(reinterpret_cast<char*>(&OAM[0]),0x100*sizeof(int));
	file.write(reinterpret_cast<char*>(&ntRAM[0]),0x800*sizeof(int));
	file.write(reinterpret_cast<char*>(&palletRAM[0]),0x20*sizeof(int));
	file.write(reinterpret_cast<char*>(&bgShiftReg16[0]),2);
	file.write(reinterpret_cast<char*>(&bgShiftReg16[1]),2);
	file.write(reinterpret_cast<char*>(&bgShiftReg8[0]),1);
	file.write(reinterpret_cast<char*>(&bgShiftReg8[1]),1);
	file.write(reinterpret_cast<char*>(&bgLatch[0]),1);
	file.write(reinterpret_cast<char*>(&bgLatch[1]),1);
	file.write(reinterpret_cast<char*>(&bgTile),sizeof(int));
	file.write(reinterpret_cast<char*>(&attrByte),sizeof(int));
	file.write(reinterpret_cast<char*>(&bgPatternLo),sizeof(int));
	file.write(reinterpret_cast<char*>(&bgPatternHi),sizeof(int));

	file.write(reinterpret_cast<char*>(&sShiftReg8_1[0]),8*sizeof(int));
	file.write(reinterpret_cast<char*>(&sShiftReg8_2[0]),8*sizeof(int));
	file.write(reinterpret_cast<char*>(&sLatch[0]),8*sizeof(int));
	file.write(reinterpret_cast<char*>(&sCount[0]),8*sizeof(int));

	file.write(reinterpret_cast<char*>(&numSprites),sizeof(int));
	file.write(reinterpret_cast<char*>(&sActiveLatch),sizeof(int));
}

void ppu::LoadState(std::ifstream &file){
	file.read(reinterpret_cast<char*>(&cycles),2);
	file.read(reinterpret_cast<char*>(&scanlines),sizeof(int));
	file.read(reinterpret_cast<char*>(&frames),sizeof(int));
	file.read(reinterpret_cast<char*>(&v),2);
	file.read(reinterpret_cast<char*>(&t),2);
	file.read(reinterpret_cast<char*>(&x),1);
	file.read(reinterpret_cast<char*>(&w),1);
	file.read(reinterpret_cast<char*>(&s0Hit),1);
	file.read(reinterpret_cast<char*>(&vBlank),1);
	file.read(reinterpret_cast<char*>(&vBuffer),sizeof(int));
	file.read(reinterpret_cast<char*>(&nmiOutput),1);
	file.read(reinterpret_cast<char*>(&nmiOccured),1);
	file.read(reinterpret_cast<char*>(&OAM[0]),0x100*sizeof(int));
	file.read(reinterpret_cast<char*>(&ntRAM[0]),0x800*sizeof(int));
	file.read(reinterpret_cast<char*>(&palletRAM[0]),0x20*sizeof(int));
	file.read(reinterpret_cast<char*>(&bgShiftReg16[0]),2);
	file.read(reinterpret_cast<char*>(&bgShiftReg16[1]),2);
	file.read(reinterpret_cast<char*>(&bgShiftReg8[0]),1);
	file.read(reinterpret_cast<char*>(&bgShiftReg8[1]),1);
	file.read(reinterpret_cast<char*>(&bgLatch[0]),1);
	file.read(reinterpret_cast<char*>(&bgLatch[1]),1);
	file.read(reinterpret_cast<char*>(&bgTile),sizeof(int));
	file.read(reinterpret_cast<char*>(&attrByte),sizeof(int));
	file.read(reinterpret_cast<char*>(&bgPatternLo),sizeof(int));
	file.read(reinterpret_cast<char*>(&bgPatternHi),sizeof(int));

	file.read(reinterpret_cast<char*>(&sShiftReg8_1[0]),8*sizeof(int));
	file.read(reinterpret_cast<char*>(&sShiftReg8_2[0]),8*sizeof(int));
	file.read(reinterpret_cast<char*>(&sLatch[0]),8*sizeof(int));
	file.read(reinterpret_cast<char*>(&sCount[0]),8*sizeof(int));

	file.read(reinterpret_cast<char*>(&numSprites),sizeof(int));
	file.read(reinterpret_cast<char*>(&sActiveLatch),sizeof(int));
}
