#include <iostream>
#include <GL/gl.h>
#include "ppu.h"
#include "NES.h"

using namespace std;

ppu::ppu(){
	memory = new int[0x4000];
	OAM = new int[0x100];
	ntRAM = new int[0x800];
	//addrLO = false;
	cycles = 30;
	scanlines = 0;
	frames = 1;
	v = 0;
	t = 0;
	x = 0;
	w = 0;
	vBuffer = 0;
	currScanline = 0;
	vBlank = false;
	tmpScanline = 0;
	bgShiftReg16 = new int[2];
	bgShiftReg8 = new int[2];
	bgLatch = new int[2];
	//secOAM = new int[32];
	sShiftReg8_1 = new int[8];
	sShiftReg8_2 = new int[8];
	sLatch = new int[8];
	sActiveLatch = 0;
	sCount = new int[8];
	s0Hit = false;
	frameComplete = false;
		
	pallet = new GLubyte*[64];
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

void ppu::reset(){
	for (int i=0;i<0x4000;i++){
		memory[i] = 0;
		if (i<0x100)
			OAM[i] = 0;
		if (i<0x800)
			ntRAM[0] = 0;
	}
	bgShiftReg16[0] = 0;
	bgShiftReg16[1] = 0;
	bgShiftReg8[0] = 0;
	bgShiftReg8[1] = 0;
	bgLatch[0] = 0;
	bgLatch[1] = 0;

	for (int i=0;i<8;i++){
		sShiftReg8_1[i] = 0;
		sShiftReg8_2[i] = 0;
		sLatch[i] = 0;
		sCount[i] = 0;
	}

	cycles = 30;
	scanlines = 0;
	frames = 1;
	v = 0;
	t = 0;
	x = 0;
	w = 0;
	
	frameComplete = false;
	s0Hit = false;
	vBuffer = 0;
	currScanline = 0;
	vBlank = false;
	tmpScanline = 0;
	
}

void ppu::cycle(int c){
	//int startTime = glutGet(GLUT_ELAPSED_TIME);
	int tmpCycles = c;
	unsigned char lookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
	while(tmpCycles > 0){
		cycles++;
		int bgPatternAddr = (*ppuReg2000&0x10)*0x100;
	
		if (scanlines == -1){
			if (tmpScanline != scanlines){
				tmpScanline = scanlines;
				nes->writeMemory(0x2002, *ppuReg2002 & 0b10111111);
				frameComplete = false;
		
				int ntAddr = 0x2000 | (v&0x0FFF);
				int attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
				int attrAddr1 = 0x23C0 | ((v-1)&0x0C00) | (((v-1) >> 4) & 0x38) | (((v-1) >> 2) & 0x07);
				int attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
				int attrQuad1 = ((((((v-1)&0x3e0)>>5)%4)/2)<<1) + (((((v-1)&0x1f))%4)/2);
				int chrAddr = (nes->PPURead(ntAddr)*0x10+bgPatternAddr)+((v&0x7000)/0x1000);
				int chrAddr1 = nes->PPURead(ntAddr-1)*0x10+bgPatternAddr+((v&0x7000)/0x1000);
				bgShiftReg16[0] = nes->PPURead(chrAddr);
				bgShiftReg16[1] = nes->PPURead(chrAddr+8);
			
				bgShiftReg16[0] += (nes->PPURead(chrAddr1)*0x100);
				bgShiftReg16[1] += (nes->PPURead(chrAddr1+8)*0x100);	
				
				bgShiftReg8[0] = 0;
				bgShiftReg8[1] = 0;
				int attrPallet = (((0b10<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>>(2*attrQuad1+1));
				int attrPallet1 = (((0b01<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>>(2*attrQuad1));
				for(int i=0;i<8;i++){	
					bgShiftReg8[0] |= attrPallet<<(7-i);
					bgShiftReg8[1] |= attrPallet1<<(7-i);
				}
				bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad+1));
				bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad));
			}
			if ((nes->readMemory(0x2001) & 0x18)){
				if (cycles >= 280 && cycles <= 304){
					v &= (0xFFFF-0x7BE0);
					v |= (t & 0x7BE0);
				}
			}
			if (cycles >= 257 && cycles <= 320)
				nes->writeMemory(0x2003, 0);
		}
		else if (scanlines >= 0 && scanlines <= 239){
			if ((*ppuReg2001 & 0x18)){
				if (cycles >= 1 && cycles <= 256) {
					drawPixel();
					if (cycles%8 == 0){
						int ntAddr = 0x2000 | (v&0x0FFF);
						int attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
						int attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
						bgShiftReg16[0] &= 0xFF00;
						bgShiftReg16[0] += (nes->PPURead(nes->PPURead(ntAddr)*0x10+bgPatternAddr+((v&0x7000)/0x1000)));
						bgShiftReg16[1] &= 0xFF00;
						bgShiftReg16[1] += (nes->PPURead(nes->PPURead(ntAddr)*0x10+bgPatternAddr+8+((v&0x7000)/0x1000)));
						bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad+1));
						bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad));
					}
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
					}
				}
				else if (cycles >= 257 && cycles <= 320){
					nes->writeMemory(0x2003, 0);
				  if (cycles == 257 ){
						v &= (0xFFFF-0x41f);
						v |= (t & 0x41f);
					}
					else if (cycles == 320){
						int tmpSL = scanlines;
						int n = 0;
						int tmpPlane1 = 0;
						int tmpPlane2 = 0;
						int spriteSize = (*ppuReg2000&0x20)>>5;
						int palletTable = 0;
						int tileOffset = 0;
						int tileAddr = 0;
						for (int i=0;i<64;i++){
							if (n >= 8)
								break;
							if (OAM[i*4] <= tmpSL && (OAM[i*4]+7+(8*spriteSize))>=(tmpSL) && n < 8){
								if (i==0)
									s0Hit = true;
								palletTable = ((spriteSize)?OAM[i*4+1]&1 : (*ppuReg2000&4)>>4)*0x1000;
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
								
								sShiftReg8_1[n] = tmpPlane1;
								sShiftReg8_2[n] = tmpPlane2;
								
								sCount[n] = OAM[i*4+3];
								sLatch[n] = OAM[i*4+2];
								n++;
							}
						}
						
						sActiveLatch = 0;
					}
				}
				else if (cycles == 336){
					int ntAddr = 0x2000 | (v&0x0FFF);
					int attrAddr = 0x23C0 | (v&0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
					int attrAddr1 = 0x23C0 | ((v-1)&0x0C00) | (((v-1) >> 4) & 0x38) | (((v-1) >> 2) & 0x07);
					int attrQuad = (((((v&0x3e0)>>5)%4)/2)<<1) + (((v&0x1f)%4)/2);
					int attrQuad1 = ((((((v-1)&0x3e0)>>5)%4)/2)<<1) + (((((v-1)&0x1f))%4)/2);
					int chrAddr = (nes->PPURead(ntAddr)*0x10+bgPatternAddr)+((v&0x7000)/0x1000);
					int chrAddr1 = nes->PPURead(ntAddr-1)*0x10+bgPatternAddr+((v&0x7000)/0x1000);
					bgShiftReg16[0] = nes->PPURead(chrAddr);
					bgShiftReg16[1] = nes->PPURead(chrAddr+8);
			
					bgShiftReg16[0] += (nes->PPURead(chrAddr1)*0x100);
					bgShiftReg16[1] += (nes->PPURead(chrAddr1+8)*0x100);	
					
					bgShiftReg8[0] = 0;
					bgShiftReg8[1] = 0;
					int attrPallet = (((0b10<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>>(2*attrQuad1+1));
					int attrPallet1 = (((0b01<<(2*attrQuad1)) & nes->PPURead(attrAddr1))>>(2*attrQuad1));
					for(int i=0;i<8;i++){	
						bgShiftReg8[0] |= attrPallet<<(7-i);
						bgShiftReg8[1] |= attrPallet1<<(7-i);
					}
					bgLatch[0] = (((0b10<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad+1));
					bgLatch[1] = (((0b01<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad));
				}
				if ((cycles >= 328 || cycles <= 256) && (cycles%8) == 0){
					if ((v & 0x001f) == 31){
						v &= (0xFFFF-0x001f);
						v ^= 0x0400;	
					}
					else
						v++; 
				}
			}
		}
		else if (scanlines == 241 && (*ppuReg2002 & 0b10000000) == 0 && vBlank == false){
			vBlank = true;
			nes->writeMemory(0x2002, *ppuReg2002 | 0b10000000);
			frames++;
			frameComplete = true;
			
			if (frames >= 2 && (*ppuReg2000 & 0b10000000)){
				//drawFrame();
			//drawBackground();
			//drawSprites();
				nes->CPU.NMI();
			}
		}
		else if (scanlines == 260 && tmpScanline != scanlines){
			tmpScanline = scanlines;
			s0Hit = false;
			nes->writeMemory(0x2002,*ppuReg2002 & 0b01111111);
			vBlank = false;
		}
	
		if (cycles > 340){
			cycles = 0;
			if (scanlines < 260)
				scanlines++;
			else
				scanlines = -1;
		}
	
		tmpCycles--;
	}								
}

void ppu::drawPixel(){
	int bgPlane1 = (bgShiftReg16[0]>>(15-x))&1;
	int bgPlane2 = (bgShiftReg16[1]>>(15-x))&1;
	int bgAttr1 = (bgShiftReg8[0] >> (7-x))&1;
	int bgAttr2 = (bgShiftReg8[1] >> (7-x))&1;
	int palletAddr = 0x3F00 + (((bgAttr1<<1)+bgAttr2)<<2) + ((bgPlane2<<1)+bgPlane1);
	
	if ((*ppuReg2001&0x8) == 0 || (((*ppuReg2001&2) == 0) && cycles <= 8)){
			bgPlane1 = 0;
			bgPlane2 = 0;
	}
	if (!(bgPlane1|bgPlane2))
			palletAddr = 0x3F00;


	int sPlane1 = 0;
	int sPlane2 = 0;
	int sPallet = 0;
	if (*ppuReg2001&0x10)
		for (int i=0;i<8;i++){
			if(sCount[i]-- == 0)
				sActiveLatch |= (128>>i);
			
			if (sActiveLatch & (128>>i)){
				sPlane1 = (sShiftReg8_1[i] & 0x80)>>7;
				sPlane2 = (sShiftReg8_2[i] & 0x80)>>7;
				sPallet = sLatch[i] & 3;
				if (sPlane1|sPlane2 && ((*ppuReg2001&0x4) || cycles > 8)){
					if (bgPlane1|bgPlane2 && s0Hit && i==0 && !(*ppuReg2002&0x40) && cycles < 256){
						nes->writeMemory(0x2002, *ppuReg2002 | 0x40);
					}
					if ((sLatch[i]&0x20)==0 || !(bgPlane1|bgPlane2))
						palletAddr = 0x3F10 + (sPallet<<2) + ((sPlane2<<1)+sPlane1);		
					}
				sShiftReg8_1[i]<<=1;
				sShiftReg8_2[i]<<=1;
				
			}
		}

	int index = ((scanlines*256+(cycles-1))*4*3)+((256*240)*4*3);
	int palletNum = nes->PPURead(palletAddr);
	float colorVal1 = (pallet[palletNum][0])/256.0;
	float colorVal2 = (pallet[palletNum][1])/256.0;
	float colorVal3 = (pallet[palletNum][2])/256.0;
	

	pixelVal[index] = colorVal1;
	pixelVal[index+1] = colorVal2;
	pixelVal[index+2] = colorVal3;

	pixelVal[index+3] = colorVal1;
	pixelVal[index+4] = colorVal2;
	pixelVal[index+5] = colorVal3;

	pixelVal[index+6] = colorVal1;
	pixelVal[index+7] = colorVal2;
	pixelVal[index+8] = colorVal3;
	
	pixelVal[index+9] = colorVal1;
	pixelVal[index+10] = colorVal2;
	pixelVal[index+11] = colorVal3;
	
	bgShiftReg16[0] = (bgShiftReg16[0]<<1)&0xFFFF;
	bgShiftReg16[1] = (bgShiftReg16[1]<<1)&0xFFFF;
	bgShiftReg8[0] = (bgShiftReg8[0]<<1)+bgLatch[0];
	bgShiftReg8[1]= (bgShiftReg8[1]<<1)+bgLatch[1];
}
void ppu::drawFrame(){
	//int startTime = glutGet(GLUT_ELAPSED_TIME);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	int startAddr = 0x2000 + (t&0xC1F);
	int baseNT = 0x2000 + (t&0xC00);
	int tileNum = 0;
	int *bgPlane1 = new int[8];
	int *bgPlane2 = new int[8];
	int *sPlane1 = new int[8];
	int *sPlane2 = new int[8];
	int *prevSPlane1 = new int[8];
	int *prevSPlane2 = new int[8];
	int *tmpPlane1 = new int[8];
	int *tmpPlane2 = new int[8];
	int *tmpChrPlane = new int[8];
	int *bgChrPlane = new int[8];
	int *sChrPlane = new int[8];
	int *prevSChrPlane = new int[8];
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	unsigned char lookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
	
	while (tileNum < 1024){
		int row = tileNum/32;
		int colm = tileNum%32;
		
		int ntAddr = startAddr + (row*0x20) + colm;
		if (ntAddr > (baseNT + (row*0x20)+0x1F)){
			if (baseNT == 0x2400)
				ntAddr = 0x2000 + (row*0x20) + (((startAddr-baseNT)+colm)%0x20);
			else
				ntAddr = baseNT + 0x400 + (row*0x20) + (((startAddr-baseNT)+colm)%0x20);
		}
	
		int relColm = (((startAddr-baseNT)+colm)%0x20);
		
		int bgChrAddr = memory[ntAddr]*0x10+0x1000;

		int oamAddr = -1;
		int prevOamAddr = -1;
		for (int i=0;i<64;i++){
			if (OAM[i*4]/8 == row){
				if ((OAM[i*4+3])/8 == colm)
					oamAddr = i*4;
				if ((OAM[i*4+3]+7)/8 == colm)
					prevOamAddr = i*4;
			}
		}
	
		int syPos = 0;
		int tile = 0;
		int attr = 0;
	  int sxPos = 0;
		int sxOffset = 0;
		int prevSyPos = 0;
		int prevTile = 0;
		int prevAttr = 0;
	  int prevSxPos = 0;
		int prevSxOffset = 0;
		
		if (oamAddr != -1){
		  syPos = OAM[oamAddr];
			tile = OAM[oamAddr+1];
			attr = OAM[oamAddr+2];
			sxPos = OAM[oamAddr+3];
			sxOffset = OAM[oamAddr+3] - (colm*8);
		}

		if (prevOamAddr != -1){
			prevSyPos = OAM[prevOamAddr];
			prevTile = OAM[prevOamAddr+1];
			prevAttr = OAM[prevOamAddr+2];
			prevSxPos = OAM[prevOamAddr+3];
			prevSxOffset =(colm*8) - OAM[prevOamAddr+3];
		}
	

		int sChrAddr = tile*0x10;
		int prevSChrAddr = prevTile*0x10;

		int tmpAddr = ntAddr;	
	
		int tmpRelColm = relColm;
		if (relColm == 0x1f){
			tmpAddr = ((baseNT==0x2400)?0x2000:(baseNT+0x400)) + (row*0x20);
			tmpRelColm = 0;
		}
		else {
			tmpAddr = ntAddr+1;
			tmpRelColm++;
		}
		
		int tmpChrAddr = memory[tmpAddr]*0x10+0x1000;
		
		
		int attrAddr = (((ntAddr)<0x2400)?0x2300:0x2700) + ((row/8+0xc)*0x10) + 	((relColm/4)+(((row%8)/4)*8));
		int tmpAttrAddr = (((tmpAddr)<0x2400)?0x2300:0x2700) + ((row/8+0xc)*0x10) + 	((tmpRelColm/4)+(((row%8)/4)*8));

		int attrQuad = (((row%4)/2)<<1) + ((relColm%4)/2);
		int tmpAttrQuad = (((row%4)/2)<<1) + ((tmpRelColm%4)/2);
		int bgPalletNum = ((0b11<<(2*attrQuad)) & memory[attrAddr])>>(2*attrQuad);
		int tmpPalletNum = ((0b11<<(2*tmpAttrQuad)) & memory[tmpAttrAddr])>>(2*tmpAttrQuad);
		int sPalletNum = (attr & 3);
		int prevSPalletNum = (prevAttr & 3);
		int palletAddr = 0x3f00;

		int pixelSize = 1;
		glColor3f(1,1,1);
		int xPos = colm*8*pixelSize;	
		int yPos = windowHeight-(row*8*pixelSize);

		int currPallet = palletAddr;
		glColor3ubv(pallet[memory[palletAddr]]);
		glRecti(xPos,yPos,xPos+8*pixelSize,yPos-8*pixelSize);
		bool drawPixel = true;

		for (int i=0;i<8;i++){
				
			bgPlane1[i] = memory[bgChrAddr+i];
			sPlane1[i] = memory[sChrAddr+i];
			tmpPlane1[i] = memory[tmpChrAddr+i];
			prevSPlane1[i] = memory[prevSChrAddr+i];
		
			bgPlane2[i] = memory[bgChrAddr+i+8];
			sPlane2[i] = memory[sChrAddr+i+8];
			tmpPlane2[i] = memory[tmpChrAddr+i+8];
			prevSPlane2[i] = memory[prevSChrAddr+i+8];
			
		
			if (attr & 0b01000000){
				sPlane1[i] = (lookup[sPlane1[i]&0xf]<<4) | lookup[sPlane1[i]>>4];	
				sPlane2[i] = (lookup[sPlane2[i]&0xf]<<4) | lookup[sPlane2[i]>>4];
			}
			if (prevAttr & 0b01000000){
				prevSPlane1[i] = (lookup[prevSPlane1[i]&0xf]<<4) | lookup[prevSPlane1[i]>>4];	
				prevSPlane2[i] = (lookup[prevSPlane2[i]&0xf]<<4) | lookup[prevSPlane2[i]>>4];
			}

			if (prevOamAddr == -1){
				prevSChrPlane[i] = 0;
				prevSPlane1[i] = 0;
				prevSPlane2[i] = 0;
			}
			else{
				prevSPlane1[i] = (prevSPlane1[i]<<(prevSxOffset));
				prevSPlane2[i] = (prevSPlane2[i]<<(prevSxOffset));
				prevSChrPlane[i] = (prevSPlane1[i] | prevSPlane2[i]);
			}
				
			bgPlane1[i] = (bgPlane1[i]<<x);
			bgPlane2[i] = (bgPlane2[i]<<x);
			bgChrPlane[i] = ((bgPlane1[i] | bgPlane2[i]));
			sPlane1[i] = (sPlane1[i]>>sxOffset);
			sPlane2[i] = (sPlane2[i]>>sxOffset);
			sChrPlane[i] = (sPlane1[i] | sPlane2[i]);
			sChrPlane[i] |= prevSChrPlane[i];
			sPlane1[i] |= prevSPlane1[i];
			sPlane2[i] |= prevSPlane2[i];
			tmpChrPlane[i] = tmpPlane1[i] | tmpPlane2[i];
			tmpChrPlane[i] &= (0xFF<<(8-x));
			tmpPlane1[i] &= (0xFF<<(8-x));
			tmpPlane2[i] &= (0xFF<<(8-x));
			bgPlane1[i] |= (tmpPlane1[i]>>(8-x));
      bgPlane2[i] |= (tmpPlane2[i]>>(8-x));
			bgChrPlane[i] |= (tmpChrPlane[i]>>(8-x));

			yPos -= pixelSize;
			xPos = colm*8*pixelSize;
			for (int j=0;j<8;j++){
				drawPixel = true;
		  	if ((oamAddr != -1) && (attr & 0b00100000)==0){
					if (sChrPlane[i] & 128>>j)
							palletAddr = 0x3F10 + (sPalletNum<<2) +  (((sPlane2[i] & 128>>j)/(128>>j))<<1) + (sPlane1[i] & 128>>j)/(128>>j);
					else
							palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2[i] & 128>>j)/(128>>j))<<1) + (bgPlane1[i] & 128>>j)/(128>>j);
					}
				else if ((prevOamAddr != -1) && (prevAttr & 0b00100000)==0){
				 	if (prevSChrPlane[i] & 128>>j)
						palletAddr = 0x3F10 + (prevSPalletNum<<2) +  (((prevSPlane2[i] & 128>>j)/(128>>j))<<1) + (prevSPlane1[i] & 128>>j)/(128>>j);
					else
						palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2[i] & 128>>j)/(128>>j))<<1) + (bgPlane1[i] & 128>>j)/(128>>j);
				}
				else{
					if (bgChrPlane[i] & 128>>j){
						palletAddr = 0x3F00 + (((j<(8-x))?bgPalletNum:tmpPalletNum)<<2) +  (((bgPlane2[i] & 128>>j)/(128>>j))<<1) + (bgPlane1[i] & 128>>j)/(128>>j);
					}
					else{
						drawPixel = false;
						palletAddr = 0x3F00;
					}
				}
				if (currPallet != palletAddr){
					glColor3ubv(pallet[memory[palletAddr]]);
					currPallet = palletAddr;
				}
				if (drawPixel)
			 	 glRecti(xPos,yPos,xPos+pixelSize,yPos+pixelSize);
				xPos += pixelSize;
			}
		}
		tileNum++;
	}
	delete[] bgPlane1,bgPlane2,sPlane1,sPlane2,bgChrPlane,sChrPlane,tmpPlane1,tmpPlane2,tmpChrPlane,prevSPlane1,prevSPlane2,prevSChrPlane;
