#include "apu.h"
#include <iostream>
#include <cmath>

using namespace std;

apu::apu(){
}

apu::~apu(){
}

void apu::reset(){
		setChannelEnables(0);
}

void apu::setChannelEnables(int data){	
	pulse1.enabled = data&0x1;
	pulse2.enabled = data&0x2;
	triangle.enabled = data&0x4;
	noise.enabled = data&0x8;
}

void apu::clock(int count){
	int ret;
	int tmpCycles = count;
	while(tmpCycles){
		if (noise.enabled){
				if (noise.currTimer <= 8){
					noise.currTimer = (noise.periodLookup[noise.period]/2)-1;
					if (noise.mode)
						noise.output = (noise.shiftReg&1)^((noise.shiftReg&0x40)>0);
					else
						noise.output = (noise.shiftReg&1)^((noise.shiftReg&0x2)>0);
					noise.shiftReg = noise.shiftReg>>1;
					noise.shiftReg = noise.shiftReg&0x3FFF;
					noise.shiftReg = noise.shiftReg + (noise.output*0x4000);
					if (noise.shiftReg&1 || noise.lenCounter == 0)
						noise.output = 0;
					else
						noise.output = (noise.constantVolume)?noise.volume:noise.eDecayCounter;
				}	
				else
					noise.currTimer--;
			}
			else
				noise.output = 0;

		if (incCycles){
			cycles++;

			bool halfFrame = false;
			bool quartFrame = false;
			if (!frameCounter.mode){
				if (cycles == 3728){
					quartFrame = true;
				}
				else if (cycles == 7456){
					halfFrame = true;

					quartFrame = true;
				}
				else if (cycles == 11185){
					quartFrame = true;
				}
				else if (cycles == 14914){
					halfFrame = true;

					quartFrame = true;
					cycles = 0;
				}	
			}
			else{
				if (cycles == 3728){
					quartFrame = true;
				}
				else if (cycles == 7456){
					halfFrame = true;
		
					quartFrame = true;
				}
				else if (cycles == 11185){
					quartFrame = true;
				}
				else if (cycles == 14914){
				}	
				else if (cycles == 18640){
					halfFrame = true;

					quartFrame = true;
					cycles = 0;
				}
			}

			if (halfFrame){
				if (pulse1.lenCounter != 0 && !pulse1.envelopeLoop)
						pulse1.lenCounter--;
					if (pulse1.sDivider == 0 && pulse1.sweepEnable){
						//Update pulse period
						int changeP = ((pulse1.timer>>pulse1.sShift));
						if (pulse1.sNegate)
							changeP = -1*changeP-1;
					
						pulse1.timer += changeP; 
						pulse1.freq = 1789773  / (16 * (double)(pulse1.timer+1));
					}
					if ((pulse1.sDivider == 0 || pulse1.sReload)){
						pulse1.sDivider = (pulse1.sPeriod+1)/2;
						pulse1.sReload = false;
					}
					else
						pulse1.sDivider--;

					pulse1.sMute = (pulse1.timer < 8) || (pulse1.timer > 0x7FF);

					if (pulse2.sDivider == 0 && pulse2.sweepEnable){
						//Update pulse period
						int changeP = ((pulse2.timer>>pulse2.sShift));	
						if (pulse2.sNegate)
							changeP = -1*changeP;
						
						pulse2.timer += changeP; 
						pulse2.freq = 1789773  / (16 * (double)(pulse2.timer+1));
					}
					if (pulse2.sDivider == 0 || pulse2.sReload){
						pulse2.sDivider = pulse2.sPeriod+1;
						pulse2.sReload = false;
					}
					else
						pulse2.sDivider--;

					pulse2.sMute = (pulse2.timer < 8) || (pulse2.timer > 0x7FF);

					if (pulse2.lenCounter != 0 && !pulse2.envelopeLoop)
						pulse2.lenCounter--;
					if (triangle.lengthCounter != 0 && !triangle.linControl)
						triangle.lengthCounter--;
					if (noise.lenCounter != 0 && !noise.envelopeLoop)
						noise.lenCounter--;
			}
			if (quartFrame){
				if (triangle.linReloadFlag)
					triangle.linearCounter = triangle.linReloadVal;
				else if (triangle.linearCounter)
					triangle.linearCounter--;
				if (!triangle.linControl)
					triangle.linReloadFlag = false;
		
				if (pulse1.envelopeStart == false){
					//clock
					if (pulse1.eDivider == 0){
						pulse1.eDivider = pulse1.volume;
						if (pulse1.eDecayCounter)
							pulse1.eDecayCounter--;
						else if (pulse1.envelopeLoop)
							pulse1.eDecayCounter = 15;
					}
					else
						pulse1.eDivider--;
				}
				else{
					pulse1.envelopeStart = false;
					pulse1.eDecayCounter = 15;
					pulse1.eDivider = pulse1.volume;
				}				
				if (pulse2.envelopeStart == false){
					//clock
					if (pulse2.eDivider == 0){
						pulse2.eDivider = pulse2.volume;
						if (pulse2.eDecayCounter)
							pulse2.eDecayCounter--;
						else if (pulse2.envelopeLoop)
							pulse2.eDecayCounter = 15;
					}
					else
						pulse2.eDivider--;
				}
				else{
					pulse2.envelopeStart = false;
					pulse2.eDecayCounter = 15;
					pulse2.eDivider = pulse2.volume;
				}
				if (noise.envelopeStart == false){
					//clock
					if (noise.eDivider == 0){
						noise.eDivider = noise.volume;
						if (noise.eDecayCounter)
							noise.eDecayCounter--;
						else if (noise.envelopeLoop)
							noise.eDecayCounter = 15;
					}
					else
						noise.eDivider--;
				}
				else{
					noise.envelopeStart = false;
					noise.eDecayCounter = 15;
					noise.eDivider = noise.volume;
				}								
			}

			if (pulse1.enabled){	
				pulse1.output = genSquareWave(pulse1.freq,dutyLookup[pulse1.duty],pulse1.freqCount)*((pulse1.constantVolume)?pulse1.volume:pulse1.eDecayCounter);
				pulse1.freqCount++;
			
				if (pulse1.timer < 8 || pulse1.lenCounter == 0 || pulse1.sMute){
					pulse1.output = 0;
				}
			}	
			else
				pulse1.output = 0;	
	
			if (pulse2.enabled){
				
				pulse2.output = genSquareWave(pulse2.freq,dutyLookup[pulse2.duty],pulse2.freqCount)*((pulse2.constantVolume)?pulse2.volume:pulse2.eDecayCounter);
				//pulse2.freqCount++;
				//cout << pulse1.output << endl;

				if (pulse2.timer < 8 || pulse2.lenCounter == 0 || pulse2.sMute){
					pulse2.output = 0;
				}
			}
			else
				pulse2.output = 0;		

		}

		if (triangle.enabled){
			if (triangle.lengthCounter && triangle.linearCounter){
				triangle.output = asin(sin(2*3.141592*triangle.freq*(double)sCount/(48000*4)))*(2*3.141592);
				triangle.freqCount++;
			}
		}
		else
			triangle.output = 0;

		if (soundMan->samplesNeeded){
			soundMan->output = ((0.00752*(pulse1.output+pulse2.output)) 			  +(0.00851*triangle.output+0.00194*noise.output))*500;
			//soundMan->output = pulse1.output*5;
				
			soundMan->clock();

			sCount++;
		}

		incCycles = !incCycles;
		tmpCycles--;
	}
}

void apu::playSound(){
	
}

void apu::soundClock(){
}

double apu::genSquareWave(double freq, float duty, int count){
	double sin1 = 0;
	double sin2 = 0;
	auto approxsin = [](float t)
	{
		float j = t * 0.15915;
		j = j - (int)j;
		return 20.785 * j * (j - 0.5) * (j - 1.0f);
	};

	/*for (int i=1;i<=1;i++){
		double sinCache = 2*3.141592*i;
		sin1 += -approxsin(sinCache*freq*(double)sCount/(48000*4))/i;
		sin2 += -approxsin(sinCache*(freq*((double)sCount/(48000*4))-duty))/i;
	}*/
	
	return (sin(2*3.141592*freq*(double)sCount/(48000*4)));
	//return (2*1/3.141592)*(sin1-sin2);
}
