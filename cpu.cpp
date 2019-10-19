#include "NES.h"
#include <fstream>
#include <bitset>
#include <GL/glut.h>
#include <GL/gl.h>

using namespace std;

/*Address Modes
	0: Accumulator
	1: absolute
	2: absolute,X
	3: absolute,Y
	4: immediate
	5: implied
	6: indirect
	7: X,indirect
	8: indirect,Y
	9: relative
	10: zeropage
	11: zeropage,X
	12: zeropage,Y
*/
bool LOG = false;
cpu::cpu(){
	a=0;x=0;y=0;pc=0;s=0;p=0;
//	memory = new int[0x10000];
	pc = 0x8000;
	s = 0xfd;
	p = 0x24;
	cycles = 0;
	vBlank = false;
	instrAddr = 0;
	instrVal = 0;
	instrIndex = 0;
	tmpPC = 0;
	instrMode = 0;
	
	opcodes = {
						{&cpu::BRK,&cpu::impl,7},{&cpu::ORA,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::zpg,3},{&cpu::ASL,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PHP,&cpu::impl,3},{&cpu::ORA,&cpu::imm,2},{&cpu::ASL,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::abs,4},{&cpu::ASL,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BPL,&cpu::rel,2},{&cpu::ORA,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::zpgX,4},{&cpu::ASL,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLC,&cpu::impl,2},{&cpu::ORA,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::absX,4},{&cpu::ASL,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2},
						{&cpu::JSR,&cpu::abs,6},{&cpu::AND,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::BIT,&cpu::zpg,3},{&cpu::AND,&cpu::zpg,3},{&cpu::ROL,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PLP,&cpu::impl,4},{&cpu::AND,&cpu::imm,2},{&cpu::ROL,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::BIT,&cpu::abs,4},{&cpu::AND,&cpu::abs,4},{&cpu::ROL,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BMI,&cpu::rel,2},{&cpu::AND,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::AND,&cpu::zpgX,4},{&cpu::ROL,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SEC,&cpu::impl,2},{&cpu::AND,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::AND,&cpu::absX,4},{&cpu::ROL,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2},
						{&cpu::RTI,&cpu::impl,6},{&cpu::EOR,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::zpg,3},{&cpu::LSR,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PHA,&cpu::impl,3},{&cpu::EOR,&cpu::imm,2},{&cpu::LSR,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::JMP,&cpu::abs,3},{&cpu::EOR,&cpu::abs,4},{&cpu::LSR,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BVC,&cpu::rel,2},{&cpu::EOR,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::zpgX,4},{&cpu::LSR,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLI,&cpu::impl,2},{&cpu::EOR,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::absX,4},{&cpu::LSR,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2},
						{&cpu::RTS,&cpu::impl,6},{&cpu::ADC,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::zpg,3},{&cpu::ROR,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PLA,&cpu::impl,4},{&cpu::ADC,&cpu::imm,2},{&cpu::ROR,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::JMP,&cpu::ind,5},{&cpu::ADC,&cpu::abs,4},{&cpu::ROR,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BVS,&cpu::rel,2},{&cpu::ADC,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::zpgX,4},{&cpu::ROR,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SEI,&cpu::impl,2},{&cpu::ADC,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::absX,4},{&cpu::ROR,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2},
						{&cpu::NOP,&cpu::impl,2},{&cpu::STA,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::zpg,3},{&cpu::STA,&cpu::zpg,3},{&cpu::STX,&cpu::zpg,3},{&cpu::NOP,&cpu::impl,2},{&cpu::DEY,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::TXA,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::abs,4},{&cpu::STA,&cpu::abs,4},{&cpu::STX,&cpu::abs,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BCC,&cpu::rel,2},{&cpu::STA,&cpu::indY,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::zpgX,4},{&cpu::STA,&cpu::zpgX,4},{&cpu::STX,&cpu::zpgY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::TYA,&cpu::impl,2},{&cpu::STA,&cpu::absY,5},{&cpu::TXS,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STA,&cpu::absX,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},
						{&cpu::LDY,&cpu::imm,2},{&cpu::LDA,&cpu::indX,6},{&cpu::LDX,&cpu::imm,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::zpg,3},{&cpu::LDA,&cpu::zpg,3},{&cpu::LDX,&cpu::zpg,3},{&cpu::NOP,&cpu::impl,2},{&cpu::TAY,&cpu::impl,2},{&cpu::LDA,&cpu::imm,2},{&cpu::TAX,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::abs,4},{&cpu::LDA,&cpu::abs,4},{&cpu::LDX,&cpu::abs,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BCS,&cpu::rel,2},{&cpu::LDA,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::zpgX,4},{&cpu::LDA,&cpu::zpgX,4},{&cpu::LDX,&cpu::zpgY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::CLV,&cpu::impl,2},{&cpu::LDA,&cpu::absY,4},{&cpu::TSX,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::absX,4},{&cpu::LDA,&cpu::absX,4},{&cpu::LDX,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::CPY,&cpu::imm,2},{&cpu::CMP,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPY,&cpu::zpg,3},{&cpu::CMP,&cpu::zpg,3},{&cpu::DEC,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::INY,&cpu::impl,2},{&cpu::CMP,&cpu::imm,2},{&cpu::DEX,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPY,&cpu::abs,4},{&cpu::CMP,&cpu::abs,4},{&cpu::DEC,&cpu::abs,3},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BNE,&cpu::rel,2},{&cpu::CMP,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CMP,&cpu::zpgX,4},{&cpu::DEC,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLD,&cpu::impl,2},{&cpu::CMP,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CMP,&cpu::absX,4},{&cpu::DEC,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2},
						{&cpu::CPX,&cpu::imm,2},{&cpu::SBC,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPX,&cpu::zpg,3},{&cpu::SBC,&cpu::zpg,3},{&cpu::INC,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::INX,&cpu::impl,2},{&cpu::SBC,&cpu::imm,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPX,&cpu::abs,4},{&cpu::SBC,&cpu::abs,4},{&cpu::INC,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BEQ,&cpu::rel,2},{&cpu::SBC,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::SBC,&cpu::zpgX,4},{&cpu::INC,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SED,&cpu::impl,2},{&cpu::SBC,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::SBC,&cpu::absX,4},{&cpu::INC,&cpu::absX,7},{&cpu::NOP,&cpu::impl,2}
};
}

