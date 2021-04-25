#include <iostream>
#include <thread>
#include "ppu.h"
#include "NES.h"

using namespace std;

ppu::ppu(){
	//Initilize PPU
	cycles = 30;
	scanlines = 0;
	currScanline = 0;
	tmpScanline = 0;
	frameComplete = false;
	frames = 1;
	v = 0;
	t = 0;
	x = 0;
	w = 0;
	vBuffer = 0;
	vBlank = false;
	sActiveLatch = 0;
	s0Hit = false;

	//Set up Pallet
	pallet[0x00] = new GLubyte[3] {84,84,84};
	pallet[0x01] = new GLubyte[3] {0,30,116};
	pallet[0x02] = new GLubyte[3] {8,16,144};
	pallet[0x03] = new GLubyte[3] {48,0,136};
	pallet[0x04] = new GLubyte[3] {68,0,100};
	pallet[0x05] = new GLubyte[3] {92,0,48};
	pallet[0x06] = new GLubyte[3] {84,4,0};
	pallet[0x07] = new GLubyte[3] {60,24,0};
	pallet[0x08] = new GLubyte[3] {32,42,0};
	pallet[0x09] = new GLubyte[3] {8,58,0};
	pallet[0x0a] = new GLubyte[3] {0,64,0};
	pallet[0x0b] = new GLubyte[3] {0,60,0};
	pallet[0x0c] = new GLubyte[3] {0,50,60};
	pallet[0x0d] = new GLubyte[3] {0,0,0};
	pallet[0x0e] = new GLubyte[3] {0,0,0};
	pallet[0x0f] = new GLubyte[3] {0,0,0};
	pallet[0x10] = new GLubyte[3] {152,150,152};
	pallet[0x11] = new GLubyte[3] {8,76,196};
	pallet[0x12] = new GLubyte[3] {40,50,236};
	pallet[0x13] = new GLubyte[3] {92,30,228};
	pallet[0x14] = new GLubyte[3] {136,20,176};
	pallet[0x15] = new GLubyte[3] {160,20,100};
	pallet[0x16] = new GLubyte[3] {152,34,32};
	pallet[0x17] = new GLubyte[3] {120,60,0};
	pallet[0x18] = new GLubyte[3] {84,90,0};
	pallet[0x19] = new GLubyte[3] {40,114,0};
	pallet[0x1a] = new GLubyte[3] {8,124,0};
	pallet[0x1b] = new GLubyte[3] {0,118,40};
	pallet[0x1c] = new GLubyte[3] {0,102,120};
	pallet[0x1d] = new GLubyte[3] {0,0,0};
	pallet[0x1e] = new GLubyte[3] {0,0,0};
	pallet[0x1f] = new GLubyte[3] {0,0,0};
	pallet[0x20] = new GLubyte[3] {236,238,236};
	pallet[0x21] = new GLubyte[3] {76,154,236};
	pallet[0x22] = new GLubyte[3] {120,124,236};
	pallet[0x23] = new GLubyte[3] {176,98,236};
	pallet[0x24] = new GLubyte[3] {228,84,236};
	pallet[0x25] = new GLubyte[3] {236,88,180};
	pallet[0x26] = new GLubyte[3] {236,106,100};
	pallet[0x27] = new GLubyte[3] {212,136,32};
	pallet[0x28] = new GLubyte[3] {160,170,0};
	pallet[0x29] = new GLubyte[3] {116,196,0};
	pallet[0x2a] = new GLubyte[3] {76,208,32};
	pallet[0x2b] = new GLubyte[3] {56,204,108};
	pallet[0x2c] = new GLubyte[3] {56,180,204};
	pallet[0x2d] = new GLubyte[3] {60,60,60};
	pallet[0x2e] = new GLubyte[3] {0,0,0};
	pallet[0x2f] = new GLubyte[3] {0,0,0};
	pallet[0x30] = new GLubyte[3] {236,238,236};
	pallet[0x31] = new GLubyte[3] {168,204,236};
	pallet[0x32] = new GLubyte[3] {188,188,236};
	pallet[0x33] = new GLubyte[3] {212,178,236};
	pallet[0x34] = new GLubyte[3] {236,174,236};
	pallet[0x35] = new GLubyte[3] {236,174,212};
	pallet[0x36] = new GLubyte[3] {236,180,176};
	pallet[0x37] = new GLubyte[3] {228,196,144};
	pallet[0x38] = new GLubyte[3] {204,210,120};
	pallet[0x39] = new GLubyte[3] {180,222,120};
	pallet[0x3a] = new GLubyte[3] {168,226,144};
	pallet[0x3b] = new GLubyte[3] {152,226,180};
	pallet[0x3c] = new GLubyte[3] {160,214,228};
	pallet[0x3d] = new GLubyte[3] {160,162,160};
	pallet[0x3e] = new GLubyte[3] {0,0,0};
	pallet[0x3f] = new GLubyte[3] {0,0,0};
}

