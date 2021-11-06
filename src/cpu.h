#pragma once

#include <vector>
#include <fstream>

class NES;

class cpu{
	struct instruction {
		int (cpu::*opcode)();
		int (cpu::*addrMode)(bool);
		int cycles;
		bool dummyRead = false;
	};


	enum {
		P_CARRY =  0b00000001,
		P_ZERO =   0b00000010,
		P_IRQ =    0b00000100,
		P_DECIMAL= 0b00001000,
		P_B1 =     0b00010000,
		P_B2 =     0b00100000,
		P_OVERFLOW=0b01000000,
		P_NEGATIVE=0b10000000
	};

	public:
		//Registors
		uint8_t a,x,y,s,p;
		uint16_t pc;

		NES *nes;
		int cycles;
		unsigned int cycleCount;
		unsigned int currentInstrCycle;
		int instr;
		int extraCycles;
		std::vector<instruction> opcodes;

		cpu();

		int instrAddr;
		int instrVal;
		bool irqFlag;
		//bool irqDelay;
		//bool irqForce;
		//bool nmiFlag;
		bool nmiLine;
		bool previousNMILine;

		void IncCpuCycle();
		int runInstructions();
		bool PollInterrupts();

		void SaveState(std::ofstream &file);
		void LoadState(std::ifstream &file);
		//Address modes
		int acc(bool dummyRead = false);
		int abs(bool dummyRead = false);
		int absX(bool dummyRead = false);
		int absY(bool dummyRead = false);
		int imm(bool dummyRead = false);
		int impl(bool dummyRead = false);
		int ind(bool dummyRead = false);
		int indX(bool dummyRead = false);
		int indY(bool dummyRead = false);
		int rel(bool dummyRead = false);
		int zpg(bool dummyRead = false);
		int zpgX(bool dummyRead = false);
		int zpgY(bool dummyRead = false);

		//Instructions
		bool IRQ();
		void NMI();
		void reset();
		int ADC();
		int AND();
		int ASL();
		int BCC();
		int BCS();
		int BEQ();
		int BIT();
		int BMI();
		int BNE();
		int BPL();
		int BRK();
		int BVC();
		int BVS();
		int CLC();
		int CLD();
		int CLI();
		int CLV();
		int CMP();
		int CPX();
		int CPY();
		int DEC();
		int DEX();
		int DEY();
		int EOR();
		int INC();
		int INX();
		int INY();
		int JMP();
		int JSR();
		int LDA();
		int LDX();
		int LDY();
		int LSR();
		int NOP();
		int ORA();
		int PHA();
		int PHP();
		int PLA();
		int PLP();
		int ROL();
		int ROR();
		int RTI();
		int RTS();
		int SBC();
		int SEC();
		int SED();
		int SEI();
		int STA();
		int STX();
		int STY();
		int TAX();
		int TAY();
		int TSX();
		int TXA();
		int TXS();
		int TYA();

};
