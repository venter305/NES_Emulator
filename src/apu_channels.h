#pragma once
#include <cmath>
#include <iostream>


//-----------------------------PULSE-------------------------------------//
class Pulse{
  public:
    bool channelNum = 0;
    bool enabled = false;
    float output = 0;
    int duty = 0;
    bool constantVolume = false;
    int volume = 0;
    int timer = 0;
    int currTimer = 0;
    int lenCounter = 0;
    bool debugEnableOutput = true;
    //sine wave
    float freq = 0;
    float freqCount = 0;
    float dutyLookup[4] = {0.125,0.25,0.50,0.75};
    //sequencer
    uint8_t seqIndex = 128;
    uint8_t sequenceLookup[4] = {0b01000000,0b01100000,0b0111100,0b10011111};
    //envelope
    bool envelopeLoop = false;
    bool envelopeStart = false;
    int eDecayCounter = 0;
    int eDivider = 0;
    //sweep
    bool sweepEnable = false;
    int sDivider = 0;
    int sPeriod = 0;
    bool sNegate = false;
    int sShift = 0;
    bool sReload = false;
    bool sMute = 0;

    void reset(){
      channelNum = 0;
      enabled = false;
      output = 0;
      duty = 0;
      constantVolume = false;
      volume = 0;
      timer = 0;
      currTimer = 0;
      lenCounter = 0;
      debugEnableOutput = true;
      freq = 0;
      freqCount = 0;
      seqIndex = 128;
      envelopeLoop = false;
      envelopeStart = false;
      eDecayCounter = 0;
      eDivider = 0;
      sweepEnable = false;
      sDivider = 0;
      sPeriod = 0;
      sNegate = false;
      sShift = 0;
      sReload = false;
      sMute = 0;
    }


    void SetEnable(bool e_state){
      enabled = e_state;
      if (!enabled)
        lenCounter = 0;
    }

    void CalcuteOutput(){

      //Sequencer
      if(enabled){
        if (currTimer <= 0){
          if (seqIndex > 1)
            seqIndex>>=1;
          else
            seqIndex = 128;
          currTimer = timer;
        }
        else
          currTimer--;

        if (lenCounter && !sMute && timer >= 8)
          output = ((constantVolume)?(volume):(eDecayCounter))*(bool)(sequenceLookup[duty]&seqIndex);
      }
      else
        output = 0;

      //Sine Wave Addition to generate square wave
      // if (enabled && !mute){
      //   if ((timer < 8 || lenCounter == 0 || sMute) && output)
      //     freqCount = 0;
      //   else
      //     output = GenSquareWave(freq,dutyLookup[duty],freqCount)*((constantVolume)?(volume/1.0):(eDecayCounter/1.0));
      // }
      // else
      //   output = 0;
      //
      // freqCount +=  2.0 / (1789773) ;
			// if (freqCount >= 1.0/freq)
			// 	freqCount = 0;

      if (!debugEnableOutput)
        output = 0;
    }

    void UpdateLengthCounter(){
      if (lenCounter > 0 && !envelopeLoop)
        lenCounter--;
    }

    void UpdateSweep(){
      if (sDivider == 0 && sweepEnable){
        //Update pulse period
        int changeP = ((timer>>sShift));
        if (sNegate)
          changeP = -1*changeP-(channelNum);

        if (timer+changeP)
          timer += changeP;

        if (timer > 0x0800)
          timer = 0x0800;
        freq = 1789773  / (16 * (double)(timer+1));
      }
      if ((sDivider == 0 || sReload)){
        sDivider = (sPeriod+1)/2;
        sReload = false;
      }
      else
        sDivider--;

      sMute = (timer < 8) || (timer > 0x7FF);
    }

    void UpdateEnvelope(){
      if (envelopeStart == false){
  			//clock
  			if (eDivider == 0){
  				eDivider = volume;
  				if (eDecayCounter)
  					eDecayCounter--;
  				else if (envelopeLoop)
  					eDecayCounter = 15;
  			}
  			else
  				eDivider--;
  		}
  		else{
  			envelopeStart = false;
  			eDecayCounter = 15;
  			eDivider = volume;
  		}
    }

