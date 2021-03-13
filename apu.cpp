#include "apu.h"
#include "NES.h"
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
	if (!pulse1.enabled)
		pulse1.lenCounter = 0;
	pulse2.enabled = data&0x2;
	if (!pulse2.enabled)
		pulse2.lenCounter = 0;
	triangle.enabled = data&0x4;
	if (!triangle.enabled)
		triangle.lengthCounter = 0;
	noise.enabled = data&0x8;
	if (!noise.enabled)
		noise.lenCounter = 0;
	dmc.enabled = data&0x10;
	dmc.irqFlag = false;
	if (!dmc.enabled)
		dmc.bytesRemaining = 0;
	if (dmc.enabled && !dmc.bytesRemaining){
		dmc.currAddr = dmc.sampleAddress;
		dmc.bytesRemaining = dmc.sampleLength;
		
		dmc.bitsRemaining = 8;
		dmc.sampleBuffer = nes->readMemory(dmc.currAddr);
		dmc.sampleEmpty = false;
		if (dmc.currAddr == 0xFFFF)
			dmc.currAddr = 0x8000;
		else
			dmc.currAddr++;
		dmc.bytesRemaining--;
		dmc.shiftBuffer = dmc.sampleBuffer;
		
		if (dmc.bytesRemaining <= 0){
			if (dmc.loopFlag){
				dmc.currAddr = dmc.sampleAddress;
				dmc.bytesRemaining = dmc.sampleLength;
			}
			else{
				dmc.sampleEmpty = true;
				if (dmc.irqEnabled){
					nes->CPU.IRQ();
					dmc.irqFlag = true;
				}
			}
		}
	}
}

void apu::clock(int count){
	int ret;
	int tmpCycles = count;
	while(tmpCycles){
		if (sCount % 6 == 0){
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

			if (dmc.enabled){	
				if (dmc.currTimer <= 0){
					dmc.currTimer = dmc.timer;
					
					//output
					if (!dmc.silenceFlag){
						if (dmc.shiftBuffer&1){
							if (dmc.output<=125)dmc.output += 2;
						}
						else{
							if (dmc.output>=2)dmc.output -= 2;
						}
					}
					else	
						dmc.output = 0;		
					
					if (!dmc.sampleEmpty){
						//8-bit sample buffer
						dmc.silenceFlag = false;
						dmc.shiftBuffer = dmc.shiftBuffer>>1;
						dmc.bitsRemaining--;
						if (dmc.bitsRemaining <= 0){
							//TODO: Stall cpu
							dmc.bitsRemaining = 8;
							dmc.sampleBuffer = nes->readMemory(dmc.currAddr);
							dmc.sampleEmpty = false;
							if (dmc.currAddr == 0xFFFF)
								dmc.currAddr = 0x8000;
							else
								dmc.currAddr++;
							dmc.bytesRemaining--;
							dmc.shiftBuffer = dmc.sampleBuffer;
							
							if (dmc.bytesRemaining <= 0){
								if (dmc.loopFlag){
									dmc.currAddr = dmc.sampleAddress;
									dmc.bytesRemaining = dmc.sampleLength;
								}
								else{
									dmc.sampleEmpty = true;
									dmc.silenceFlag = true;
									if (dmc.irqEnabled){
										nes->CPU.IRQ();
										dmc.irqFlag = true;
									}
								}
							}
						}
					}
				}
				else
					dmc.currTimer -= 2;
			}
			

			cycles++;

			if (!frameCounter.mode){
				if (cycles == 3728){
					quartFrame();
				}
				else if (cycles == 7456){
					halfFrame();

					quartFrame();
				}
				else if (cycles == 11185){
					quartFrame();
				}
				else if (cycles == 14858){
					if (!frameCounter.irqInhibit){
						frameCounter.irqFlag = true;
						nes->CPU.IRQ();
					}
				}
				else if (cycles == 14914){
					quartFrame();
					halfFrame();
					cycles = 0;
				}
			}
			else{
				if (cycles == 3728){
					quartFrame();
				}
				else if (cycles == 7456){
					halfFrame();
		
					quartFrame();
				}
				else if (cycles == 11185){
					quartFrame();
				}
				else if (cycles == 14914){
				}	
				else if (cycles == 18640){
					halfFrame();

					quartFrame();
					cycles = 0;
				}
			}
				
			
			auto pulseUpdate = [this](Pulse *p){
				if (p->enabled){	
					p->output = genSquareWave(p->freq,dutyLookup[p->duty],p->freqCount)*((p->constantVolume)?(p->volume/1.0):(p->eDecayCounter/1.0));
				
					/*if (pulse1.sMute){
						pulse1.output = 0;
					}*/
					
					if (p->timer < 8 || p->lenCounter == 0 || p->sMute){
						p->output = 0;
					}
				}	
				else
					p->output = 0;	
			};
			
			pulseUpdate(&pulse1);
			pulseUpdate(&pulse2);

		if (triangle.enabled){
			if (triangle.lengthCounter && triangle.linearCounter){
				triangle.output = asin(sin(2*3.141592*triangle.freq*triangle.freqCount))*(2*3.141592);
			}
		}
		else
			triangle.output = 0;

	//---------------------------Output------------------------------------
	
			//cout << dmc.output << endl;
		double pulseOut = 0.00752 *(pulse1.output+pulse2.output);
		double triangleOut = 0.00851*triangle.output;
		double noiseOut = 0.00194*noise.output;
		double dmcOut = 0.00335*dmc.output;
		output = (pulseOut + triangleOut + noiseOut + dmcOut)*256;
	//	output = (((0.00752 *0*(pulse1.output+pulse2.output)) 			  +(0.00851*triangle.output+0.00194*0*noise.output + dmc.output*0*0.00335))*256);
			//soundMan->clock();

			pulse1.freqCount +=  2.0 / (1789773) ;
			if (pulse1.freqCount >= 1.0/pulse1.freq)
				pulse1.freqCount = 0;

				pulse2.freqCount +=  2.0 / (1789773) ;
			if (pulse2.freqCount >= 1.0/pulse2.freq)
				pulse2.freqCount = 0;

			triangle.freqCount +=  2.0 / (1789773)  ;
			if (triangle.freqCount >= 1.0/triangle.freq)
				triangle.freqCount = 0;

			incCycles = !incCycles;
				
		}
		sCount++;

		tmpCycles--;
	}
}

