#include "NES.h"
#include "cpu.h"
#include <fstream>
#include <bitset>

using namespace std;

bool LOG = false;
cpu::cpu(){
	//Initialize CPU
	a=0;x=0;y=0;pc=0;s=0;p=0;
	pc = 0x8000;
	s = 0xfd;
	p = 0x24;
	cycles = -1;
	cycleCount = 0;

	irqFlag = false;
	//irqDelay = false;
	//irqForce = false;
	//nmiFlag = false;
	nmiLine = true;
	previousNMILine = true;


	//List of opcodes
	//{AddressMode,Instruction,numCycles}
	opcodes = {
						{&cpu::BRK,&cpu::impl,7},{&cpu::ORA,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::zpg,3},{&cpu::ASL,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PHP,&cpu::impl,3},{&cpu::ORA,&cpu::imm,2},{&cpu::ASL,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::abs,4},{&cpu::ASL,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BPL,&cpu::rel,2},{&cpu::ORA,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::zpgX,4},{&cpu::ASL,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLC,&cpu::impl,2},{&cpu::ORA,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ORA,&cpu::absX,4},{&cpu::ASL,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2},
						{&cpu::JSR,&cpu::abs,6},{&cpu::AND,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::BIT,&cpu::zpg,3},{&cpu::AND,&cpu::zpg,3},{&cpu::ROL,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PLP,&cpu::impl,4},{&cpu::AND,&cpu::imm,2},{&cpu::ROL,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::BIT,&cpu::abs,4},{&cpu::AND,&cpu::abs,4},{&cpu::ROL,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BMI,&cpu::rel,2},{&cpu::AND,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::AND,&cpu::zpgX,4},{&cpu::ROL,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SEC,&cpu::impl,2},{&cpu::AND,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::AND,&cpu::absX,4},{&cpu::ROL,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2},
						{&cpu::RTI,&cpu::impl,6},{&cpu::EOR,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::zpg,3},{&cpu::LSR,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PHA,&cpu::impl,3},{&cpu::EOR,&cpu::imm,2},{&cpu::LSR,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::JMP,&cpu::abs,3},{&cpu::EOR,&cpu::abs,4},{&cpu::LSR,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BVC,&cpu::rel,2},{&cpu::EOR,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::zpgX,4},{&cpu::LSR,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLI,&cpu::impl,2},{&cpu::EOR,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::EOR,&cpu::absX,4},{&cpu::LSR,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2},
						{&cpu::RTS,&cpu::impl,6},{&cpu::ADC,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::zpg,3},{&cpu::ROR,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::PLA,&cpu::impl,4},{&cpu::ADC,&cpu::imm,2},{&cpu::ROR,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::JMP,&cpu::ind,5},{&cpu::ADC,&cpu::abs,4},{&cpu::ROR,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BVS,&cpu::rel,2},{&cpu::ADC,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::zpgX,4},{&cpu::ROR,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SEI,&cpu::impl,2},{&cpu::ADC,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::ADC,&cpu::absX,4},{&cpu::ROR,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2},
						{&cpu::NOP,&cpu::impl,2},{&cpu::STA,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::zpg,3},{&cpu::STA,&cpu::zpg,3},{&cpu::STX,&cpu::zpg,3},{&cpu::NOP,&cpu::impl,2},{&cpu::DEY,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::TXA,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::abs,4},{&cpu::STA,&cpu::abs,4},{&cpu::STX,&cpu::abs,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BCC,&cpu::rel,2},{&cpu::STA,&cpu::indY,6,true},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STY,&cpu::zpgX,4},{&cpu::STA,&cpu::zpgX,4},{&cpu::STX,&cpu::zpgY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::TYA,&cpu::impl,2},{&cpu::STA,&cpu::absY,5,true},{&cpu::TXS,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::STA,&cpu::absX,5,true},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},
						{&cpu::LDY,&cpu::imm,2},{&cpu::LDA,&cpu::indX,6},{&cpu::LDX,&cpu::imm,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::zpg,3},{&cpu::LDA,&cpu::zpg,3},{&cpu::LDX,&cpu::zpg,3},{&cpu::NOP,&cpu::impl,2},{&cpu::TAY,&cpu::impl,2},{&cpu::LDA,&cpu::imm,2},{&cpu::TAX,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::abs,4},{&cpu::LDA,&cpu::abs,4},{&cpu::LDX,&cpu::abs,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BCS,&cpu::rel,2},{&cpu::LDA,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::zpgX,4},{&cpu::LDA,&cpu::zpgX,4},{&cpu::LDX,&cpu::zpgY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::CLV,&cpu::impl,2},{&cpu::LDA,&cpu::absY,4},{&cpu::TSX,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::LDY,&cpu::absX,4},{&cpu::LDA,&cpu::absX,4},{&cpu::LDX,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},
						{&cpu::CPY,&cpu::imm,2},{&cpu::CMP,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPY,&cpu::zpg,3},{&cpu::CMP,&cpu::zpg,3},{&cpu::DEC,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::INY,&cpu::impl,2},{&cpu::CMP,&cpu::imm,2},{&cpu::DEX,&cpu::acc,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPY,&cpu::abs,4},{&cpu::CMP,&cpu::abs,4},{&cpu::DEC,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BNE,&cpu::rel,2},{&cpu::CMP,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CMP,&cpu::zpgX,4},{&cpu::DEC,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::CLD,&cpu::impl,2},{&cpu::CMP,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CMP,&cpu::absX,4},{&cpu::DEC,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2},
						{&cpu::CPX,&cpu::imm,2},{&cpu::SBC,&cpu::indX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPX,&cpu::zpg,3},{&cpu::SBC,&cpu::zpg,3},{&cpu::INC,&cpu::zpg,5},{&cpu::NOP,&cpu::impl,2},{&cpu::INX,&cpu::impl,2},{&cpu::SBC,&cpu::imm,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::CPX,&cpu::abs,4},{&cpu::SBC,&cpu::abs,4},{&cpu::INC,&cpu::abs,6},{&cpu::NOP,&cpu::impl,2},
						{&cpu::BEQ,&cpu::rel,2},{&cpu::SBC,&cpu::indY,5},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::SBC,&cpu::zpgX,4},{&cpu::INC,&cpu::zpgX,6},{&cpu::NOP,&cpu::impl,2},{&cpu::SED,&cpu::impl,2},{&cpu::SBC,&cpu::absY,4},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::NOP,&cpu::impl,2},{&cpu::SBC,&cpu::absX,4},{&cpu::INC,&cpu::absX,7,true},{&cpu::NOP,&cpu::impl,2}
};
}