    double GenSquareWave(double freq, float duty, double count){
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

    	return (sin1-sin2)/2;
    }

    void SaveState(std::ofstream &file){
      file.write(reinterpret_cast<char*>(&enabled),1);
    	file.write(reinterpret_cast<char*>(&output),sizeof(float));
    	file.write(reinterpret_cast<char*>(&duty),sizeof(int));
    	file.write(reinterpret_cast<char*>(&constantVolume),1);
    	file.write(reinterpret_cast<char*>(&volume),sizeof(int));
    	file.write(reinterpret_cast<char*>(&timer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&lenCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.write(reinterpret_cast<char*>(&freq),sizeof(float));
    	file.write(reinterpret_cast<char*>(&freqCount),sizeof(float));
    	file.write(reinterpret_cast<char*>(&seqIndex),1);
    	file.write(reinterpret_cast<char*>(&envelopeLoop),1);
    	file.write(reinterpret_cast<char*>(&envelopeStart),1);
    	file.write(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&eDivider),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sweepEnable),1);
    	file.write(reinterpret_cast<char*>(&sDivider),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sPeriod),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sNegate),1);
    	file.write(reinterpret_cast<char*>(&sShift),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sReload),1);
    	file.write(reinterpret_cast<char*>(&sMute),1);
    }

    void LoadState(std::ifstream &file){
      file.read(reinterpret_cast<char*>(&enabled),1);
    	file.read(reinterpret_cast<char*>(&output),sizeof(float));
    	file.read(reinterpret_cast<char*>(&duty),sizeof(int));
    	file.read(reinterpret_cast<char*>(&constantVolume),1);
    	file.read(reinterpret_cast<char*>(&volume),sizeof(int));
    	file.read(reinterpret_cast<char*>(&timer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&lenCounter),sizeof(int));
    	file.read(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.read(reinterpret_cast<char*>(&freq),sizeof(float));
    	file.read(reinterpret_cast<char*>(&freqCount),sizeof(float));
    	file.read(reinterpret_cast<char*>(&seqIndex),1);
    	file.read(reinterpret_cast<char*>(&envelopeLoop),1);
    	file.read(reinterpret_cast<char*>(&envelopeStart),1);
    	file.read(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
    	file.read(reinterpret_cast<char*>(&eDivider),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sweepEnable),1);
    	file.read(reinterpret_cast<char*>(&sDivider),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sPeriod),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sNegate),1);
    	file.read(reinterpret_cast<char*>(&sShift),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sReload),1);
    	file.read(reinterpret_cast<char*>(&sMute),1);
    }
};

//-----------------------------Triangle-----------------------------------//
class Triangle{
  public:
    bool enabled = false;
    int timer = 0;
    int currTimer = 0;
    float output = 0;
    bool debugEnableOutput = true;
    //linear counter
    bool linControl = false;
    int linReloadVal = 0;
    bool linReloadFlag = false;
    int linearCounter = 0;
    //length counter
    int lengthCounter = 0;
    //sine wave
    float freq = 0;
    float freqCount = 0;
    //sequencer
    int seqIndex = 0;
    int outputLookup[32] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    void reset() {
      enabled = false;
      timer = 0;
      currTimer = 0;
      output = 0;
      debugEnableOutput = true;
      linControl = false;
      linReloadVal = 0;
      linReloadFlag = false;
      linearCounter = 0;
      lengthCounter = 0;
      freq = 0;
      freqCount = 0;
      seqIndex = 0;
    }

    void SetEnable(bool e_state){
      enabled = e_state;
      if (!enabled)
        lengthCounter = 0;
    }