//	glFlush();
	//cout << (glutGet(GLUT_ELAPSED_TIME)-startTime) << endl;
}

void ppu::drawScanline(){
//	int startTime = glutGet(GLUT_ELAPSED_TIME);
	int startAddr = 0x2000 + (t&0xC1F);
	int baseNT = 0x2000 + (t&0xC00);
	int tileNum = 0;
	int bgPlane1 = 0;
	int bgPlane2 = 0;
	int sPlane1 = 0;
	int sPlane2 = 0;
	int prevSPlane1 = 0;
	int prevSPlane2 = 0;
	int tmpPlane1 = 0;
	int tmpPlane2 = 0;
	int tmpChrPlane = 0;
	int bgChrPlane = 0;
	int sChrPlane = 0;
	int prevSChrPlane = 0;
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	unsigned char lookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
	while (tileNum < 32){
		int row = scanlines/8;
		int tileRow = scanlines%8;
		int colm = tileNum;

		int ntAddr = startAddr + (row*0x20) + colm;
		if (ntAddr > (baseNT + (row*0x20)+0x1F)){
			if (baseNT == 0x2400)
				ntAddr = 0x2000 + (row*0x20) + (((startAddr-baseNT)+colm)%0x20);
			else
				ntAddr = baseNT + 0x400 + (row*0x20) + (((startAddr-baseNT)+colm)%0x20);
		}
	
		int relColm = (((startAddr-baseNT)+colm)%0x20);
		
		int bgChrAddr = memory[ntAddr]*0x10+0x1000;

		int oamAddr = -1;
		int prevOamAddr = -1;
		for (int i=0;i<64;i++){
			if (OAM[i*4]/8 == row){
				if ((OAM[i*4+3])/8 == colm)
					oamAddr = i*4;
				if ((OAM[i*4+3]+7)/8 == colm)
					prevOamAddr = i*4;
			}
		}
	
		int syPos = 0;
		int tile = 0;
		int attr = 0;
	  int sxPos = 0;
		int sxOffset = 0;
		int prevSyPos = 0;
		int prevTile = 0;
		int prevAttr = 0;
	  int prevSxPos = 0;
		int prevSxOffset = 0;
		
		if (oamAddr != -1){
		  syPos = OAM[oamAddr];
			tile = OAM[oamAddr+1];
			attr = OAM[oamAddr+2];
			sxPos = OAM[oamAddr+3];
			sxOffset = OAM[oamAddr+3] - (colm*8);
		}

		if (prevOamAddr != -1){
			prevSyPos = OAM[prevOamAddr];
			prevTile = OAM[prevOamAddr+1];
			prevAttr = OAM[prevOamAddr+2];
			prevSxPos = OAM[prevOamAddr+3];
			prevSxOffset =(colm*8) - OAM[prevOamAddr+3];
		}
	

		int sChrAddr = tile*0x10;
		int prevSChrAddr = prevTile*0x10;

		int tmpAddr = ntAddr;	
	
		int tmpRelColm = relColm;
		if (relColm == 0x1f){
			tmpAddr = ((baseNT==0x2400)?0x2000:(baseNT+0x400)) + (row*0x20);
			tmpRelColm = 0;
		}
		else {
			tmpAddr = ntAddr+1;
			tmpRelColm++;
		}
		
		int tmpChrAddr = memory[tmpAddr]*0x10+0x1000;
		
		
		int attrAddr = (((ntAddr)<0x2400)?0x2300:0x2700) + ((row/8+0xc)*0x10) + 	((relColm/4)+(((row%8)/4)*8));
		int tmpAttrAddr = (((tmpAddr)<0x2400)?0x2300:0x2700) + ((row/8+0xc)*0x10) + 	((tmpRelColm/4)+(((row%8)/4)*8));

		int attrQuad = (((row%4)/2)<<1) + ((relColm%4)/2);
		int tmpAttrQuad = (((row%4)/2)<<1) + ((tmpRelColm%4)/2);
		int bgPalletNum = ((0b11<<(2*attrQuad)) & memory[attrAddr])>>(2*attrQuad);
		int tmpPalletNum = ((0b11<<(2*tmpAttrQuad)) & memory[tmpAttrAddr])>>(2*tmpAttrQuad);
		int sPalletNum = (attr & 3);
		int prevSPalletNum = (prevAttr & 3);
		int palletAddr = 0x3f00;

		int pixelSize = 1;
		glColor3f(1,1,1);
		int xPos = colm*8*pixelSize;	
		int yPos = windowHeight-(scanlines*pixelSize);

		int currPallet = palletAddr;
		glColor3ubv(pallet[memory[palletAddr]]);
		glRecti(xPos,yPos,xPos+8*pixelSize,yPos-pixelSize);
		bool drawPixel = true;

		bgPlane1 = memory[bgChrAddr+tileRow];
		sPlane1 = memory[sChrAddr+tileRow];
		tmpPlane1 = memory[tmpChrAddr+tileRow];
		prevSPlane1 = memory[prevSChrAddr+tileRow];
	
		bgPlane2 = memory[bgChrAddr+tileRow+8];
		sPlane2 = memory[sChrAddr+tileRow+8];
		tmpPlane2 = memory[tmpChrAddr+tileRow+8];
		prevSPlane2 = memory[prevSChrAddr+tileRow+8];
		
	
		if (attr & 0b01000000){
			sPlane1 = (lookup[sPlane1&0xf]<<4) | lookup[sPlane1>>4];	
			sPlane2 = (lookup[sPlane2&0xf]<<4) | lookup[sPlane2>>4];
		}
		if (prevAttr & 0b01000000){
			prevSPlane1 = (lookup[prevSPlane1&0xf]<<4) | lookup[prevSPlane1>>4];	
			prevSPlane2 = (lookup[prevSPlane2&0xf]<<4) | lookup[prevSPlane2>>4];
		}

		if (prevOamAddr == -1){
			prevSChrPlane = 0;
			prevSPlane1 = 0;
			prevSPlane2 = 0;
		}
		else{
			prevSPlane1 = (prevSPlane1<<(prevSxOffset));
			prevSPlane2 = (prevSPlane2<<(prevSxOffset));
			prevSChrPlane = (prevSPlane1 | prevSPlane2);
		}
			
		bgPlane1 = (bgPlane1<<x);
		bgPlane2 = (bgPlane2<<x);
		bgChrPlane = ((bgPlane1 | bgPlane2));
		sPlane1 = (sPlane1>>sxOffset);
		sPlane2 = (sPlane2>>sxOffset);
		sChrPlane = (sPlane1 | sPlane2);
		sChrPlane |= prevSChrPlane;
		sPlane1 |= prevSPlane1;
		sPlane2 |= prevSPlane2;
		tmpChrPlane = tmpPlane1 | tmpPlane2;
		tmpChrPlane &= (0xFF<<(8-x));
		tmpPlane1 &= (0xFF<<(8-x));
		tmpPlane2 &= (0xFF<<(8-x));
		bgPlane1 |= (tmpPlane1>>(8-x));
    bgPlane2 |= (tmpPlane2>>(8-x));
		bgChrPlane |= (tmpChrPlane>>(8-x));

		yPos -= pixelSize;
		xPos = colm*8*pixelSize;
		for (int j=0;j<8;j++){
			drawPixel = true;
	  	if ((oamAddr != -1) && (attr & 0b00100000)==0){
				if (sChrPlane & 128>>j)
						palletAddr = 0x3F10 + (sPalletNum<<2) +  (((sPlane2 & 128>>j)/(128>>j))<<1) + (sPlane1 & 128>>j)/(128>>j);
				else
						palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2 & 128>>j)/(128>>j))<<1) + (bgPlane1 & 128>>j)/(128>>j);
				}
			else if ((prevOamAddr != -1) && (prevAttr & 0b00100000)==0){
			 	if (prevSChrPlane & 128>>j)
					palletAddr = 0x3F10 + (prevSPalletNum<<2) +  (((prevSPlane2 & 128>>j)/(128>>j))<<1) + (prevSPlane1 & 128>>j)/(128>>j);
				else
					palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2 & 128>>j)/(128>>j))<<1) + (bgPlane1 & 128>>j)/(128>>j);
			}
			else{
				if (bgChrPlane & 128>>j){
					palletAddr = 0x3F00 + (((j<(8-x))?bgPalletNum:tmpPalletNum)<<2) +  (((bgPlane2 & 128>>j)/(128>>j))<<1) + (bgPlane1 & 128>>j)/(128>>j);
				}
				else{
					drawPixel = false;
					palletAddr = 0x3F00;
				}
			}
			if (currPallet != palletAddr){
				glColor3ubv(pallet[memory[palletAddr]]);
				currPallet = palletAddr;
			}
			if (drawPixel)
		 	 glRecti(xPos,yPos,xPos+pixelSize,yPos+pixelSize);
			xPos += pixelSize;
		}
	
		
		/*
		
		int ntAddr = 0x2000 + (row*0x20) + tileNum;
		int bgChrAddr = memory[ntAddr]*0x10+0x1000;

		int oamAddr = -1;
		for (int i=0;i<246;i += 4){
			if (OAM[i]/8 == row && OAM[i+3]/8 == colm)
				oamAddr = i;
		}
	
		int syPos = 0;
		int tile = 0;
		int attr = 0;
	  int sxPos = 0;
		
		if (oamAddr != -1){
		  syPos = OAM[oamAddr];
			tile = OAM[oamAddr+1];
			attr = OAM[oamAddr+2];
			sxPos = OAM[oamAddr+3];
		}

		int sChrAddr = tile*0x10;
		
		int attrAddr = 0x2300 + ((row/8+0xc)*0x10) + 	((colm/4)+(((row%8)/4)*8));
		
		int bgPlane1 = memory[bgChrAddr+tileRow];
		int bgPlane2 = memory[bgChrAddr+tileRow+8];
		int sPlane1 = memory[sChrAddr+tileRow];
		int sPlane2 = memory[sChrAddr+tileRow+8];
	
		
		unsigned char lookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
		if (attr & 0b01000000){
			sPlane1 = (lookup[sPlane1&0xf]<<4) | lookup[sPlane1>>4];	
			sPlane2 = (lookup[sPlane2&0xf]<<4) | lookup[sPlane2>>4];
			
		}		

		int attrQuad = (((row%4)/2)<<1) + ((colm%4)/2);
		int bgPalletNum = ((0b11<<(2*attrQuad)) & memory[attrAddr])>>(2*attrQuad);
		int sPalletNum = (attr & 3);
		int palletAddr = 0x3f00;

		int bgChrPlane = bgPlane1 | bgPlane2;
		int sChrPlane = sPlane1 | sPlane2;
		
	//	cout << dec << "Row: " << row << " Colm:" << colm << " Addr:" << oamAddr/4 << " Xpos:" << sxPos << " YPos:" << syPos << hex << " Tile:" << tileNum << " Attr:" << attr << endl;
		
		int pixelSize = 1;
		glColor3f(1,1,1);
		int xPos = colm*8*pixelSize;	
		int yPos = glutGet(GLUT_WINDOW_HEIGHT)-(scanlines*pixelSize);

		
		for (int j=0;j<8;j++){
			if (oamAddr != -1 && (attr & 0b00100000)==0){
				if (sChrPlane & 128>>j)
					palletAddr = 0x3F10 + (sPalletNum<<2) +  (((sPlane2 & 128>>j)/(128>>j))<<1) + (sPlane1 & 128>>j)/(128>>j);
				else
					palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2 & 128>>j)/(128>>j))<<1) + (bgPlane1 & 128>>j)/(128>>j);
			}
			else{
				if (bgChrPlane & 128>>j)
					palletAddr = 0x3F00 + (bgPalletNum<<2) +  (((bgPlane2 & 128>>j)/(128>>j))<<1) + (bgPlane1 & 128>>j)/(128>>j);
				else
					palletAddr = 0x3F00;
			}
			glColor3ubv(pallet[memory[palletAddr]]);
			glRecti(xPos,yPos,xPos+pixelSize,yPos+pixelSize);
			xPos += pixelSize;
		}
	*/
	
		tileNum++;
	}
	//glFlush();
