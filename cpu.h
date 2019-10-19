#ifndef CPU_H_
#define CPU_H_

#include <vector>

class NES;

class cpu{
	struct instruction {
		int (cpu::*opcode)();
		int (cpu::*addrMode)();
		int cycles;
	};
	public:
		//Registors
		int a,x,y,s,p;
		int pc;
		//memory
		//int *memory;
		//std::vector<int> cart;
		//ppu *PPU;
		//controller *CONTRL;
		NES *nes;
		int cycles;
		bool vBlank;
		std::vector<instruction> opcodes;
		cpu();
		
		int instrAddr;
		int instrVal;
		int instrIndex;
		int tmpPC;
		int instrMode;
		int *ppuReg2000;
		int *ppuReg2002;
		

		void runInstructions();
		int acc();
		int abs();
		int absX();
		int absY();
		int imm();
		int impl();
		int ind();
		int indX();
		int indY();
		int rel();
		int zpg();
		int zpgX();
		int zpgY();

		//int getCycles(int,int,int);
		void IRQ();
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

#endif