void cpu::IncCpuCycle(){
	nes->PPU.Clock(3);
	nes->CART.mapper->Clock();
	nes->APU.clock(1);

	cycleCount++;
	cycles++;
}

//Run next Instruction
int cpu::runInstructions(){
	//If valid pc
	if(pc || true){
		//Load Instruction
		if (cycles == 0){
			if(LOG)cout << hex << pc << " A:"<< (int)a << " X:" << (int)x << " Y:" << (int)y << " sp:" << (int)s  << dec << " CPU Cycles:" << cycleCount  << " PPU Cycles:"<< nes->PPU.cycles << " PPU Scanline:" << nes->PPU.scanlines << " PPU Frames:" << nes->PPU.frames <<" p:" << bitset <8> (p) << ' ';

			currentInstrCycle = cycleCount;
			//Load instruction value
			instr = nes->readMemory(pc++);
			IncCpuCycle();
		//	cycles = opcodes[instr].cycles;
			extraCycles = 0;

			int extra1 = (this->*opcodes[instr].addrMode)(opcodes[instr].dummyRead);
			int extra2 = (this->*opcodes[instr].opcode)();
			//reset interrupt flags
			irqFlag = false;
			//nmiLine = false;
			previousNMILine = nmiLine;

			extraCycles += (extra1&extra2);
			//add in extra cycles
			//cycles += extraCycles;

		}
		cycles--;
		//cycleCount++;
	}
	return extraCycles;
}

bool cpu::PollInterrupts(){
	//if (!irqFlag)std::cout << irqFlag << std::endl;
	//trigger NMI on edge
	if (nes->CART.PollInterrupts())
		irqFlag = true;

	if(nmiLine == true && previousNMILine == false){
		NMI();
		return true;
	}
	if (irqFlag){
			return IRQ();
	}
	return false;
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

	cycles = 0;
	cycleCount = 8;
	currentInstrCycle = 8;
	instrAddr = 0;
	instrVal = 0;

	irqFlag = false;
	nmiLine = true;
	previousNMILine = true;
}