//	cout << (glutGet(GLUT_ELAPSED_TIME)-startTime) << endl;
}

void ppu::drawBackground(){
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//	Display Nametables
	int ntAddr = 0x2000;
	int ntNum = 0;
	while (ntAddr < 0x3000){
		int ntOffsetX = 0;
		int ntOffsetY = 0;
		if ((ntAddr >= 0x2400 && ntAddr <= 0x27FF) || ntAddr >= 0x2C00)
			ntOffsetX = 1;
		if (ntAddr >= 0x2800)
			ntOffsetY = 1;
		int chrAddr = nes->PPURead(ntAddr)*0x10+(nes->readMemory(0x2000)&0x10)*0x100;
		int *plane1 = new int[8];
		int *plane2 = new int[8];
		
		int row = ntNum/32;
		int colm = ntNum%32;
		int attrAddr = 0x2300 + ((row/8+0xc)*0x10) + 	((colm/4)+(((row%8)/4)*8));
		//cout << hex << ntAddr << ' ' << chrAddr << " ";
	
		for (int i=0;i<16;i++){
			if (i < 8)
				plane1[i] = nes->PPURead(chrAddr+i);
			else
				plane2[i-8] = nes->PPURead(chrAddr+i);
		}	
		
		int attrQuad = (((row%4)/2)<<1) + ((colm%4)/2);
		int palletNum = ((0b11<<(2*attrQuad)) & nes->PPURead(attrAddr))>>(2*attrQuad);
		//int palletNum = 0;
		//cout << hex << palletNum << "\n";
		int palletAddr = 0x3f00;
		int *chrPlane = new int[8];
		for (int i=0;i<8;i++){
			chrPlane[i] = plane1[i] | plane2[i];
		}
	
		int pixelSize = 1;
		glColor3f(1,1,1);
		int xPos = (ntNum%32)*8*pixelSize;	
		int yPos = ((ntNum/32)*8*pixelSize+(240*ntOffsetY));
		int color = 0;

		for (int i=0;i<8;i++){
			yPos += pixelSize;
			xPos = (ntNum%32)*8*pixelSize+(256*ntOffsetX);
			for (int j=0;j<8;j++){
				if (chrPlane[i] & 128>>j)
					palletAddr = 0x3F00 + (palletNum<<2) +  (((plane2[i] & 128>>j)/(128>>j))<<1) + (plane1[i] & 128>>j)/(128>>j);
				else
					palletAddr = 0x3F00;
				glColor3ubv(pallet[nes->PPURead(palletAddr)]);
				//cout << hex << "\t" << palletAddr << dec << ' ' << (int)pallet[memory[palletAddr]][0] << ' ' << (int)pallet[memory[palletAddr]][1] << ' ' << (int)pallet[memory[palletAddr]][2] << endl;
				float posX = ((xPos)-256)/256.0;
				float posY = ((240-yPos)/240.0);
				glRectf(posX,posY,posX+(1.0/256),posY+(1.0/240));
			//	glRecti(xPos,yPos,(xPos)+(pixelSize,(yPos)+pixelSize);
				xPos += pixelSize;
			}
		}
		
		if (ntNum == 0x3FF)
			ntNum = 0;
		else
			ntNum++;
		ntAddr++;		
	}
	//glFlush();

}

