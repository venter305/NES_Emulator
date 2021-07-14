#pragma once

#include <fstream>
#include "apu_channels.h"

class NES;

class apu{
	public:

		uint8_t lenCounterLookup[0x20] = {10,254,20,2,40,4,80,6,160,8,60,10,14,12,26,14,12,16,24,18,48,20,96,22,192,24,72,26,16,28,32,30};

	 	Pulse pulse1,pulse2;
		Triangle triangle;
		Noise noise;
		int dmcRateLookup[16] = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54};
		DMC dmc;

		struct fCounter{
			bool mode = false;
			bool irqFlag = false;
			bool irqInhibit = false;
			int count = 0;
		}frameCounter;

		NES *nes;

		unsigned int cycles;

		uint16_t output;


		apu();
		~apu();

		void setChannelEnables(int);
		void clock(int);
		void reset();
		void halfFrame();
		void quartFrame();

		void SaveState(std::ofstream &file);
		void LoadState(std::ifstream &file);
};