void apu::halfFrame(){
	if (pulse1.lenCounter != 0 && !pulse1.envelopeLoop)
		pulse1.lenCounter--;
	if (pulse2.lenCounter != 0 && !pulse2.envelopeLoop)
		pulse2.lenCounter--;
	//cout << pulse1.lenCounter << endl;
	if (triangle.lengthCounter != 0 && !triangle.linControl)
		triangle.lengthCounter--;
	if (noise.lenCounter != 0 && !noise.envelopeLoop)
		noise.lenCounter--;
		
	auto pulseControl = [this](Pulse *p,bool channel1){
		if (p->sDivider == 0 && p->sweepEnable){
			//Update pulse period
			int changeP = ((p->timer>>p->sShift));
			if (p->sNegate)
				changeP = -1*changeP-(channel1);
		
			p->timer += changeP; 
			p->freq = 1789773  / (16 * (double)(p->timer+1));
		}
		if ((p->sDivider == 0 || p->sReload)){
			p->sDivider = (p->sPeriod+1)/2;
			p->sReload = false;
		}
		else
			p->sDivider--;

		p->sMute = (p->timer < 8) || (p->timer > 0x7FF) || (p->lenCounter == 0);
	};
	
	pulseControl(&pulse1,true);
	pulseControl(&pulse2,false);
}

void apu::quartFrame(){
	if (triangle.linReloadFlag)
		triangle.linearCounter = triangle.linReloadVal;
	else if (triangle.linearCounter)
		triangle.linearCounter--;
	if (!triangle.linControl)
		triangle.linReloadFlag = false;
		
	auto pulseEnvelope = [this](Pulse *p){
		if (p->envelopeStart == false){
			//clock
			if (p->eDivider == 0){
				p->eDivider = p->volume;
				if (p->eDecayCounter)
					p->eDecayCounter--;
				else if (p->envelopeLoop)
					p->eDecayCounter = 15;
			}
			else
				p->eDivider--;
		}
		else{
			p->envelopeStart = false;
			p->eDecayCounter = 15;
			p->eDivider = p->volume;
		}				
		
	};

	pulseEnvelope(&pulse1);
	pulseEnvelope(&pulse2);
	
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

uint8_t apu::getSample(){
	return output;
}

void apu::soundClock(){
}

double apu::genSquareWave(double freq, float duty, double count){
	double sin1 = 0;
	double sin2 = 0;
	auto approxsin = [](float t)
	{
		float j = t * 0.15915;
		j = j - (int)j;
		return 20.785 * j * (j - 0.5) * (j - 1.0f);
	};

	for (int i=1;i<=10;i++){
		double sinCache = 2.0*3.141592*i*freq*count;
		sin1 += -approxsin(sinCache)/i;
		sin2 += -approxsin(sinCache-(duty))/i;
	}
		
	//cout << sin1 << endl;
	
	//return (sin(2*3.141592*freq*count));
	double out = (sin1-sin2)/2;
	//if (out > 1) cout << out << endl;
	return out;
}