void ppu::drawSprites(){
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
		//Sprites
	int oamAddr = 0;
	int sNum = 0;
	while (oamAddr < 0x100){

		int syPos = OAM[oamAddr];
		int tileNum = OAM[oamAddr+1];
		int attr = OAM[oamAddr+2];
		int sxPos = OAM[oamAddr+3];
		//cout << dec << oamAddr/4 << " Xpos:" << sxPos << " YPos:" << syPos << hex << " Tile:" << tileNum << " Attr:" << attr << endl;
		int chrAddr = tileNum*0x10;
		int palletNum = (attr & 3);
		int palletAddr = 0x3f10;
		
		int *plane1 = new int[8];
		int *plane2 = new int[8];

		for (int i=0;i<16;i++){
			if (i < 8)
				plane1[i] = memory[chrAddr+i];
			else
				plane2[i-8] = memory[chrAddr+i];
		}	
		
		int *chrPlane = new int[8];
		for (int i=0;i<8;i++){
			chrPlane[i] = plane1[i] | plane2[i];
		}
	
		int pixelSize = 2;
		glColor3f(1,1,1);
		int xPos = (sNum%16)*8*pixelSize;	
		int yPos = glutGet(GLUT_WINDOW_HEIGHT)-((sNum/16)*8*pixelSize);
		int color = 0;

		for (int i=0;i<8;i++){
			yPos -= pixelSize;
			xPos = (sNum%16)*8*pixelSize;
			for (int j=0;j<8;j++){
				if (chrPlane[i] & 128>>j)
					palletAddr = 0x3F10 + (palletNum<<2) +  (((plane2[i] & 128>>j)/(128>>j))<<1) + (plane1[i] & 128>>j)/(128>>j);
				else
					palletAddr = 0x3F10;
				glColor3ubv(pallet[memory[palletAddr]]);
				glRecti(xPos,yPos,xPos+pixelSize,yPos+pixelSize);
				xPos += pixelSize;
			}
		}

		oamAddr += 4;
		sNum++;
	}
	glFlush();

}