//Reset the PPU
void ppu::reset(){
	//Memory
	for (int i=0;i<0x4000;i++){
		memory[i] = 0;
		if (i<0x100)
			OAM[i] = 0;
		if (i<0x800)
			ntRAM[0] = 0;
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
	currScanline = 0;
	tmpScanline = 0;
	frames = 1;

	//Registers
	v = 0;
	t = 0;
	x = 0;
	w = 0;
	vBuffer = 0;

	//Flags
	frameComplete = false;
	s0Hit = false;
	vBlank = false;

}

void ppu::runCycles(int c){
}

/*void ppu::clock(int c){
	int tmpCycles = c;
	locked = true;
	while(tmpCycles > 0){
		cycles++;
		int bgPatternAddr = (*ppuReg2000&0x10)*0x100;

		//Prerender scanline
		if (scanlines == -1){
			//Set up Background rendering
			if (tmpScanline != scanlines){
				tmpScanline = scanlines;
				frameComplete = false;
				//Reset Sprite 0
				nes->writeMemory(0x2002, *ppuReg2002 & 0b10111111);

				//Set up Background Registers

				bgShiftReg16[0] = nes->PPURead(chrAddr);
				bgShiftReg16[1] = nes->PPURead(chrAddr+8);

				bgShiftReg16[0] += (nes->PPURead(chrAddr1)*0x100);
				bgShiftReg16[1] += (nes->PPURead(chrAddr1+8)*0x100);

				bgShiftReg8[0] = (((0b10<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>0)*0xFF;
			bgShiftReg8[1] = (((0b01<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>0)*0xFF;

				bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
				bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
			}
			//Reload the vertical scroll bits
			if ((nes->readMemory(0x2001) & 0x18)){
				//if (cycles >= 280 && cycles <= 304){
				if (cycles == 280){
					//Set v equal to t
					v &= (0xFFFF-0x7BE0);
					v |= (t & 0x7BE0);
					ntAddr = 0x2000 | (v&0x0FFF);
					attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
					attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
					attrAddr1 = 0x23C0 | ((v-1)&0x0C00) | (((v-1) >> 4) & 0x38) | (((v-1) >> 2) & 0x07);
					attrQuad1 = ((((((v-1)&0x3e0)>>5)%4)/2)<<1) + (((((v-1)&0x1f))%4)/2);
					chrAddr = (nes->PPURead(ntAddr)*0x10+bgPatternAddr)+((v&0x7000)/0x1000);
					chrAddr1 = nes->PPURead(ntAddr-1)*0x10+bgPatternAddr+((v&0x7000)/0x1000);
				}
			}
			//Set the OAMADDR to 0
			if (cycles >= 257 && cycles <= 320)
				nes->writeMemory(0x2003, 0);
		}
		//Rendering scanlines
		else if (scanlines >= 0 && scanlines <= 239){
			//Rendering is enabled
			if ((*ppuReg2001 & 0x18)){
				//Rendering cycles
				if (cycles >= 1 && cycles <= 256) {
					//Draw
					drawPixel();

					//Every 8 cycles
					if (cycles%8 == 0){
						//Load Background registers

						bgShiftReg16[0] &= 0xFF00;
						bgShiftReg16[0] += (nes->PPURead(nes->PPURead(ntAddr)*0x10+bgPatternAddr+((v&0x7000)/0x1000)));
						bgShiftReg16[1] &= 0xFF00;
						bgShiftReg16[1] += (nes->PPURead(nes->PPURead(ntAddr)*0x10+bgPatternAddr+8+((v&0x7000)/0x1000)));

						bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
						bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
					}
					//Increment vertical position in v
					if (cycles == 256){
						int fineY = (v & 0x7000);
						int coarseY = (v & 0x03E0)>>5;
						if (fineY != 0x7000)
							v += 0x1000;
						else{
							v &= (0xFFFF-0x7000);
							if (coarseY == 29){
								coarseY = 0;
								v ^= 0x0800;
							}
							else if (coarseY == 31)
								coarseY = 0;
							else
								coarseY++;
							v = (v & (0xFFFF-0x03e0)) | (coarseY<<5);
						}
						ntAddr = 0x2000 | (v&0x0FFF);
						attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
						attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
					}
				}
				//Get Sprite data for next scanline and update scroll
			//	else if (cycles >= 257 && cycles <= 320){
				else if (cycles == 257){
					//Copy vertical bits from t to v
					nes->writeMemory(0x2003, 0);

					v &= (0xFFFF-0x41f);
					v |= (t & 0x41f);
					ntAddr = 0x2000 | (v&0x0FFF);
					attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
					attrAddr1 = 0x23C0 | ((v-1)&0x0C00) | (((v-1) >> 4) & 0x38) | (((v-1) >> 2) & 0x07);
				attrQuad1 = ((((((v-1)&0x3e0)>>5)%4)/2)<<1) + (((((v-1)&0x1f))%4)/2);
				chrAddr = (nes->PPURead(ntAddr)*0x10+bgPatternAddr)+((v&0x7000)/0x1000);
				chrAddr1 = nes->PPURead(ntAddr-1)*0x10+bgPatternAddr+((v&0x7000)/0x1000);

					//Get next scanline Sprite data
					int tmpSL = scanlines;
					numSprites = 0;
					int tmpPlane1 = 0;
					int tmpPlane2 = 0;
					int spriteSize = (*ppuReg2000&0x20)>>5;
					int palletTable = 0;
					int tileOffset = 0;
					int tileAddr = 0;
					sMask = 0x80;
					for (int i=0;i<64;i++){
						if (numSprites >= 8)
							break;
						if (OAM[i*4] <= tmpSL && (OAM[i*4]+7+(8*spriteSize))>=(tmpSL) && numSprites < 8){
							if (i==0)
								s0Hit = true;
							palletTable = ((spriteSize)?OAM[i*4+1]&1 : (*ppuReg2000&8)>>3)*0x1000;
							tileOffset = (tmpSL-OAM[i*4])%8;
							tileAddr = ((!spriteSize)?OAM[i*4+1]:OAM[i*4+1]&0xFE)*0x10;
							if ((tmpSL-OAM[i*4]) >= 8 && spriteSize)
								tileAddr += 0x10;
							if (OAM[i*4+2]&0x80){
								if (spriteSize){
									if ((tmpSL-OAM[i*4]) >= 8)
										tileAddr -= 0x10;
									else
										tileAddr += 0x10;
								}
								tmpPlane1 = nes->PPURead(palletTable+tileAddr +(7-tileOffset));
								tmpPlane2 = nes->PPURead(palletTable+tileAddr + 8 + (7-tileOffset));
							}
							else{
								tmpPlane1 = nes->PPURead(palletTable+tileAddr+(tileOffset));
								tmpPlane2 = nes->PPURead(palletTable+tileAddr+8+(tileOffset));
							}

							if (OAM[i*4+2]&0x40){
								tmpPlane1 = (lookup[tmpPlane1&0xf]<<4) | lookup[tmpPlane1>>4];
								tmpPlane2 = (lookup[tmpPlane2&0xf]<<4) | lookup[tmpPlane2>>4];
							}

							sShiftReg8_1[numSprites] = tmpPlane1;
							sShiftReg8_2[numSprites] = tmpPlane2;

							sCount[numSprites] = OAM[i*4+3];
							sLatch[numSprites] = OAM[i*4+2];
							numSprites++;
						}
					}

					sActiveLatch = 0;

				}
				//Get background tiles for next scanline
				else if (cycles == 336){
					bgShiftReg16[0] = nes->PPURead(chrAddr);
					bgShiftReg16[1] = nes->PPURead(chrAddr+8);

					bgShiftReg16[0] += (nes->PPURead(chrAddr1)*0x100);
					bgShiftReg16[1] += (nes->PPURead(chrAddr1+8)*0x100);

					bgShiftReg8[0] = (((0b10<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>0)*0xFF;
				bgShiftReg8[1] = (((0b01<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>0)*0xFF;

					bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
					bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>0);
				}
				//Increment horizontal bits of v
				if ((cycles >= 328 || cycles <= 256) && (cycles%8) == 0){
					if ((v & 0x001f) == 31){
						v &= (0xFFFF-0x001f);
						v ^= 0x0400;
					}
					else
						v++;
					ntAddr = 0x2000 | (v&0x0FFF);
					attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
					attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
					attrAddr1 = 0x23C0 | ((v-1)&0x0C00) | (((v-1) >> 4) & 0x38) | (((v-1) >> 2) & 0x07);
					attrQuad1 = ((((((v-1)&0x3e0)>>5)%4)/2)<<1) + (((((v-1)&0x1f))%4)/2);
					chrAddr = (nes->PPURead(ntAddr)*0x10+bgPatternAddr)+((v&0x7000)/0x1000);
					chrAddr1 = nes->PPURead(ntAddr-1)*0x10+bgPatternAddr+((v&0x7000)/0x1000);
				}
			}
		}
		//PostRender scanlines
		//Set Vertical blank and increment frame count
		else if (scanlines == 241 && (*ppuReg2002 & 0b10000000) == 0 && vBlank == false){
			vBlank = true;
			nes->writeMemory(0x2002, *ppuReg2002 | 0b10000000);
			frames++;
			frameComplete = true;

			//Send NMI
			if (frames >= 2 && (*ppuReg2000 & 0b10000000)){
				nes->CPU.NMI();
			}
		}

		//Reset Vblank
		else if (scanlines == 260 && tmpScanline != scanlines){
			tmpScanline = scanlines;
			s0Hit = false;
			nes->writeMemory(0x2002,*ppuReg2002 & 0b01111111);
			vBlank = false;
		}

		//Increment scanline
		if (cycles > 340){
			cycles = 0;
			if (scanlines < 260)
				scanlines++;
			else
				scanlines = -1;
		}

		tmpCycles--;
	}

	locked = false;
}*/

//Cycle the PPU
void ppu::clock(int c){
	int tmpCycles = c;
	auto transferVertical = [&](){
		//Set v equal to t
		v &= (0xFFFF-0x7BE0);
		v |= (t & 0x7BE0);
	};

	auto transferHorizontal = [&](){
		v &= (0xFFFF-0x41f);
		v |= (t & 0x41f);
	};

	auto incrementX = [&](){
		if ((v & 0x001f) == 31){
			v &= (0xFFFF-0x001f);
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
			v &= (0xFFFF-0x7000);
			if (coarseY == 29){
				coarseY = 0;
				v ^= 0x0800;
			}
			else if (coarseY == 31)
				coarseY = 0;
			else
				coarseY++;
			v = (v & (0xFFFF-0x03e0)) | (coarseY<<5);
		}
	};

	while(tmpCycles > 0){
		cycles++;
		int bgPatternAddr = (*ppuReg2000&0x10)*0x100;
		if (scanlines == -1){
			if (tmpScanline != scanlines){
				tmpScanline = scanlines;
				frameComplete = false;
				//Reset Sprite 0
				nes->writeMemory(0x2002, *ppuReg2002 & 0b10111111);

				//Set up Background Registers
			}
			//Reload the vertical scroll bits
			if ((nes->readMemory(0x2001) & 0x18)){
				//if (cycles >= 280 && cycles <= 304){
				if (cycles == 280){
					//Set v equal to t
					transferVertical();
				}
			}
			//Set the OAMADDR to 0
			if (cycles >= 257 && cycles <= 320)
				nes->writeMemory(0x2003, 0);
		}
		else if (scanlines >= 0 && scanlines <= 239){
			if ((*ppuReg2001 & 0x18)){
				//Rendering cycles
				if ((cycles >= 328 && cycles < 337)*1 || (cycles >= 1 && cycles <= 256)) {
					drawPixel();
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
							bgShiftReg16[0] = (bgShiftReg16[0]&0xFF00) | bgPatternLo;
							bgShiftReg16[1] = (bgShiftReg16[1]&0xFF00) | bgPatternHi;

							bgLatch[0] = (attrByte&0x2)>0;
							bgLatch[1] = (attrByte&0x1);
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
					int tmpSL = scanlines;
					numSprites = 0;
					int tmpPlane1 = 0;
					int tmpPlane2 = 0;
					int spriteSize = (*ppuReg2000&0x20)>>5;
					int palletTable = 0;
					int tileOffset = 0;
					int tileAddr = 0;
					sMask = 0x80;
					for (int i=0;i<64;i++){
						if (numSprites >= 8)
							break;
						if (OAM[i*4] <= tmpSL && (OAM[i*4]+7+(8*spriteSize))>=(tmpSL) && numSprites < 8){
							if (i==0)
								s0Hit = true;
							palletTable = ((spriteSize)?OAM[i*4+1]&1 : (*ppuReg2000&8)>>3)*0x1000;
							tileOffset = (tmpSL-OAM[i*4])%8;
							tileAddr = ((!spriteSize)?OAM[i*4+1]:OAM[i*4+1]&0xFE)*0x10;
							if ((tmpSL-OAM[i*4]) >= 8 && spriteSize)
								tileAddr += 0x10;
							if (OAM[i*4+2]&0x80){
								if (spriteSize){
									if ((tmpSL-OAM[i*4]) >= 8)
										tileAddr -= 0x10;
									else
										tileAddr += 0x10;
								}
								tmpPlane1 = nes->PPURead(palletTable+tileAddr +(7-tileOffset));
								tmpPlane2 = nes->PPURead(palletTable+tileAddr + 8 + (7-tileOffset));
							}
							else{
								tmpPlane1 = nes->PPURead(palletTable+tileAddr+(tileOffset));
								tmpPlane2 = nes->PPURead(palletTable+tileAddr+8+(tileOffset));
							}

							if (OAM[i*4+2]&0x40){
								tmpPlane1 = (lookup[tmpPlane1&0xf]<<4) | lookup[tmpPlane1>>4];
								tmpPlane2 = (lookup[tmpPlane2&0xf]<<4) | lookup[tmpPlane2>>4];
							}

							sShiftReg8_1[numSprites] = tmpPlane1;
							sShiftReg8_2[numSprites] = tmpPlane2;

							sCount[numSprites] = OAM[i*4+3];
							sLatch[numSprites] = OAM[i*4+2];
							numSprites++;
						}
					}

					sActiveLatch = 0;
				}
				else if (cycles == 338){
					int tmpBg = nes->PPURead(0x2000 | ((v-2)&0x0FFF));
					int tmpBgLo = nes->PPURead(tmpBg*0x10+bgPatternAddr+(((v-2)&0x7000)/0x1000));
					int tmpBgHi = nes->PPURead(tmpBg*0x10+bgPatternAddr+ 8 +(((v-2)&0x7000)/0x1000));
					bgShiftReg16[0] = tmpBgLo*0x100 | bgPatternLo;
					bgShiftReg16[1] = tmpBgHi*0x100 | bgPatternHi;


					bgLatch[0] = (attrByte&0x2)>0;
					bgLatch[1] = attrByte&0x1;

					bgShiftReg8[0] = bgLatch[0]*0xFF;
					bgShiftReg8[1] = bgLatch[1]*0xFF;
				}
			}
		}

		else if (scanlines == 241 && (*ppuReg2002 & 0b10000000) == 0 && vBlank == false){
			vBlank = true;
			nes->writeMemory(0x2002, *ppuReg2002 | 0b10000000);
			frames++;
			frameComplete = true;

			//Send NMI
			if (frames >= 2 && (*ppuReg2000 & 0b10000000)){
				nes->CPU.NMI();
			}
		}

		//Reset Vblank
		else if (scanlines == 260 && tmpScanline != scanlines){
			tmpScanline = scanlines;
			s0Hit = false;
			nes->writeMemory(0x2002,*ppuReg2002 & 0b01111111);
			vBlank = false;
		}

		//Increment scanline
		if (cycles > 340){
			cycles = 0;
			if (scanlines < 260)
				scanlines++;
			else
				scanlines = -1;
		}

		tmpCycles--;
	}

	locked = false;
}

//Draw a Pixel
void ppu::drawPixel(){
	if (cycles >= 1 && cycles <= 256 && scanlines > -1 && scanlines <= 239){
		//Get background data
		int bgPlane1 = 0;
		int bgPlane2 = 0;
		int bgAttr1 = 0;
		int bgAttr2 = 0;
		int palletAddr = 0;

		//If background is disabled or left hand side is hidden
		if ((*ppuReg2001&0x8) == 0 || (((*ppuReg2001&2) == 0) && cycles <= 8)){
				bgPlane1 = 0;
				bgPlane2 = 0;
		}
		else{
			bgPlane1 = (bgShiftReg16[0]>>(15-x))&1;
			bgPlane2 = (bgShiftReg16[1]>>(15-x))&1;
			bgAttr1 = (bgShiftReg8[0] >> (7-x))&1;
			bgAttr2 = (bgShiftReg8[1] >> (7-x))&1;
		}

		//Render Sprite
		int sTmpPlane1 = 0;
		int sTmpPlane2 = 0;
		int sTmpPallet = 0;
		int sNum = 0;
		bool out = false;
		int sPlane1 = 0;
		int sPlane2 = 0;
		int sPallet = 0;

		if (*ppuReg2001&0x10)
			for (int i=0;i<numSprites;i++){
				if(sCount[i]-- == 0)
					sActiveLatch |= (128>>i);

				if (sActiveLatch & (128>>i)){

					sTmpPlane1 = (sShiftReg8_1[i] & 0x80)>0;
					sTmpPlane2 = (sShiftReg8_2[i] & 0x80)>0 ;

					sTmpPallet = sLatch[i] & 3;

					sShiftReg8_1[i]<<=1;
					sShiftReg8_2[i]<<=1;
					if (sTmpPlane1|sTmpPlane2 && ((*ppuReg2001&0x4) || cycles > 8)){
						//2if (i == 0 && scanlines == 38)
							//cout << (bgPlane1|bgPlane2) << endl;
						if (!out){
							out = true;
							sNum = i;
							sPlane1 = sTmpPlane1;
							sPlane2 = sTmpPlane2;
							sPallet = sTmpPallet;
						}
						if (bgPlane1|bgPlane2 && s0Hit && i==0 && !(*ppuReg2002&0x40) && cycles < 256){
							nes->writeMemory(0x2002, *ppuReg2002 | 0x40);
							//cout << scanlines << endl;
						}
					}

				}
			}

		if (!(bgPlane1|bgPlane2) && !(sPlane1|sPlane2))
			palletAddr = 0x3F00;
		else if (!(bgPlane1|bgPlane2))
			palletAddr = 0x3F10 + (sPallet<<2) + ((sPlane2<<1)+sPlane1);
		else if (!(sPlane1|sPlane2))
			palletAddr = 0x3F00 + (((bgAttr1<<1)+bgAttr2)<<2) + ((bgPlane2<<1)+bgPlane1);
		else if ((sLatch[sNum]&0x20)==0)
			palletAddr = 0x3F10 + (sPallet<<2) + ((sPlane2<<1)+sPlane1);
		else
			palletAddr = 0x3F00 + (((bgAttr1<<1)+bgAttr2)<<2) + ((bgPlane2<<1)+bgPlane1);

		//Add pixel to screen
		int palletNum = nes->PPURead(palletAddr)&0b00111111;

		GLubyte colorVal1 = (pallet[palletNum][0]);
		GLubyte colorVal2 = (pallet[palletNum][1]);
		GLubyte colorVal3 = (pallet[palletNum][2]);

		pixelVal[scanlines][cycles-1][0] = colorVal1;
		pixelVal[scanlines][cycles-1][1] = colorVal2;
		pixelVal[scanlines][cycles-1][2] = colorVal3;
	}

	//Increment Background registers
	bgShiftReg16[0] = (bgShiftReg16[0]<<1)&0xFFFF;
	bgShiftReg16[1] = (bgShiftReg16[1]<<1)&0xFFFF;
	bgShiftReg8[0] = (bgShiftReg8[0]<<1)+bgLatch[0];
	bgShiftReg8[1]= (bgShiftReg8[1]<<1)+bgLatch[1];
}