void cpu::SaveState(std::ofstream &file){
	file.write(reinterpret_cast<char*>(&cycles),sizeof(int));
	file.write(reinterpret_cast<char*>(&extraCycles),sizeof(int));
	file.write(reinterpret_cast<char*>(&instrAddr),sizeof(int));
	file.write(reinterpret_cast<char*>(&instrVal),sizeof(int));
	file.write(reinterpret_cast<char*>(&irqFlag),1);
	file.write(reinterpret_cast<char*>(&nmiLine),1);
	file.write(reinterpret_cast<char*>(&previousNMILine),1);
	file.write(reinterpret_cast<char*>(&a),1);
	file.write(reinterpret_cast<char*>(&x),1);
	file.write(reinterpret_cast<char*>(&y),1);
	file.write(reinterpret_cast<char*>(&s),1);
	file.write(reinterpret_cast<char*>(&p),1);
	file.write(reinterpret_cast<char*>(&pc),2);
}

void cpu::LoadState(std::ifstream &file){
	file.read(reinterpret_cast<char*>(&cycles),sizeof(int));
	file.read(reinterpret_cast<char*>(&extraCycles),sizeof(int));
	file.read(reinterpret_cast<char*>(&instrAddr),sizeof(int));
	file.read(reinterpret_cast<char*>(&instrVal),sizeof(int));
	file.read(reinterpret_cast<char*>(&irqFlag),1);
	file.read(reinterpret_cast<char*>(&nmiLine),1);
	file.read(reinterpret_cast<char*>(&previousNMILine),1);
	file.read(reinterpret_cast<char*>(&a),1);
	file.read(reinterpret_cast<char*>(&x),1);
	file.read(reinterpret_cast<char*>(&y),1);
	file.read(reinterpret_cast<char*>(&s),1);
	file.read(reinterpret_cast<char*>(&p),1);
	file.read(reinterpret_cast<char*>(&pc),2);
}

//Interrupt Request
bool cpu::IRQ(){
	//return if Interrupts are disabled
  if ((p & P_IRQ)){
			return false;
	}

	//cycle PPU
  // nes->PPU.Clock(3*7);
	// cycleCount += 7;

	//Dummy Reads
	IncCpuCycle();
	IncCpuCycle();

	//std::cout << "Scanlines: " << nes->PPU.scanlines << ' ' << nes->PPU.cycles << std::endl;

	//Push PC to stack
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	IncCpuCycle();
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	IncCpuCycle();
	//Push P register to stack
	nes->writeMemory(0x0100+s, p);
	s--;
	IncCpuCycle();

	//Disable interupts
	p |= (P_B2 | P_IRQ);

	//Jump to Interupt address
	pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);
	IncCpuCycle();
	IncCpuCycle();
///std::cout << std::hex << pc << std::endl;

	return true;

}

