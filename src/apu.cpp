#include "apu.h"
#include "NES.h"
#include <iostream>
#include <cmath>

apu::apu(){
	pulse1.channelNum = 0;
	pulse2.channelNum = 1;
}

apu::~apu(){
}

void apu::reset(){
		setChannelEnables(0);
		pulse1.reset();
		pulse2.reset();
		triangle.reset();
		noise.reset();
		dmc.reset();
		frameCounter.mode = false;
		frameCounter.irqFlag = false;
		frameCounter.irqInhibit = false;
		frameCounter.count = 0;

		cycles = 0;
		output = 0;
}

void apu::setChannelEnables(int data){
	pulse1.SetEnable(data&0x1);
	pulse2.SetEnable(data&0x2);
	triangle.SetEnable(data&0x4);
	noise.SetEnable(data&0x8);
	dmc.SetEnable(data&0x10);

	if (dmc.enabled && !dmc.bytesRemaining){
		if(dmc.RestartSample(nes->readMemory(dmc.sampleAddress)))
			nes->CPU.irqFlag = true;
	}
}

void apu::clock(int count){
	int ret;
	int tmpCycles = count;
	while(tmpCycles){
		if (cycles % 2 == 0){
			frameCounter.count++;

			if (!frameCounter.mode){
				if (frameCounter.count == 0){
				}
				else if (frameCounter.count == 3728){
					quartFrame();
				}
				else if (frameCounter.count == 7456){
					halfFrame();

					quartFrame();
				}
				else if (frameCounter.count == 11185){
					quartFrame();
				}
				else if (frameCounter.count >= 14914){
					quartFrame();
					halfFrame();
					frameCounter.count = 0;
					if (!frameCounter.irqInhibit){
						frameCounter.irqFlag = true;
					}
				}
			}
			else{
				if (frameCounter.count == 3728){
					quartFrame();
				}
				else if (frameCounter.count == 7456){
					halfFrame();

					quartFrame();
				}
				else if (frameCounter.count == 11185){
					quartFrame();
				}
				else if (frameCounter.count == 14914){
				}
				else if (frameCounter.count >= 18640){
					halfFrame();

					quartFrame();
					frameCounter.count = 0;
				}
			}

			pulse1.CalcuteOutput();
			pulse2.CalcuteOutput();
			triangle.CalcuteOutput();
			noise.CalculateOutput();
			if (dmc.CalculateOutput(nes->readMemory(dmc.currAddr)) || frameCounter.irqFlag)
				nes->CPU.irqFlag = true;
	//---------------------------Mixer------------------------------------//

			//Linear
			double pulseOut = 0.00552 *(pulse1.output+pulse2.output);
			double triangleOut = 0.00751*triangle.output;
			double noiseOut = 0.00294*noise.output;
			double dmcOut = 0.00135*dmc.output;
			output = (pulseOut + triangleOut + noiseOut + dmcOut)*0xFF;
		}
		cycles++;

		tmpCycles--;
	}
}

void apu::halfFrame(){
	pulse1.UpdateLengthCounter();
	pulse2.UpdateLengthCounter();
	triangle.UpdateLengthCounter();
	noise.UpdateLengthCounter();

	pulse1.UpdateSweep();
	pulse2.UpdateSweep();
}

void apu::quartFrame(){
	triangle.UpdateLinearCounter();

	pulse1.UpdateEnvelope();
	pulse2.UpdateEnvelope();
	noise.UpdateEnvelope();
}

void apu::SaveState(std::ofstream &file){
	file.write(reinterpret_cast<char*>(&cycles),sizeof(int));
	file.write(reinterpret_cast<char*>(&output),2);
	file.write(reinterpret_cast<char*>(&frameCounter.mode),1);
	file.write(reinterpret_cast<char*>(&frameCounter.irqFlag),1);
	file.write(reinterpret_cast<char*>(&frameCounter.irqInhibit),1);
	file.write(reinterpret_cast<char*>(&frameCounter.count),sizeof(int));

	pulse1.SaveState(file);
	pulse2.SaveState(file);
	triangle.SaveState(file);
	noise.SaveState(file);
	dmc.SaveState(file);
}

void apu::LoadState(std::ifstream &file){
	file.read(reinterpret_cast<char*>(&cycles),sizeof(int));
	file.read(reinterpret_cast<char*>(&output),2);
	file.read(reinterpret_cast<char*>(&frameCounter.mode),1);
	file.read(reinterpret_cast<char*>(&frameCounter.irqFlag),1);
	file.read(reinterpret_cast<char*>(&frameCounter.irqInhibit),1);
	file.read(reinterpret_cast<char*>(&frameCounter.count),sizeof(int));

	pulse1.LoadState(file);
	pulse2.LoadState(file);
	triangle.LoadState(file);
	noise.LoadState(file);
	dmc.LoadState(file);
}
