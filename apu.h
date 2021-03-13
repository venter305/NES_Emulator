#ifndef APU_H_
#define APU_H_

#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <queue>
#include "SoundManager/soundManager.h"

class NES;

class apu{
	public:

	//uint8_t dutyLookup[4] = {0b01000000,0b01100000,0b01111000,0b10011111};
	float dutyLookup[4] = {0.125,0.25,0.50,0.75};

	uint8_t lenCounterLookup[0x20] = {10,254,20,2,40,4,80,6,160,8,60,10,14,12,26,14,12,16,24,18,48,20,96,22,192,24,72,26,16,28,32,30};
	struct Pulse{
		bool enabled = false;
		int duty = 0;
		bool envelopeLoop = false;
		bool constantVolume = false;
		bool envelopeStart = false;
		int eDecayCounter = 0;
		int eDivider = 0;
		int volume = 0;
		bool sweepEnable = false;
		int sDivider = 0;
		int sPeriod = 0;
		bool sNegate = false;
		int sShift = 0;
		bool sReload = false;
		int sMute = 0;
		int timer = 0;
		int currTimer = 0;
		int lenCounter = 0;
		double output = 0;
		int seqMask = 1;
		int seqOffset = 1;
		double freq = 0;
		double freqCount = 0;
	} pulse1,pulse2;

	struct Triangle{
		bool enabled = false;
		bool linControl = false;
		int linReloadVal = 0;
		bool linReloadFlag = false;
		int linearCounter = 0;
		int timer = 0;
		int currTimer = 0;
		int lengthCounter = 0;
		double output = 0;
		double freq = 0;
		double freqCount = 0;
		int seqIndex = 0;
		int outputLookup[32] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	}triangle;

	struct Noise{
		bool enabled = false;
		bool envelopeLoop = false;
		bool constantVolume = false;
		bool envelopeStart = false;
		int eDecayCounter = 0;
		int eDivider = 0;
		int volume = 0;
		bool loop = false;
		int period = 0;
		int lenCounter = 0;
		double output = 0;
		int shiftReg = 1;
		int currTimer = 0;
		bool mode = false;
		int periodLookup[16] = {4,8,16,32,64,96,128,160,202,254,380,508,762,1016,2034,4068};
	}noise;

	int dmcRateLookup[16] = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54};
	struct DMC{
		bool enabled = 0;
		int sampleBuffer = 0;
		int shiftBuffer = 0;
		int timer = 0;
		int currTimer = 0;
		int sampleAddress = 0;
		int sampleLength = 0;
		bool irqFlag = false;
		bool irqEnabled = false;
		int output = 0;
		bool loopFlag = false;
		int bitsRemaining = 0;
		int bytesRemaining = 0;
		int currAddr = 0;
		bool silenceFlag = false;
		bool sampleEmpty = true;
	}dmc;

	struct fCounter{
		bool mode = false;
		bool irqFlag = false;
		bool irqInhibit = false;
		int count = 0;
	}frameCounter;

	NES *nes;
	SoundManager *soundMan;
	int cycles;
	bool incCycles = false;
	int sCount = 0;

	/*static int bufSize;
	//static std::queue<uint8_t> *soundBuf;
	static uint8_t *soundBuf;
	int sBufIndex = 0;
	int clockIndex = 0;
//	p	a_simple *s = NULL;*/

	/*pa_threaded_mainloop *s = NULL;
	pa_context *context = NULL;
	pa_mainloop_api *mainloop_api = NULL;
	static pa_stream *stream;*/
	
	int max = 124;
	int count1 = 124;
	int duty = 0b00001111;
	int j = 0;
	int mask = 1;
	
	uint8_t output;
	
	
	apu();
	~apu();
	
	void setChannelEnables(int);
	void clock(int);
	void soundClock();
	uint8_t getSample();
	void reset();
	double genSquareWave(double,float,double);
	void halfFrame();
	void quartFrame();
};

#endif