    void CalcuteOutput(){
      //Sequencer
      if (enabled){
        if (currTimer <= 0){
          if (seqIndex >= 31)
          seqIndex = 0;
          else
          seqIndex++;
          currTimer = timer;
        }
        else
          currTimer -= 2;

        if (lengthCounter && linearCounter && timer > 8 || output != 15){
          output = outputLookup[seqIndex];
        }
        else
          seqIndex = 0;
      }
      else
        output = 0;

      //Generate Triangle Wave from Sine wave
    //   if (enabled && !mute){
    //     if (lengthCounter && linearCounter || output != 0){
    //       output = std::asin(std::sin(2*3.141592*freq*freqCount))*(2*3.141592);
    //     }
    //     else{
    //       freqCount = 0;
    //     }
    //   }
    //   else
    //     output = 0;
    //
    //   freqCount +=  2.0 / (1789773) ;
		// 	if (freqCount >= 1.0/freq)
		// 		freqCount = 0;

      if (!debugEnableOutput)
        output = 0;
    }

    void UpdateLengthCounter(){
      if (lengthCounter != 0 && !linControl)
        lengthCounter--;
    }

    void UpdateLinearCounter(){
      if (linReloadFlag)
    		linearCounter = linReloadVal;
    	else if (linearCounter)
    		linearCounter--;
    	if (!linControl)
    		linReloadFlag = false;
    }

    void SaveState(std::ofstream &file){
      file.write(reinterpret_cast<char*>(&enabled),1);
    	file.write(reinterpret_cast<char*>(&enabled),sizeof(int));
    	file.write(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&output),sizeof(float));
    	file.write(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.write(reinterpret_cast<char*>(&linControl),1);
    	file.write(reinterpret_cast<char*>(&linReloadVal),sizeof(int));
    	file.write(reinterpret_cast<char*>(&linReloadFlag),1);
    	file.write(reinterpret_cast<char*>(&linearCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&lengthCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&freq),sizeof(float));
    	file.write(reinterpret_cast<char*>(&freqCount),sizeof(float));
    	file.write(reinterpret_cast<char*>(&seqIndex),sizeof(int));
    }

    void LoadState(std::ifstream &file){
      file.read(reinterpret_cast<char*>(&enabled),1);
    	file.read(reinterpret_cast<char*>(&enabled),sizeof(int));
    	file.read(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&output),sizeof(float));
    	file.read(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.read(reinterpret_cast<char*>(&linControl),1);
    	file.read(reinterpret_cast<char*>(&linReloadVal),sizeof(int));
    	file.read(reinterpret_cast<char*>(&linReloadFlag),1);
    	file.read(reinterpret_cast<char*>(&linearCounter),sizeof(int));
    	file.read(reinterpret_cast<char*>(&lengthCounter),sizeof(int));
    	file.read(reinterpret_cast<char*>(&freq),sizeof(float));
    	file.read(reinterpret_cast<char*>(&freqCount),sizeof(float));
    	file.read(reinterpret_cast<char*>(&seqIndex),sizeof(int));
    }
};

//-----------------------------Noise-------------------------------------//
class Noise{
  public:
    bool enabled = false;
    bool debugEnableOutput = true;
    bool constantVolume = false;
    int volume = 0;
    bool loop = false;
    int period = 0;
    int lenCounter = 0;
    float output = 0;
    int shiftReg = 1;
    int currTimer = 0;
    bool mode = false;
    int periodLookup[16] = {4,8,16,32,64,96,128,160,202,254,380,508,762,1016,2034,4068};
    //envelope
    bool envelopeLoop = false;
    bool envelopeStart = false;
    int eDecayCounter = 0;
    int eDivider = 0;

    void reset(){
      enabled = false;
      debugEnableOutput = true;
      constantVolume = false;
      volume = 0;
      loop = false;
      period = 0;
      lenCounter = 0;
      output = 0;
      shiftReg = 1;
      currTimer = 0;
      mode = false;
      envelopeLoop = false;
      envelopeStart = false;
      eDecayCounter = 0;
      eDivider = 0;
    }

    void SetEnable(bool e_state){
      enabled = e_state;
      if (!enabled)
        lenCounter = 0;
    }

    void CalculateOutput(){
      if (enabled){
				if (currTimer <= 8){
					currTimer = (periodLookup[period]/2)-1;
					if (mode)
						output = (shiftReg&1)^((shiftReg&0x40)>0);
					else
						output = (shiftReg&1)^((shiftReg&0x2)>0);
					shiftReg = shiftReg>>1;
					shiftReg = shiftReg&0x3FFF;
					shiftReg = shiftReg + (output*0x4000);
					if (shiftReg&1 || lenCounter == 0)
						output = 0;
					else
						output = (constantVolume)?volume:eDecayCounter;
				}
				else
					currTimer--;
			}
			else
				output = 0;

      if (!debugEnableOutput)
        output = 0;
    }