//Non-maskable interrupt
void cpu::NMI(){
	//cycle PPU
  // nes->PPU.Clock(3*7);
	// cycleCount += 7;

	IncCpuCycle();
	IncCpuCycle();

	//Disable interupts
	//Push PC to stack
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	IncCpuCycle();
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	IncCpuCycle();
	//Push P register to stack
	nes->writeMemory(0x0100+s, p);
	s--;
	IncCpuCycle();

	p |= (P_B2 | P_IRQ);
	//Jump to Interrupt address
	pc = nes->readMemory(0xFFFA)+(nes->readMemory(0xFFFB)*256);
	IncCpuCycle();
	IncCpuCycle();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//Address Modes
//Implicit
int cpu::impl(bool dummyRead){
	if(LOG) std::cout << " impl ";
	IncCpuCycle();
	return 0;
}

//Accumulator
int cpu::acc(bool dummyRead){
	if(LOG) std::cout << " acc ";
	instrAddr = -1;
	instrVal = a;
	IncCpuCycle();
	return 0;
}

//Absolute
int cpu::abs(bool dummyRead){
	if(LOG) std::cout << " abs ";
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	IncCpuCycle();
	IncCpuCycle();
	instrAddr = (x2*256)+x1;
	instrVal = -1;
	return 0;
}

//Absolute X
int cpu::absX(bool dummyRead){
	if(LOG) std::cout << " absX ";
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	IncCpuCycle();
	IncCpuCycle();
	instrAddr = (x2*256)+x1+x;
	instrAddr %= 0x10000;
	instrVal = -1;
	if (x1+x > 0xFF || dummyRead){
		IncCpuCycle();
		return 1;
	}
	else
		return 0;
}

//Absolute Y
int cpu::absY(bool dummyRead){
	if(LOG) std::cout << " absY ";
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	IncCpuCycle();
	IncCpuCycle();
	instrAddr = ((x2*256)+x1+y)%0x10000;
	instrVal = -1;
	if (x1+y > 0xFF || dummyRead){
		IncCpuCycle();
		return 1;
	}
	else
		return 0;
}

//Immediate
int cpu::imm(bool dummyRead){
	if(LOG) std::cout << " imm ";
	instrVal = nes->readMemory(pc++);
	IncCpuCycle();
	instrAddr = -2;
	return 0;
}

//Indirect
int cpu::ind(bool dummyRead) {
	if(LOG) std::cout << " ind ";
	int x1 = nes->readMemory(pc++);
	int x2 = nes->readMemory(pc++);
	IncCpuCycle();
	IncCpuCycle();
	int addr = (x2*256)+x1;
	if (x1 == 0xff){
		int addr1 = (x2*256);
		instrAddr = (nes->readMemory(addr1)*256)+nes->readMemory(addr);
		IncCpuCycle();
		IncCpuCycle();
	}
	else{
		instrAddr = ((nes->readMemory(addr+1)*256)+nes->readMemory(addr));
		IncCpuCycle();
		IncCpuCycle();
	}
	instrVal = -1;
	return 0;
}

//Indirect X
int cpu::indX(bool dummyRead){
	if(LOG) std::cout << " indX ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	int addr = (x1+x)%256;
	instrAddr = (nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr);
	IncCpuCycle();
	IncCpuCycle();
	IncCpuCycle();
	instrVal = -1;
	return 0;
}

//Indirect Y
int cpu::indY(bool dummyRead){
	if(LOG) std::cout << " indY ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	int addr = x1;
	instrAddr = ((nes->readMemory((addr+1)%256)*256)+nes->readMemory(addr)+y)&0xFFFF;
	IncCpuCycle();
	IncCpuCycle();
	instrVal = -1;
	if (addr+y > 0xFF || dummyRead){
		IncCpuCycle();
		return 1;
	}
	else
		return 0;
}

//Relative
int cpu::rel(bool dummyRead){
	if(LOG) std::cout << " rel ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	if (x1 & 0x80)
		x1 = x1-0x100;
	instrVal = x1;
	return 0;
}

//Zero page
int cpu::zpg(bool dummyRead){
	if(LOG) std::cout << " zpg ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	instrAddr = x1;
	instrVal = nes->readMemory(x1);
	IncCpuCycle();
	return 0;
}

//Zero page X
int cpu::zpgX(bool dummyRead){
	if(LOG) std::cout << " zpgX ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	instrAddr = (x1+x)%256;
	instrVal = nes->readMemory(instrAddr);
	IncCpuCycle();
	IncCpuCycle();
	return 0;
}

//Zero page Y
int cpu::zpgY(bool dummyRead){
	if(LOG) std::cout << " zpgY ";
	int x1 = nes->readMemory(pc++);
	IncCpuCycle();
	instrAddr = (x1+y)%256;
	instrVal = nes->readMemory(instrAddr);
	IncCpuCycle();
	IncCpuCycle();
	return 0;
}
//
//Add with Carry
int cpu::ADC(){
	//Log Instruction
	if(LOG)cout << "ADC" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Add with a register and Carry flag
	int tmp = a + instrVal + (p & P_CARRY);

	//Set overflow flag
	if (a <= 127 && instrVal <= 127 && tmp > 127)
		p |= P_OVERFLOW;
	else if (a > 127 && instrVal > 127 && tmp <= 127)
		p |= P_OVERFLOW;
	else
		p &= ~P_OVERFLOW;

	//Store in A register
	a = tmp;

	//Set flags
	if (tmp >= 256)
		p |= P_CARRY;
	else
		p &= ~P_CARRY;

	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();
	return 1;
}

//Logical AND
int cpu::AND(){
	//Log Instruction
	if(LOG)cout << "AND" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//AND A with value
	a &= (0xFF&instrVal);

	//Set flags
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Arithmetic Shift Left
int cpu::ASL(){
	//Log Instruction
	if(LOG)cout << "ASL" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
 	}
	//Clear Negative and Carry
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry Flag to the MSB of instrVal
	p |= ((instrVal & 0b10000000)>>7);
	//Shift instrVal to the left
	instrVal = (instrVal<<1) & 0b11111110;
	//Store value
	if (instrAddr == -1)
		a = 0xFF&instrVal;
	else{
		nes->writeMemory(instrAddr,instrVal);
		IncCpuCycle();
		IncCpuCycle();
	}

	//Set Flags
	p |= (instrVal & P_NEGATIVE);
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if Carry Clear
int cpu::BCC(){
	//Log instruction
	if(LOG)cout << "BCC" << endl;
	if (!(p & P_CARRY)) {
		//Add extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if Carry Set
int cpu::BCS(){
	//Log Instruction
	if(LOG)cout << "BCS" << endl;
	if ((p & P_CARRY)){
		//Add extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if Equal
int cpu::BEQ(){
	//Log Instruction
	if(LOG)cout << "BEQ" << endl;
	//Return if Zero is clear
	if ((p & 0b00000010)){
		//Add extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}
//
//Bit Test
int cpu::BIT(){
	//Log Instruction
	if(LOG)cout << "BIT" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Clear Flags
	p &= ~(P_NEGATIVE | P_OVERFLOW | P_ZERO);
	//Set Negative and overflow flags
	p |= (instrVal & (P_NEGATIVE | P_OVERFLOW));
	//Set Zero Flag
	if (!(a & instrVal)) p |= P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}
//
//Branch if Minus
int cpu::BMI(){
	//Log Instruction
	if(LOG)cout << "BMI" << endl;
	if ((p & P_NEGATIVE)){
		//Add	extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}
	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if not equal
int cpu::BNE(){
	//Log Instruction
	if(LOG)cout << "BNE" << endl;
	if (!(p & P_ZERO)){
		//Add extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if positive
int cpu::BPL(){
	//Log Instruction
	if(LOG)cout << "BPL" << " " << instrAddr << " " << instrVal << endl;
	if (!(p & P_NEGATIVE)){
		//Add extra cycle if new address crosses page boundry
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}
	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Force Interrupt
int cpu::BRK(){
	//Log Instruction
	if(LOG)cout << "BRK" << endl;

	pc++;


	//Push PC to stack
	nes->writeMemory(0x0100+s, pc/256);
	s--;
	IncCpuCycle();
	nes->writeMemory(0x0100+s, pc%256);
	s--;
	IncCpuCycle();
	//Push P register to stack
	nes->writeMemory(0x0100+s, p);
	s--;
	IncCpuCycle();

	//Disable interupts
	p |= (P_B1 | P_IRQ);

	//Jump to Interupt address
	//NMI interrupt
	if(nmiLine == true && previousNMILine == false)
		pc = nes->readMemory(0xFFFA)+(nes->readMemory(0xFFFB)*256);
	else
		pc = nes->readMemory(0xFFFE)+(nes->readMemory(0xFFFF)*256);

	IncCpuCycle();
	IncCpuCycle();

	return 0;
}

//Branch if Overflow Clear
int cpu::BVC(){
	//Log Instruction
	if(LOG)cout << "BVC" << endl;
	if (!(p & P_OVERFLOW)){
		//Add extra cycle if new address is not on Zero page
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Branch if Overflow Set
int cpu::BVS(){
	//Log Instruction
	if(LOG)cout << "BVS" << endl;
	if ((p & P_OVERFLOW)){
		//Add extra cycle if new address is not on Zero page
		extraCycles++;
		IncCpuCycle();
		if ((pc&0xFF00) != ((pc+instrVal)&0xFF00)){
			extraCycles++;
			IncCpuCycle();
		}
		//Jump to new address
		pc += instrVal;
	}

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Clear Carry flag
int cpu::CLC(){
	//Log Instruction
	if(LOG)cout << "CLC" << endl;
	p &= ~P_CARRY;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Clear Decimal Flag
int cpu::CLD(){
	//Log Instruction
	if(LOG)cout << "CLD" << endl;
	p &= ~P_DECIMAL;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Clear Interrupt Flag
int cpu::CLI(){
	//Log Instruction
	if(LOG)cout << "CLI" << endl;
	// if((p & P_IRQ))irqDelay = true;
	// else irqDelay = false;

	//Poll for interrupts before clearing interrupt flag
	int tmpP = p&~P_IRQ;
	if (PollInterrupts()){
		nes->writeMemory(0x0100+s+1,tmpP);
		return 0;
	}

	p &= ~P_IRQ;

	return 0;
}

//Clear Overflow Flag
int cpu::CLV(){
	//Log Instruction
	if(LOG)cout << "CLV" << endl;
	p &= ~P_OVERFLOW;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Compare
int cpu::CMP(){
	//Log Instruction
	if(LOG)cout << "CMP" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Subtract instrVal from A
	int diff = a - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & P_NEGATIVE);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;
	else p &= ~P_CARRY;

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Compare X Register
int cpu::CPX(){
	//Log Instruction
	if(LOG)cout << "CPX" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Subtract instrVal from A
	int diff = x - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & P_NEGATIVE);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;
	else p &= ~P_CARRY;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Compare Y Register
int cpu::CPY(){
	//Log Instruction
	if(LOG)cout << "CPY" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Subtract instrVal from A
	int diff = y - instrVal;
	//Set Negative flag to MSB of diff
	p &= ~P_NEGATIVE;
	p |= (diff & P_NEGATIVE);
	//Set Flags
	if (diff == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	if (diff >= 0) p |= P_CARRY;
	else p &= ~P_CARRY;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Decrement Memory
int cpu::DEC(){
	//Log Instruction
	if(LOG)cout << "DEC" << endl;
	//Decrement Memory at instrAddr
	if (nes->peekMemory(instrAddr) == 0){
		nes->writeMemory(instrAddr, 0xFF);
	}
	else{
		nes->writeMemory(instrAddr, nes->peekMemory(instrAddr)-1);
	}
	IncCpuCycle();
	IncCpuCycle();

	if (instrVal == -1)
		IncCpuCycle();

	//Set Negative flag to MSB of the memory at instrAddr
	p &= ~P_NEGATIVE;
	p |= (nes->peekMemory(instrAddr) & P_NEGATIVE);
	//Set Zero flag
	if (nes->peekMemory(instrAddr) == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

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
	p |= (x & P_NEGATIVE);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

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
	p |= (y & P_NEGATIVE);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Exclusive OR
int cpu::EOR(){
	//Log Instruction
	if(LOG)cout << "EOR" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Exclusive OR with instrVal
	a ^= (0xFF&instrVal);
	//Set Zero Flags
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A Register
	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Increment Memory
int cpu::INC(){
	//Log Instruction
	if(LOG)cout << "INC" << endl;
	//Increment Memory
	nes->writeMemory(instrAddr, nes->peekMemory(instrAddr)+1);
	//Wrap value if greater then 255
	if (nes->peekMemory(instrAddr) > 0xFF)
		nes->writeMemory(instrAddr, nes->peekMemory(instrAddr) - 0x100);

	IncCpuCycle();
	IncCpuCycle();
	if(instrVal == -1)
		IncCpuCycle();

	//Set Negative flag to MSB of memory value
	p &= ~P_NEGATIVE;
	p |= (nes->peekMemory(instrAddr) & P_NEGATIVE);
	//Set Zero flag
	if (nes->peekMemory(instrAddr) == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

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
	p |= (x & P_NEGATIVE);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

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
	p |= (y & P_NEGATIVE);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Jump
int cpu::JMP(){
	//Log Instruction
	if(LOG)cout << "JMP" << endl;
	//Jump to instrAddr
	pc = instrAddr;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Jump to Subroutine
int cpu::JSR(){
	//Log Instruction
	if(LOG)cout << "JSR" << endl;
	//Push PC to stack
	pc--;
	nes->writeMemory(0x0100+s, pc/0x100);
	IncCpuCycle();
	s--;
	nes->writeMemory(0x0100+s, pc%0x100);
	s--;
	IncCpuCycle();

	IncCpuCycle();
	//Jump to instrAddr
	pc = instrAddr;

	//Poll for interrupts
	PollInterrupts();

	return 0;

}
//
//Load Accumulator
int cpu::LDA(){
	//Log Instruction
	if(LOG)cout << "LDA " << hex << instrAddr << ' ' << instrVal << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Load A with instrVal
	a = 0xFF&instrVal;
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Load X Register
int cpu::LDX(){
	//Log Instruction
	if(LOG)cout << "LDX" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	x = 0xFF&instrVal;
	//Set Negative flag to MSB of X Register
	p &= ~P_NEGATIVE;
	p |= (x & P_NEGATIVE);
	//Set Zero flag
	if (x == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Load Y Register
int cpu::LDY(){
	//Log Instruction
	if(LOG)cout << "LDY" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	y = 0xFF&instrVal;
	//Set Negative flag to MSB of Y Register
	p &= ~P_NEGATIVE;
	p |= (y & P_NEGATIVE);
	//Set Zero flag
	if (y == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 1;
}
//
//Logical Shift Right
int cpu::LSR(){
	//Log Instruction
	if(LOG)cout << "LSR" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Clear Negative and Carry flags
	p &= ~(P_NEGATIVE | P_CARRY);
	//Set Carry flag equal to LSB of instrVal
	p |= (instrVal & 0b0000001);
	//Shift instrVal to the right
	instrVal = (instrVal>>1) & 0b01111111;
	//Store instrVal
	if (instrAddr == -1)
		a = 0xFF&instrVal;
	else{
		nes->writeMemory(instrAddr, instrVal);
		IncCpuCycle();
		IncCpuCycle();
	}

	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//No operation
int cpu::NOP(){
	//Log Instruction
	if(LOG)cout << "NOP" << endl;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Logical Inclusive OR
int cpu::ORA(){
	//Log Instruction
	if(LOG)cout << "ORA" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//OR A with instrVal
	a |= (0xFF&instrVal);
	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Push Accumlator
int cpu::PHA(){
	//Log Instruction
	if(LOG)cout << "PHA" << endl;
	//Push A to stack
	nes->writeMemory(0x0100+s, a);
	s--;
	IncCpuCycle();


	//Poll for interrupts
	PollInterrupts();

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
	IncCpuCycle();

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Pull Accumulator
int cpu::PLA(){
	//Log Instruction
	if(LOG)cout << "PLA" << endl;
	//Pull A from stack
	s++;
	a = nes->readMemory(0x0100+s);
	IncCpuCycle();
	IncCpuCycle();

	//Set Zero Flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Pull processor status
int cpu::PLP(){
	//Log Instruction
	if(LOG)cout << "PLP" << endl;
	//Pull P register from stack
	//int tmpP = p;

	s++;
	int tmpP = nes->readMemory(0x0100+s);
	IncCpuCycle();
	IncCpuCycle();

	//Poll for interrupts before setting p register
	if (PollInterrupts()){
		//set stored p register
		nes->writeMemory(0x0100+s+1,tmpP);
		return 0;
	}

	p = tmpP;

	return 0;
}

//Rotate Left
int cpu::ROL(){
	//Log Instruction
	if(LOG)cout << "ROL" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
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
	if (instrAddr == -1)
		a = 0xFF&instrVal;
	else{
		 nes->writeMemory(instrAddr,instrVal);
		 IncCpuCycle();
		 IncCpuCycle();
	 }


	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
	//Set Negative flag to MSB of instrVal
	p |= (instrVal & 0b10000000);

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Rotate right
int cpu::ROR(){
	//Log Instruction
	if(LOG)cout << "ROR" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
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
	if (instrAddr == -1)
		a = 0xFF&instrVal;
	else{
		nes->writeMemory(instrAddr,instrVal);
		IncCpuCycle();
		IncCpuCycle();
	}


	//Set Negative flag to MSB of instrVal
	p |= (instrVal & 0b10000000);
	//Set Zero flag
	if (instrVal == 0) p |= P_ZERO;
	else p &= ~P_ZERO;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Return from Interrupt
int cpu::RTI(){
	//Log Instruction
	if(LOG)cout << "RTI" << endl;
	//Pull P and PC registers from the stack
	s++;
	p = nes->readMemory(0x0100+s);
	IncCpuCycle();
	s++;
	pc = nes->readMemory(0x0100+s);
	IncCpuCycle();
	s++;
	pc += (nes->readMemory(0x0100+s)*0x100);
	IncCpuCycle();

	IncCpuCycle();

	//Poll for interrupts after pulling p register
	PollInterrupts();

	return 0;
}

//Return from Subroutine
int cpu::RTS(){
	//Log Instruction
	if(LOG)cout << "RTS" << endl;
	//Pull PC from stack
	s++;
	pc = nes->readMemory(0x0100+s);
	IncCpuCycle();
	s++;
	pc += (nes->readMemory(0x0100+s)*0x100);
	IncCpuCycle();

	IncCpuCycle();
	IncCpuCycle();

	pc++;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Subtract with Carry
int cpu::SBC(){
	//Log Instruction
	if(LOG)cout << "SBC" << endl;
	if(instrVal == -1){
		instrVal = nes->readMemory(instrAddr);
		IncCpuCycle();
	}
	//Subtract A,value and the carry flag
	int value = instrVal ^ 0x00FF;
	int tmp = a + value + (p & P_CARRY);
	//Set Overflow flag
	if ((a^tmp)&(value^tmp)&0x80)
		p |= P_OVERFLOW;
	else
		p &= ~P_OVERFLOW;
	a = 0xFF&tmp;
	if (tmp >= 0x100){
		p |= P_CARRY;
	}
	else
		p &= ~P_CARRY;

	//Set Zero flag
	if (a == 0) p |= P_ZERO;
	else p &= ~P_ZERO;
  //Set Negative flag to MSB of A register
	p &= ~P_NEGATIVE;
	p |= (a & P_NEGATIVE);

	//Poll for interrupts
	PollInterrupts();

	return 1;
}

//Set Carry Flag
int cpu::SEC(){
	//Log Instruction
	if(LOG)cout << "SEC" << endl;
	p |= P_CARRY;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Set Decimal flag
int cpu::SED(){
	//Log Instruction
	if(LOG)cout << "SED" << endl;
	p |= P_DECIMAL;

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Set Interrupt Disable
int cpu::SEI(){
	if(LOG)cout << "SEI" << endl;
	//if (irqFlag == false)
	// if(!(p & P_IRQ))irqForce = true;
	// else irqForce = false;

	//Poll for interrupts before setting interrupt flag
	if (PollInterrupts()){
		nes->writeMemory(0x0100+s+1,p|P_IRQ);
		return 0;
	}

	p |= P_IRQ;

	return 0;
}
//
//Store Accumulator
int cpu::STA(){
	//Log Instruction
	if(LOG)cout << "STA " << hex << instrAddr << dec << endl;
	nes->writeMemory(instrAddr, a);
	if (instrVal == -1)
		IncCpuCycle();

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Store X Register
int cpu::STX(){
	//Log Instruction
	if(LOG)cout << "STX" << endl;
	nes->writeMemory(instrAddr, x);
	if (instrVal == -1)
		IncCpuCycle();

	//Poll for interrupts
	PollInterrupts();

	return 0;

}

//Store Y Register
int cpu::STY(){
	//Log Instruction
	if(LOG)cout << "STY" << endl;
	nes->writeMemory(instrAddr, y);
	if (instrVal == -1)
		IncCpuCycle();

	//Poll for interrupts
	PollInterrupts();

	return 0;
}
//
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

	//Poll for interrupts
	PollInterrupts();

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

	//Poll for interrupts
	PollInterrupts();

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

	//Poll for interrupts
	PollInterrupts();

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

	//Poll for interrupts
	PollInterrupts();

	return 0;
}

//Transfer X to Stack Pointer
int cpu::TXS(){
	//Log Instruction
	if(LOG)cout << "TXS" << endl;
	s = x;

	//Poll for interrupts
	PollInterrupts();

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

	//Poll for interrupts
	PollInterrupts();

	return 0;
}
