#include "NES.h"
#include "cpu.h"
#include <fstream>
#include <bitset>

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
	//Initialize CPU
	a=0;x=0;y=0;pc=0;s=0;p=0;
	pc = 0x8000;
	s = 0xfd;
	p = 0x24;
	cycles = 0;
	numCycles = 0;
	vBlank = false;
	instrAddr = 0;
	instrVal = 0;
	instrIndex = 0;
	tmpPC = 0;
	instrMode = 0;
	
	//List of opcodes
	//{AddressMode,Instruction,numCycles}
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

//Run next Instruction
int cpu::runInstructions(){
	//If valid pc
	int extraCycles = 0;
	if(pc){
		//Log Instruction
		if (cycles == 0){
			if(LOG)cout << hex << pc << " A:"<< (int)a << " X:" << (int)x << " Y:" << (int)y << " 0x6000 " << nes->readMemory(0x6000) << dec << " PPU Cycles:"<< nes->PPU.cycles << " PPU Scanline:" << nes->PPU.scanlines << " PPU Frames:" << nes->PPU.frames <<" p:" << bitset <8> (p) << ' ';\
			int instr = nes->readMemory(pc++);
			cycles = opcodes[instr].cycles;
			
			//Run Instruction
			int extra1 = (this->*opcodes[instr].addrMode)();
			int extra2 = (this->*opcodes[instr].opcode)();
			extraCycles += extra1+extra2;
			cycles += (extra1&extra2);
			numCycles += cycles;
			//Cycle PPU and APU
			//nes->PPU.clock(3*cycles);
			//nes->APU.clock(cycles);
		}
		
		cycles--;
	}
	return extraCycles;
}

//Interrupt Request
void cpu::IRQ(){
	//return if Interrupts are disabled
	if (p & P_IRQ)
		return;
	
	//Disable interupts
	p |= (P_B2 | P_IRQ);
	//Push PC to stack
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	//Push P register to stack
	nes->writeMemory(0x0100+s, p);
	s--;

	//Jump to Interupt address
	pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);
	
}

//Non-maskable interrupt
void cpu::NMI(){
	//cycle PPU
  nes->PPU.clock(3*8);
	//Disable interupts
	p |= (P_B2 | P_IRQ);
	//Push PC to stack
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	//Push P register to stack
	nes->writeMemory(0x0100+s, p);
	s--;

	//Jump to Interrupt address
	pc = nes->readMemory(0xFFFA)+(nes->readMemory(0xFFFB)*256);
}

//Reset CPU
void cpu::reset(){
	//Jump to Starting address
	pc = nes->readMemory(0xFFFC)+(nes->readMemory(0xFFFD)*256);
	a = 0;
	x = 0;
	y = 0;
	s = 0xFD;
	p = 0;
}
//Address Modes
//Implicit
int cpu::impl(){
	return 0;
}

//Accumulator
int cpu::acc(){
	instrAddr = -1;
	instrVal = a;
	return 0;
}

//Absolute
int cpu::abs(){
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	instrAddr = (x2*256)+x1;
	instrVal = nes->readMemory((x2*256)+x1);
	return 0;
}

//Absolute X
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

//Absolute Y
int cpu::absY(){
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	instrAddr = ((x2*256)+x1+y)%0x10000;
	instrVal = nes->readMemory(instrAddr);
	if (x1+y > 0xFF)
		return 1;
	else
		return 0;
}

//Immediate
int cpu::imm(){
	instrVal = nes->readMemory(pc++);
	instrAddr = -2;
	return 0;
}

//Indirect
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

//Indirect X
int cpu::indX(){
	int x1 = nes->readMemory(pc++);
	int addr = (x1+x)%256;
	instrAddr = (nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr);
	instrVal = nes->readMemory((nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr));
	return 0;
}

//Indirect Y
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

//Relative
int cpu::rel(){
	int x1 = nes->readMemory(pc++);
	if (x1 & 0x80)
		x1 = x1-0x100;
	instrVal = x1;
	return 0;
}

//Zero page
int cpu::zpg(){
	int x1 = nes->readMemory(pc++);
	instrAddr = x1;
	instrVal = nes->readMemory(x1);
	return 0;
}