void cpu::runInstructions(){
	 //float startTime = glfwGetTime();
	if(pc){
		if(LOG)cout << hex << pc << " A:"<< a << " X:" << x << " Y:" << y << " 0x6000 " << nes->readMemory(0x6000) << dec << " PPU Cycles:"<< nes->PPU.cycles << " PPU Scanline:" << nes->PPU.scanlines << " PPU Frames:" << nes->PPU.frames <<" p:" << bitset <8> (p) << ' ';
		int instr = nes->readMemory(pc++);
		cycles = opcodes[instr].cycles;
		int extra1 = (this->*opcodes[instr].addrMode)();
		int extra2 = (this->*opcodes[instr].opcode)();
		cycles += (extra1&extra2);
	//	cout << (glfwGetTime()-startTime)*1000 << ' ';
		//startTime = glfwGetTime();
		nes->PPU.cycle(3*cycles);
	}
	//cout << (glfwGetTime()-startTime)*1000 << endl;
}


void cpu::IRQ(){
	if (p & 0b00000100)
		return;
	
	p |= 0b00100100;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	nes->writeMemory(0x0100+s, p);
	s--;

	pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);
	
}

void cpu::NMI(){
  nes->PPU.cycle(3*8);
	p |= 0b00100100;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	nes->writeMemory(0x0100+s, p);
	s--;

	pc = nes->readMemory(0xFFFA)+(nes->readMemory(0xFFFB)*256);
}

void cpu::reset(){
	pc = nes->readMemory(0xFFFC)+(nes->readMemory(0xFFFD)*256);
	a = 0;
	x = 0;
	y = 0;
	s = 0xFD;
	p = 0;
}