    void UpdateLengthCounter(){
      if (lenCounter != 0 && !envelopeLoop)
        lenCounter--;
    }

    void UpdateEnvelope(){
      if (envelopeStart == false){
        //clock
        if (eDivider == 0){
          eDivider = volume;
          if (eDecayCounter)
            eDecayCounter--;
          else if (envelopeLoop)
            eDecayCounter = 15;
        }
        else
          eDivider--;
      }
      else{
        envelopeStart = false;
        eDecayCounter = 15;
        eDivider = volume;
      }
    }

    void SaveState(std::ofstream &file){
      file.write(reinterpret_cast<char*>(&enabled),1);
    	file.write(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.write(reinterpret_cast<char*>(&constantVolume),1);
    	file.write(reinterpret_cast<char*>(&volume),sizeof(int));
    	file.write(reinterpret_cast<char*>(&loop),1);
    	file.write(reinterpret_cast<char*>(&period),sizeof(int));
    	file.write(reinterpret_cast<char*>(&lenCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&output),sizeof(float));
    	file.write(reinterpret_cast<char*>(&shiftReg),sizeof(int));
    	file.write(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&mode),1);
    	file.write(reinterpret_cast<char*>(&envelopeLoop),1);
    	file.write(reinterpret_cast<char*>(&envelopeStart),1);
    	file.write(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
    	file.write(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
    }

    void LoadState(std::ifstream &file){
      file.read(reinterpret_cast<char*>(&enabled),1);
      file.read(reinterpret_cast<char*>(&debugEnableOutput),1);
      file.read(reinterpret_cast<char*>(&constantVolume),1);
      file.read(reinterpret_cast<char*>(&volume),sizeof(int));
      file.read(reinterpret_cast<char*>(&loop),1);
      file.read(reinterpret_cast<char*>(&period),sizeof(int));
      file.read(reinterpret_cast<char*>(&lenCounter),sizeof(int));
      file.read(reinterpret_cast<char*>(&output),sizeof(float));
      file.read(reinterpret_cast<char*>(&shiftReg),sizeof(int));
      file.read(reinterpret_cast<char*>(&currTimer),sizeof(int));
      file.read(reinterpret_cast<char*>(&mode),1);
      file.read(reinterpret_cast<char*>(&envelopeLoop),1);
      file.read(reinterpret_cast<char*>(&envelopeStart),1);
      file.read(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
      file.read(reinterpret_cast<char*>(&eDecayCounter),sizeof(int));
    }
};

//-----------------------------DMC-------------------------------------//
class DMC{
  public:
    bool enabled = 0;
    bool debugEnableOutput = true;
    int timer = 0;
    int currTimer = 0;
    int output = 0;
    bool loopFlag = false;
    int currAddr = 0;
    int bytesRemaining = 0;
    int bitsRemaining = 0;
    bool silenceFlag = false;
    //sample
    int sampleBuffer = 0;
    int sampleAddress = 0;
    int sampleLength = 0;
    bool sampleEmpty = true;
    //interrupt
    bool irqFlag = false;
    bool irqEnabled = false;
    //shift register
    int shiftBuffer = 0;

    void reset(){
      enabled = 0;
      debugEnableOutput = true;
      timer = 0;
      currTimer = 0;
      output = 0;
      loopFlag = false;
      currAddr = 0;
      bytesRemaining = 0;
      bitsRemaining = 0;
      silenceFlag = false;
      sampleBuffer = 0;
      sampleAddress = 0;
      sampleLength = 0;
      sampleEmpty = false;
      irqFlag = false;
      irqEnabled = false;
      shiftBuffer = 0;
    }

    void SetEnable(bool e_state){
      enabled = e_state;
      irqFlag = false;
      if (!enabled)
        bytesRemaining = 0;
    }

    bool RestartSample(int sample){
      currAddr = sampleAddress;
      bytesRemaining = sampleLength;

      bitsRemaining = 8;
      sampleBuffer = sample;
      sampleEmpty = false;
      if (currAddr == 0xFFFF)
        currAddr = 0x8000;
      else
        currAddr++;
      bytesRemaining--;
      shiftBuffer = sampleBuffer;

      if (bytesRemaining <= 0){
        if (loopFlag){
          currAddr = sampleAddress;
          bytesRemaining = sampleLength;
        }
        else{
          sampleEmpty = true;
          if (irqEnabled){
            irqFlag = true;
            return true;
          }
        }
      }

      return false;
    }

    bool CalculateOutput(int sample){
      if (enabled){
				if (currTimer <= 0){
					currTimer = timer;

					//output
					if (!silenceFlag){
						if (shiftBuffer&1){
							if (output<=125)output += 2;
						}
						else{
							if (output>=2)output -= 2;
						}
					}
					else
						output = 0;

					if (!sampleEmpty){
						//8-bit sample buffer
						silenceFlag = false;
						shiftBuffer = shiftBuffer>>1;
						bitsRemaining--;
						if (bitsRemaining <= 0){
							//TODO: Stall cpu
							bitsRemaining = 8;
							sampleBuffer = sample;
							sampleEmpty = false;
							if (currAddr == 0xFFFF)
								currAddr = 0x8000;
							else
								currAddr++;
							bytesRemaining--;
							shiftBuffer = sampleBuffer;

							if (bytesRemaining <= 0){
								if (loopFlag){
									currAddr = sampleAddress;
									bytesRemaining = sampleLength;
								}
								else{
									sampleEmpty = true;
									silenceFlag = true;
									if (irqEnabled){
										irqFlag = true;
									}
								}
							}
						}
					}
				}
				else
					currTimer -= 2;
			}

      if (!debugEnableOutput)
        output = 0;

      if (irqFlag)
        return true;

      return false;
    }

    void SaveState(std::ofstream &file){
      file.write(reinterpret_cast<char*>(&enabled),1);
    	file.write(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.write(reinterpret_cast<char*>(&timer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&output),sizeof(int));
    	file.write(reinterpret_cast<char*>(&loopFlag),1);
    	file.write(reinterpret_cast<char*>(&currAddr),sizeof(int));
    	file.write(reinterpret_cast<char*>(&bytesRemaining),sizeof(int));
    	file.write(reinterpret_cast<char*>(&bitsRemaining),sizeof(int));
    	file.write(reinterpret_cast<char*>(&silenceFlag),1);
    	file.write(reinterpret_cast<char*>(&sampleBuffer),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sampleAddress),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sampleLength),sizeof(int));
    	file.write(reinterpret_cast<char*>(&sampleEmpty),1);
    	file.write(reinterpret_cast<char*>(&irqFlag),1);
    	file.write(reinterpret_cast<char*>(&irqEnabled),1);
    	file.write(reinterpret_cast<char*>(&shiftBuffer),sizeof(int));
    }

    void LoadState(std::ifstream &file){
      file.read(reinterpret_cast<char*>(&enabled),1);
    	file.read(reinterpret_cast<char*>(&debugEnableOutput),1);
    	file.read(reinterpret_cast<char*>(&timer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&currTimer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&output),sizeof(int));
    	file.read(reinterpret_cast<char*>(&loopFlag),1);
    	file.read(reinterpret_cast<char*>(&currAddr),sizeof(int));
    	file.read(reinterpret_cast<char*>(&bytesRemaining),sizeof(int));
    	file.read(reinterpret_cast<char*>(&bitsRemaining),sizeof(int));
    	file.read(reinterpret_cast<char*>(&silenceFlag),1);
    	file.read(reinterpret_cast<char*>(&sampleBuffer),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sampleAddress),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sampleLength),sizeof(int));
    	file.read(reinterpret_cast<char*>(&sampleEmpty),1);
    	file.read(reinterpret_cast<char*>(&irqFlag),1);
    	file.read(reinterpret_cast<char*>(&irqEnabled),1);
    	file.read(reinterpret_cast<char*>(&shiftBuffer),sizeof(int));
    }
};