//Zero page X
int cpu::zpgX(){
	int x1 = nes->readMemory(pc++);
	instrAddr = (x1+x)%256;
	instrVal = nes->readMemory((x1+x)%256);
	return 0;
}

//Zero page Y
int cpu::zpgY(){
	int x1 = nes->readMemory(pc++);
	instrAddr = (x1+y)%256;
	instrVal = nes->readMemory((x1+y)%256);
	return 0;
}

//Add with Carry
int cpu::ADC(){
	//Log Instruction
	if(LOG)cout << "ADC" << endl;
	//Add with a register and Carry flag
	int tmp = a + instrVal + (p & P_CARRY);
	//Set overflow flag
	if (a <= 127 && instrVal <= 127 && tmp > 127){
		p |= P_OVERFLOW;
	}
	else if (a > 127 && instrVal > 127 && tmp <= 127) {
		p |= P_OVERFLOW;
	}
	else {
		p &= ~P_OVERFLOW;
		}
	
	//Store in A register
	a = tmp;
	
	//Set flags
	if (tmp >= 256){
		p |= P_CARRY;
	}
	else
		p &= ~P_CARRY;
	
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
  
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 1;
}

//Logical AND
int cpu::AND(){
	//Log Instruction
	if(LOG)cout << "AND" << endl;
	//AND A with value
	a &= (0xFF&instrVal);
	
	//Set flags
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 1;
}