int cpu::impl(){
	return 0;
}

int cpu::acc(){
	instrAddr = -1;
	instrVal = a;
	return 0;
}

int cpu::abs(){
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	instrAddr = (x2*256)+x1;
	instrVal = nes->readMemory((x2*256)+x1);
	return 0;
}

int cpu::absX(){
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	instrAddr = (x2*256)+x1+x;
	instrVal = nes->readMemory((x2*256)+x1+x);
	if (x1+x > 0xFF)
		return 1;
	else
		return 0;
}	

int cpu::absY(){
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	instrAddr = (x2*256)+x1+y;
	instrVal = nes->readMemory((x2*256)+x1+y);
	if (x1+y > 0xFF)
		return 1;
	else
		return 0;
}

int cpu::imm(){
	instrVal = nes->readMemory(pc++);
	instrAddr = -2;
	return 0;
}

int cpu::ind() {
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	int addr = (x2*256)+x1;
	if (x1 == 0xff){
		int addr1 = (x2*256);
		instrAddr = ((nes->readMemory(addr1)*256)+nes->readMemory(addr));
		instrVal = nes->readMemory(((nes->readMemory(addr1)*256)+nes->readMemory(addr)));
	}
	else{
		instrAddr = ((nes->readMemory(addr+1)*256)+nes->readMemory(addr));
		instrVal = nes->readMemory((nes->readMemory(addr+1)*256)+nes->readMemory(addr));
	}
	return 0;
}

int cpu::indX(){
	int x1 = nes->readMemory(pc++);
	int addr = (x1+x)%256;
	instrAddr = (nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr);
	instrVal = nes->readMemory((nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr));
	return 0;
}

int cpu::indY(){
	int x1 = nes->readMemory(pc++);
	int addr = x1;
	instrAddr = ((nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr)+y)&0xFFFF;
	instrVal = nes->readMemory(((nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr)+y)&0xFFFF);
	if (addr+y > 0xFF)
		return 1;
	else
		return 0;
}

int cpu::rel(){
	int x1 = nes->readMemory(pc++);
	if (x1 & 0x80)
		x1 = x1-0x100;
	instrVal = x1;
	return 0;
}

int cpu::zpg(){
	int x1 = nes->readMemory(pc++);
	instrAddr = x1;
	instrVal = nes->readMemory(x1);
	return 0;
}

int cpu::zpgX(){
	int x1 = nes->readMemory(pc++);
	instrAddr = (x1+x)%256;
	instrVal = nes->readMemory((x1+x)%256);
	return 0;
}

int cpu::zpgY(){
	int x1 = nes->readMemory(pc++);
	instrAddr = (x1+y)%256;
	instrVal = nes->readMemory((x1+y)%256);
	return 0;
}

int cpu::ADC(){
	if(LOG)cout << "ADC" << endl;
	int cMask = 0b00000001;
	int tmp = a + instrVal + (p & cMask);
	if (a <= 127 && instrVal <= 127 && tmp > 127){
		p |= 0b01000000;
	}
	else if (a > 127 && instrVal > 127 && tmp <= 127) {
		p |= 0b01000000;
	}
	else {
		p &= 0b10111111;
		}
	a = tmp;
	if (a >= 256){
		a -= 256;
		p |= cMask;
	}
	else
		p &= 0b11111110;
	
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
  
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 1;
}

int cpu::AND(){
	if(LOG)cout << "AND" << endl;
	a &= instrVal;
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 1;
}

int cpu::ASL(){
	if(LOG)cout << "ASL" << endl;
	p &= 0b01111110;
	p |= ((instrVal & 0b10000000)>>7);
	instrVal = (instrVal<<1) & 0b11111110;
	if (instrAddr == -1) a = instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	p |= (instrVal & 0b10000000);
	if (instrVal == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::BCC(){
	if(LOG)cout << "BCC" << endl;
	if ((p & 0b00000001))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BCS(){
	if(LOG)cout << "BCS" << endl;
	if (!(p & 0b00000001))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BEQ(){
	if(LOG)cout << "BEQ" << endl;
	if (!(p & 0b00000010))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BIT(){
	if(LOG)cout << "BIT" << endl;
	p &= 0b00111101;
	p |= (instrVal & 0b11000000);
	if (instrAddr == 0x2002)
		nes->writeMemory(0x2002, *ppuReg2002&0x7F);
	if (!(a & instrVal)) p |= 0b00000010; 
	return 0;
}

int cpu::BMI(){
	if(LOG)cout << "BMI" << endl;
	if (!(p & 0b10000000))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BNE(){
	if(LOG)cout << "BNE" << endl;
	if ((p & 0b00000010))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BPL(){
	if(LOG)cout << "BPL" << endl;
	if ((p & 0b10000000))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BRK(){
	if(LOG)cout << "BRK" << endl;
	p |= 0b00110100;
	pc++;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	nes->writeMemory(0x0100+s, p);
	s--;
	
	pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);
	return 0;
}

int cpu::BVC(){
	if(LOG)cout << "BVC" << endl;
	if ((p & 0b01000000))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::BVS(){
	if(LOG)cout << "BVS" << endl;
	if (!(p & 0b01000000))
		return 0;
	//cycles++;
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	pc += instrVal;
	return 1;
}

int cpu::CLC(){
	if(LOG)cout << "CLC" << endl;
	p &= 0b11111110;
	return 0;
}

int cpu::CLD(){
	if(LOG)cout << "CLD" << endl;
	p &= 0b11110111;
	return 0;
}

int cpu::CLI(){
	if(LOG)cout << "CLI" << endl;
	p &= 0b11111011;
	return 0;
}

int cpu::CLV(){
	if(LOG)cout << "CLV" << endl;
	p &= 0b10111111;
	return 0;
}

int cpu::CMP(){
	if(LOG)cout << "CMP" << endl;	
	int diff = a - instrVal;
	p &= 0b01111111;
	p |= (diff & 0b10000000);
	if (diff == 0) p |= 0b00000010;
	else p &= 0b11111101;
	if (diff >= 0) p |= 0b00000001;
	else p &= 0b11111110;	
	return 1;
}

int cpu::CPX(){
	if(LOG)cout << "CPX" << endl;
	int diff = x - instrVal;
	p &= 0b01111111;
	p |= (diff & 0b10000000);
	if (diff == 0) p |= 0b00000010;
	else p &= 0b11111101;
	if (diff >= 0) p |= 0b00000001;	
	else p &= 0b11111110;	
	return 0;
}

int cpu::CPY(){
	if(LOG)cout << "CPY" << endl;	
	int diff = y - instrVal;
	p &= 0b01111111;
	p |= (diff & 0b10000000);
	if (diff == 0) p |= 0b00000010;
	else p &= 0b11111101;
	if (diff >= 0) p |= 0b00000001;	
	else p &= 0b11111110;	
	return 0;
}

int cpu::DEC(){
	if(LOG)cout << "DEC" << endl;
	if (nes->readMemory(instrAddr) == 0)
		nes->writeMemory(instrAddr, 0xFF);
	else
		nes->writeMemory(instrAddr, nes->readMemory(instrAddr)-1);
	p &= 0b01111111;
	p |= (nes->readMemory(instrAddr) & 0b10000000);
	if (nes->readMemory(instrAddr) == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::DEX(){
	if(LOG)cout << "DEX" << endl;
	if (x == 0)
		x = 0xFF;
	else
		x--;
	p &= 0b01111111;
	p |= (x & 0b10000000);
	if (x == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::DEY(){
	if(LOG)cout << "DEY" << endl;
	if (y == 0)
		y = 0xFF;
	else
		y--;
	p &= 0b01111111;
	p |= (y & 0b10000000);
	if (y == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::EOR(){
	if(LOG)cout << "EOR" << endl;
	a ^= instrVal;
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 1;
}

int cpu::INC(){
	if(LOG)cout << "INC" << endl;
	nes->writeMemory(instrAddr, nes->readMemory(instrAddr)+1);
	if (nes->readMemory(instrAddr) >= 256)
		nes->writeMemory(instrAddr, nes->readMemory(instrAddr) - 256);
	p &= 0b01111111;
	p |= (nes->readMemory(instrAddr) & 0b10000000);
	if (nes->readMemory(instrAddr) == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::INX(){
	if(LOG)cout << "INX" << endl;
	(x)++;
	if (x >= 256)
		x -= 256;
	p &= 0b01111111;
	p |= (x & 0b10000000);
	if (x == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::INY(){
	if(LOG)cout << "INY" << endl;
	(y)++;
	if (y >= 256)
		y -= 256;
	p &= 0b01111111;
	p |= (y & 0b10000000);
	if (y == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::JMP(){
	if(LOG)cout << "JMP" << endl;
	pc = instrAddr;
	return 0;
}

int cpu::JSR(){
	if(LOG)cout << "JSR" << endl;
	pc--;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	pc = instrAddr;
	return 0;
	
}

int cpu::LDA(){
	if(LOG)cout << "LDA " << hex << instrAddr << ' ' << instrVal << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		a = nes->CONTRL.readController(instrAddr-0x4016);
	else
		a = instrVal;
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002& 0b01111111);
		nes->PPU.w = 0;
	}
	else if (instrAddr == 0x2007){
		if (nes->PPU.v < 0x3f00) a = nes->PPU.vBuffer;
		else a = nes->PPURead(nes->PPU.v%0x4000);
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	p &= 0b01111111;
	p |= (a & 0b10000000);
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 1;
}

int cpu::LDX(){
	if(LOG)cout << "LDX" << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		x = nes->CONTRL.readController(instrAddr-0x4016);
	else
		x = instrVal;
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b01111111);
		nes->PPU.w = 0;
	}
	else if (instrAddr == 0x2007){
		if (nes->PPU.v < 0x3f00) x = nes->PPU.vBuffer;
		else x = nes->PPURead(nes->PPU.v%0x4000);
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	p &= 0b01111111;
	p |= (x & 0b10000000);
	if (x == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 1;
}

int cpu::LDY(){
	if(LOG)cout << "LDY" << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		y = nes->CONTRL.readController(instrAddr-0x4016);
	else
		y = instrVal;
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b01111111);
		nes->PPU.w = 0;
	}
	else if (instrAddr == 0x2007){
		if (nes->PPU.v < 0x3f00) y = nes->PPU.vBuffer;
		else y = nes->PPURead(nes->PPU.v%0x4000);
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	p &= 0b01111111;
	p |= (y & 0b10000000);
	if (y == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 1;
}

int cpu::LSR(){
	if(LOG)cout << "LSR" << endl;	
	p &= 0b01111110;
	p |= (instrVal & 0b00000001);
	instrVal = (instrVal>>1) & 0b01111111;
	if (instrAddr == -1) a = instrVal; 
	else nes->writeMemory(instrAddr, instrVal);
	if (instrVal == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::NOP(){
	if(LOG)cout << "NOP" << endl;
	return 0;
}

int cpu::ORA(){
	if(LOG)cout << "ORA" << endl;
	a |= instrVal;
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 1;
}	

int cpu::PHA(){
	if(LOG)cout << "PHA" << endl;
	nes->writeMemory(0x0100+s, a);
	s--;
	return 0;
}

int cpu::PHP(){
	if(LOG)cout << "PHP" << endl;
	p |= 0b00110000;
	nes->writeMemory(0x0100+s, p);
	s--;
	return 0;
}

int cpu::PLA(){
	if(LOG)cout << "PLA" << endl;
	s++;
	a = nes->readMemory(0x0100+s);
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 0;
}

int cpu::PLP(){
	if(LOG)cout << "PLP" << endl;
	s++;
	p = nes->readMemory(0x0100+s);
	return 0;
}

int cpu::ROL(){
	if(LOG)cout << "ROL" << endl;
	int m = (p & 0b00000001);
	p &= 0b01111110;
	p |= ((instrVal & 0b10000000)>>7);	
	instrVal = (instrVal<<1) & 0b11111110;
	instrVal |= m;
	if (instrAddr == -1) a = instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	if (instrVal == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p |= (instrVal & 0b10000000);
	return 0;
}

int cpu::ROR(){
	if(LOG)cout << "ROR" << endl;
	unsigned char n = 0;
	n |= (p & 0b00000001);
	n <<= 7;
	p &= 0b01111110;
	p |= (instrVal & 0b0000001);

	instrVal = (instrVal>>1) & 0b01111111;
	instrVal |= n;
	if (instrAddr == -1) a = instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	p |= (instrVal & 0b10000000);
	if (instrVal == 0) p |= 0b00000010;
	else p &= 0b11111101;
	p |= (instrVal & 0b10000000);
	return 0;
}

int cpu::RTI(){
	if(LOG)cout << "RTI" << endl;
	s++;
	p = nes->readMemory(0x0100+s);
	s++;
	pc = nes->readMemory(0x0100+s);
	s++; 
	pc += (nes->readMemory(0x0100+s)*256);
	return 0;
}

int cpu::RTS(){
	if(LOG)cout << "RTS" << endl;	
	s++;
	pc = nes->readMemory(0x0100+s);
	s++; 
	pc += (nes->readMemory(0x0100+s)*256);
	pc++;
	return 0;
}

int cpu::SBC(){
	if(LOG)cout << "SBC" << endl;
	int value = instrVal ^ 0x00FF;
	int cMask = 0b00000001;
	int tmp = a + value + (p & cMask);
	if ((a^tmp)&(value^tmp)&0x80)
		p |= 0b01000000;
	else 
		p &= 0b10111111;
	a = tmp;
	if (a >= 256){
		a -= 256;
		p |= cMask;
	}
	else
		p &= 0b11111110;
	
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
  
	p &= 0b01111111;
	p |= (a & 0b10000000);
	return 1;
}

int cpu::SEC(){
	if(LOG)cout << "SEC" << endl;
	p |= 0b00000001;
	return 0;
}

int cpu::SED(){
	if(LOG)cout << "SED" << endl;
	p |= 0b00001000;
	return 0;
}

int cpu::SEI(){
	if(LOG)cout << "SEI" << endl;
	p |= 0b00000100;
	return 0;
}

int cpu::STA(){
	if(LOG)cout << "STA" << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		nes->CONTRL.pollController(a,instrAddr-0x4016);
	else
		nes->writeMemory(instrAddr, a);
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (a & 0b00011111));
	}
	return 0;
}

int cpu::STX(){
	if(LOG)cout << "STX" << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		nes->CONTRL.pollController(x,instrAddr-0x4016);
	else
		nes->writeMemory(instrAddr, x);
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (x & 0b00011111));
	}
	return 0;
	
}

int cpu::STY(){
	if(LOG)cout << "STY" << endl;
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		nes->CONTRL.pollController(y,instrAddr-0x4016);
	else
		nes->writeMemory(instrAddr, y);
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (y & 0b00011111));
	}
	return 0;
}

int cpu::TAX(){
	if(LOG)cout << "TAX" << endl;
	x = a;
	p &= 0b01111111;
	p |= (x & 0b10000000);
	if (x == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::TAY(){
	if(LOG)cout << "TAY" << endl;
	y = a;
	p &= 0b01111111;
	p |= (y & 0b10000000);
	if (y == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::TSX(){
	if(LOG)cout << "TSX" << endl;
	x = s;
	p &= 0b01111111;
	p |= (x & 0b10000000);
	if (x == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::TXA(){
	if(LOG)cout << "TXA" << endl;
	a = x;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}

int cpu::TXS(){
	if(LOG)cout << "TXS" << endl;
	s = x;
	return 0;
}

int cpu::TYA(){
	if(LOG)cout << "TYA" << endl;
	a = y;
	p &= 0b01111111;
	p |= (a & 0b10000000);
	if (a == 0) p |= 0b00000010;
	else p &= 0b11111101;
	return 0;
}