//Arithmetic Shift Left
int cpu::ASL(){
	//Log Instruction
	if(LOG)cout << "ASL" << endl;
	//Clear Negative and Carry
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry Flag to the MSB of instrVal
	p |= ((instrVal & 0b10000000)>>7);
	//Shift instrVal to the left
	instrVal = (instrVal<<1) & 0b11111110;
	//Store value
	if (instrAddr == -1) a = 0xFF&instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	//Set Flags
	p |= (instrVal & 0b10000000);
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Branch if Carry Clear
int cpu::BCC(){
	//Log instruction
	if(LOG)cout << "BCC" << endl;
	//Return if Carry is set
	if ((p & P_CARRY))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Branch if Carry Set
int cpu::BCS(){
	//Log Instruction
	if(LOG)cout << "BCS" << endl;
	//Return if Carry is clear
	if (!(p & P_CARRY))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Branch if Equal
int cpu::BEQ(){
	//Log Instruction
	if(LOG)cout << "BEQ" << endl;
	//Return if Zero is clear
	if (!(p & 0b00000010))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Bit Test
int cpu::BIT(){
	//Log Instruction
	if(LOG)cout << "BIT" << endl;
	//Clear Flags
	p &= ~(P_NEGATIVE | P_OVERFLOW | P_ZERO);
	//Set Negative and overflow flags
	p |= (instrVal & 0b11000000);
	//Clear MSB of PPU Reg 0x2002
	if (instrAddr == 0x2002)
		nes->writeMemory(0x2002, *ppuReg2002&0x7F);
	//Set Zero Flag
	if (!(a & instrVal)) p |= P_ZERO; 
	return 0;
}

//Branch if Minus
int cpu::BMI(){
	//Log Instruction
	if(LOG)cout << "BMI" << endl;
	//Return if Negative is clear
	if (!(p & P_NEGATIVE))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Branch if not equal
int cpu::BNE(){
	//Log Instruction
	if(LOG)cout << "BNE" << endl;
	//Return if Zero is Set
	if ((p & P_ZERO))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Branch if positive
int cpu::BPL(){
	//Log Instruction
	if(LOG)cout << "BPL" << endl;
	//Return if Negative is Set
	if ((p & P_NEGATIVE))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Force Interrupt
int cpu::BRK(){
	//Log Instruction
	if(LOG)cout << "BRK" << endl;
	//Set Flags
	p |= (P_B2 | P_B1 | P_IRQ);
	//Write PC and P registers to stack
	pc++;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	nes->writeMemory(0x0100+s, p);
	s--;
	
	//Jump to Interrupt addresss
	pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);
	return 0;
}

//Branch if Overflow Clear
int cpu::BVC(){
	//Log Instruction
	if(LOG)cout << "BVC" << endl;
	//Return if Overflow is Set
	if ((p & P_OVERFLOW))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Branch if Overflow Set
int cpu::BVS(){
	//Log Instruction
	if(LOG)cout << "BVS" << endl;
	//Return if Overflow is Clear
	if (!(p & P_OVERFLOW))
		return 0;
	//Add extra cycle if new address is not on Zero page
	if ((pc&0x00FF) != ((pc+instrVal)&0x00FF))
		cycles++;
	//Jump to new address
	pc += instrVal;
	return 1;
}

//Clear Carry flag
int cpu::CLC(){
	//Log Instruction
	if(LOG)cout << "CLC" << endl;
	p &= ~P_CARRY;
	return 0;
}

//Clear Decimal Flag
int cpu::CLD(){
	//Log Instruction
	if(LOG)cout << "CLD" << endl;
	p &= ~P_DECIMAL;
	return 0;
}

//Clear Interrupt Flag
int cpu::CLI(){
	//Log Instruction
	if(LOG)cout << "CLI" << endl;
	p &= ~P_IRQ;
	return 0;
}

//Clear Overflow Flag
int cpu::CLV(){
	//Log Instruction
	if(LOG)cout << "CLV" << endl;
	p &= ~P_OVERFLOW;
	return 0;
}

//Compare
int cpu::CMP(){
	//Log Instruction
	if(LOG)cout << "CMP" << endl;	
	//Subtract instrVal from A 
	int diff = a - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & 0b10000000);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;
	else p &= ~P_CARRY;	
	return 1;
}

//Compare X Register
int cpu::CPX(){
	//Log Instruction
	if(LOG)cout << "CPX" << endl;
	//Subtract instrVal from A
	int diff = x - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & 0b10000000);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;	
	else p &= ~P_CARRY;	
	return 0;
}

//Compare Y Register
int cpu::CPY(){
	//Log Instruction
	if(LOG)cout << "CPY" << endl;	
	//Subtract instrVal from A
	int diff = y - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & 0b10000000);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;	
	else p &= ~P_CARRY;	
	return 0;
}

//Decrement Memory
int cpu::DEC(){
	//Log Instruction
	if(LOG)cout << "DEC" << endl;
	//Decrement Memory at instrAddr
	if (nes->readMemory(instrAddr) == 0)
		nes->writeMemory(instrAddr, 0xFF);
	else
		nes->writeMemory(instrAddr, nes->readMemory(instrAddr)-1);
	//Set Negative flag to MSB of the memory at instrAddr
	p &= ~P_NEGATIVE;
	p |= (nes->readMemory(instrAddr) & 0b10000000);
	//Set Zero flag
	if (nes->readMemory(instrAddr) == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Decrement X Register
int cpu::DEX(){
	//Log Instruction
	if(LOG)cout << "DEX" << endl;
	//Decrement X Register
	if (x == 0)
		x = 0xFF;
	else
		x--;
	//Set Negative flag to MSB of X Register
	p &= ~P_NEGATIVE;
	p |= (x & 0b10000000);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Decrement Y Register
int cpu::DEY(){
	//Log Instruction
	if(LOG)cout << "DEY" << endl;
	//Decrement Y Register
	if (y == 0)
		y = 0xFF;
	else
		y--;
	//Set Negative flag to MSB of Y Register
	p &= ~P_NEGATIVE;
	p |= (y & 0b10000000);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Exclusive OR
int cpu::EOR(){
	//Log Instruction
	if(LOG)cout << "EOR" << endl;
	//Exclusive OR with instrVal
	a ^= (0xFF&instrVal);
	//Set Zero Flags
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A Register
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 1;
}

//Increment Memory
int cpu::INC(){
	//Log Instruction
	if(LOG)cout << "INC" << endl;
	//Increment Memory
	nes->writeMemory(instrAddr, nes->readMemory(instrAddr)+1);
	//Wrap value if greater them 256
	if (nes->readMemory(instrAddr) >= 256)
		nes->writeMemory(instrAddr, nes->readMemory(instrAddr) - 256);
	//Set Negative flag to MSB of memory value
	p &= ~P_NEGATIVE;
	p |= (nes->readMemory(instrAddr) & 0b10000000);
	//Set Zero flag
	if (nes->readMemory(instrAddr) == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Increment X Register
int cpu::INX(){
	//Log Instruction
	if(LOG)cout << "INX" << endl;
	//Increment X Register
	x++;
	//Set Negative flag to MSB of X register
	p &= ~P_NEGATIVE;
	p |= (x & 0b10000000);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Increment Y Register
int cpu::INY(){
	//Log Instruction
	if(LOG)cout << "INY" << endl;
	//Increment Y Register
	y++;
	//Set Negative flag to MSB of Y Register
	p &= ~P_NEGATIVE;
	p |= (y & 0b10000000);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Jump
int cpu::JMP(){
	//Log Instruction
	if(LOG)cout << "JMP" << endl;
	//Jump to instrAddr
	pc = instrAddr;
	return 0;
}

//Jump to Subroutine
int cpu::JSR(){
	//Log Instruction
	if(LOG)cout << "JSR" << endl;
	//Push PC to stack
	pc--;
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	//Jump to instrAddr
	pc = instrAddr;
	return 0;
	
}

//Load Accumulator
int cpu::LDA(){
	//Log Instruction
	if(LOG)cout << "LDA " << hex << instrAddr << ' ' << instrVal << endl;
	//Read controllers
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		a = 0xFF&nes->CONTRL.readController(instrAddr-0x4016);
	//Load A with instrVal
	else
		a = 0xFF&instrVal;
	//Clear MSB of PPU Reg 0x2002
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002& 0b01111111);
		nes->PPU.w = 0;
	}
	//Read from PPU VRAM
	else if (instrAddr == 0x2007){
		//Read from VBuffer
		if (nes->PPU.v < 0x3f00) a = 0xFF&nes->PPU.vBuffer;
		//Read from VRAM
		else a = 0xFF&nes->PPURead(nes->PPU.v%0x4000);
		//Set VBuffer equal to VRAM
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);
		//Increment PPU VRAM by value in 0x2000
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 1;
}

//Load X Register
int cpu::LDX(){
	//Log Instruction
	if(LOG)cout << "LDX" << endl;
	//Read controllers
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		x = nes->CONTRL.readController(instrAddr-0x4016);
	//Load X register
	else
		x = instrVal;
	//Clear MSB of PPU Reg 0x20002
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b01111111);
		nes->PPU.w = 0;
	}
	//Read from PPU VRAM
	else if (instrAddr == 0x2007){
		//Read from PPU VBuffer
		if (nes->PPU.v < 0x3f00) x = nes->PPU.vBuffer;
		//Read from PPU VRAM
		else x = nes->PPURead(nes->PPU.v%0x4000);
		//Set PPU VBuffer equal to VRAM
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);	
		//Incrememnt VRAM by value in 0x2000
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	//Set Negative flag to MSB of X Register
	p &= ~P_NEGATIVE;
	p |= (x & 0b10000000);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 1;
}

//Load Y Register
int cpu::LDY(){
	//Log Instruction
	if(LOG)cout << "LDY" << endl;
	//Read controllers
	if (instrAddr == 0x4016 || instrAddr == 0x4017)
		y = nes->CONTRL.readController(instrAddr-0x4016);
	//Load Y Register
	else
		y = instrVal;
	//Clear MSB of PPU Reg 0x2002
	if (instrAddr == 0x2002){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b01111111);
		nes->PPU.w = 0;
	}
	//Read from PPU VRAM
	else if (instrAddr == 0x2007){
		//Read from VBuffer
		if (nes->PPU.v < 0x3f00) y = nes->PPU.vBuffer;
		//Read from VRAM
		else y = nes->PPURead(nes->PPU.v%0x4000);
		//Set VBuffer equal to VRAM
		nes->PPU.vBuffer = nes->PPURead(nes->PPU.v%0x3f00);
		//Increment VRAM by value in 0x2000
		if (*ppuReg2000 & 0b00000100)
			nes->PPU.v += 32;
		else
			nes->PPU.v++;
	}
	//Set Negative flag to MSB of Y Register
	p &= ~P_NEGATIVE;
	p |= (y & 0b10000000);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 1;
}

//Logical Shift Right
int cpu::LSR(){
	//Log Instruction
	if(LOG)cout << "LSR" << endl;	
	//Clear Negative and Carry flags
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry flag equal to LSB of instrVal
	p |= (instrVal & 0b00000001);
	//Shift instrVal to the right
	instrVal = (instrVal>>1) & 0b01111111;
	//Store instrVal
	if (instrAddr == -1) a = 0xFF&instrVal; 
	else nes->writeMemory(instrAddr, instrVal);
	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//No operation
int cpu::NOP(){
	//Log Instruction	
	if(LOG)cout << "NOP" << endl;
	return 0;
}

//Logical Inclusive OR
int cpu::ORA(){
	//Log Instruction
	if(LOG)cout << "ORA" << endl;
	//OR A with instrVal
	a |= (0xFF&instrVal);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 1;
}	

//Push Accumlator
int cpu::PHA(){
	//Log Instruction
	if(LOG)cout << "PHA" << endl;
	//Push A to stack
	nes->writeMemory(0x0100+s, a);
	s--;
	return 0;
}

//Push processor status
int cpu::PHP(){
	//Log Instruction
	if(LOG)cout << "PHP" << endl;
	//Set B flags
	p |= (P_B1 | P_B2);
	//Push P register
	nes->writeMemory(0x0100+s, p);
	s--;
	return 0;
}

//Pull Accumulator
int cpu::PLA(){
	//Log Instruction
	if(LOG)cout << "PLA" << endl;
	//Pull A from stack
	s++;
	a = nes->readMemory(0x0100+s);
	//Set Zero Flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 0;
}

//Pull processor status
int cpu::PLP(){
	//Log Instruction
	if(LOG)cout << "PLP" << endl;
	//Pull P register from stack
	s++;
	p = nes->readMemory(0x0100+s);
	return 0;
}

//Rotate Left
int cpu::ROL(){
	//Log Instruction
	if(LOG)cout << "ROL" << endl;
	//Get Carry flag
	int m = (p & P_CARRY);
	//Clear Negative and Carry flags
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry flag to MSB of instrVal
	p |= ((instrVal & 0b10000000)>>7);	
	//Shift instrVal to the left
	instrVal = (instrVal<<1) & 0b11111110;
	//Set LSB of instrVal to original carry value
	instrVal |= m;
	//Store instrVal
	if (instrAddr == -1) a = 0xFF&instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of instrVal
	p |= (instrVal & 0b10000000);
	return 0;
}

//Rotate right
int cpu::ROR(){
	//Log Instruction
	if(LOG)cout << "ROR" << endl;
	//Get Carry flag
	uint8_t n = 0;
	n |= (p & P_CARRY) << 7;
	//Clear Negative and Carry flag
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry flag to LSB of instrVal
	p |= (instrVal & 0b0000001);
	//Shift instrVal to the right
	instrVal = (instrVal>>1) & 0b01111111;
	//Set the MSB of instrVal to the old Carry value
	instrVal |= n;
	//Store instrVal
	if (instrAddr == -1) a = 0xFF&instrVal;
	else nes->writeMemory(instrAddr,instrVal);
	//Set Negative flag to MSB of instrVal
	p |= (instrVal & 0b10000000);
	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Return from Interrupt 
int cpu::RTI(){
	//Log Instruction
	if(LOG)cout << "RTI" << endl;
	//Pull P and PC registers from the stack
	s++;
	p = nes->readMemory(0x0100+s);
	s++;
	pc = nes->readMemory(0x0100+s);
	s++;
	pc += (nes->readMemory(0x0100+s)*256);
	return 0;
}

//Return from Subroutine
int cpu::RTS(){
	//Log Instruction
	if(LOG)cout << "RTS" << endl;	
	//Pull PC from stack
	s++;
	pc = nes->readMemory(0x0100+s);
	s++; 
	pc += (nes->readMemory(0x0100+s)*256);
	pc++;
	return 0;
}

//Subtract with Carry
int cpu::SBC(){
	//Log Instruction
	if(LOG)cout << "SBC" << endl;
	//Subtract A,value and the carry flag
	int value = instrVal ^ 0x00FF;
	int tmp = a + value + (p & P_CARRY);
	//Set Overflow flag
	if ((a^tmp)&(value^tmp)&0x80)
		p |= P_OVERFLOW;
	else 
		p &= ~P_OVERFLOW;
	a = 0xFF&tmp;
	if (tmp >= 256){
		p |= P_CARRY;
	}
	else
		p &= ~P_CARRY;
	
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
  //Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	return 1;
}

//Set Carry Flag
int cpu::SEC(){
	//Log Instruction
	if(LOG)cout << "SEC" << endl;
	p |= P_CARRY;
	return 0;
}

//Set Decimal flag
int cpu::SED(){
	//Log Instruction
	if(LOG)cout << "SED" << endl;
	p |= P_DECIMAL;
	return 0;
}

//Set Interrupt Disable
int cpu::SEI(){
	if(LOG)cout << "SEI" << endl;
	p |= P_IRQ;
	return 0;
}

//Store Accumulator
int cpu::STA(){
	//Log Instruction
	if(LOG)cout << "STA" << endl;
	//Poll Controller
	//if (instrAddr == 0x4016 || instrAddr == 0x4017)
		//nes->CONTRL.pollController(a,instrAddr-0x4016);
	//Store A in instrAddr
	else
		nes->writeMemory(instrAddr, a);
	//If writing to a PPU Register, write bottom 5 bits to PPU Reg 0x2002	
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (a & 0b00011111));
	}
	return 0;
}

//Store X Register
int cpu::STX(){
	//Log Instruction
	if(LOG)cout << "STX" << endl;
	//Poll Controller
	//if (instrAddr == 0x4016 || instrAddr == 0x4017)
		//nes->CONTRL.pollController(x,instrAddr-0x4016);
	//Store X register in instrAddr
	else
		nes->writeMemory(instrAddr, x);
	//If writing to a PPU Register, write bottom 5 bits to PPU Reg 0x2002	
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (x & 0b00011111));
	}
	return 0;
	
}

//Store Y Register
int cpu::STY(){
	//Log Instruction
	if(LOG)cout << "STY" << endl;
	//Poll Instruction
	//if (instrAddr == 0x4016 || instrAddr == 0x4017)
		//nes->CONTRL.pollController(y,instrAddr-0x4016);
	//Store Y Register in instrAddr
	else
		nes->writeMemory(instrAddr, y);
	//If writing to a PPU Register, write bottom 5 bits to PPU Reg 0x2002	
	if ((instrAddr >= 0x2000 && instrAddr <= 0x2007 && instrAddr != 0x2002)){
		nes->writeMemory(0x2002, *ppuReg2002 & 0b11100000);
		nes->writeMemory(0x2002, *ppuReg2002 | (y & 0b00011111));
	}
	return 0;
}

//Transfer Accumulator to X
int cpu::TAX(){
	//Log Instruction
	if(LOG)cout << "TAX" << endl;
	//Set X equal to A
	x = a;
	//Set Negative Flag equal to MSB of X register
	p &= ~P_NEGATIVE;
	p |= (x & 0b10000000);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Transfer Accumulator to Y
int cpu::TAY(){
	//Log Instruction
	if(LOG)cout << "TAY" << endl;
	//Set Y equal to A
	y = a;
	//Set Negative flag to MSB of Y Register
	p &= ~P_NEGATIVE;
	p |= (y & 0b10000000);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Transfer Stack Pointer to X
int cpu::TSX(){
	//Log Instruction
	if(LOG)cout << "TSX" << endl;
	//Set X equal to S 
	x = s;
	//Set Negative flag to MSB of X register
	p &= ~P_NEGATIVE;
	p |= (x & 0b10000000);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Transfer X to A
int cpu::TXA(){
	//Log Instruction
	if(LOG)cout << "TXA" << endl;
	//Set A equal to X
	a = x;
	//Set Negative flag to MSB of A
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}

//Transfer X to Stack Pointer
int cpu::TXS(){
	//Log Instruction
	if(LOG)cout << "TXS" << endl;
	s = x;
	return 0;
}

//Transfer Y to Accumulator
int cpu::TYA(){
	//Log Instruction
	if(LOG)cout << "TYA" << endl;
	//Set A equal to Y
	a = y;
	//Set Negative flag to MSB of A
	p &= ~P_NEGATIVE;
	p |= (a & 0b10000000);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	return 0;
}
