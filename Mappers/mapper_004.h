#pragma once
#include "mapper.h"

//MMC3

/*ROM banks:
  8KB+8KB+16KB fixed
  $8000-$9FFF : swapable or fixed to second to last bank
  $A000-$BFFF : swapable
  $C000-$DFFF : swapable of fixed to second to last bank
  $E000-$FFFF : fixed to last bank
  prgBankMode:
    0: $8000-$9FFF switchable
       $C000-$DFFF Fixed to second to last bank
    1: $C000-$DFFF switchable
       $8000-$9FFF Fixed to second to last bank

CHR banks
  2KB+2KB+1KB+1KB+1KB+1KB
  chrBankMode:
    0:
      $0000-$0FFF 2 2KB banks
      $1000-$1FFF 4 1KB banks
    1:
      $1000-$1FFF 2 2KB banks
      $0000-$0FFF 4 1KB banks
*/
class Mapper_004: public Mapper{
public:
  int currentBank = 0;

  int nameTableMirroring = 0;

  bool ramWriteProtection = false;
  bool enableRAM = true;

  int previousCHRRead = 0;

  bool irqEnable = false;
  bool irqPending = false;
  int irqReloadVal = 0;
  bool irqReload = false;
  int irqCounter = 0;

  int prgBankMode = 0;
  int prgBank1 = 0; //$8000/$C000 bank
  int prgBank2 = 1; //$A000 bank
  int chrBankMode = 0;
  int chrBank2k1 = 0; // 2KB bank
  int chrBank2k2 = 2; // 2KB bank
  int chrBank1k1 = 4; // 1KB bank
  int chrBank1k2 = 5; // 1KB bank
  int chrBank1k3 = 6; // 1KB bank
  int chrBank1k4 = 7; // 1KB bank

  int a12NumCyclesDown = 0;
  int a12LastClock = 0;
  int currentClock = 0;

  Mapper_004(int prgBanks,int chrBanks):Mapper(prgBanks,chrBanks){}

  void Clock(){
    currentClock++;
  }

  int GetPrgAddr(int addr){
    int ramOffset = 0x2000*prgRAM;
    //RAM
    if (addr >= 0x6000 && addr <= 0x7FFF)
      return addr-0x6000;

    if (addr >= 0x8000 && addr <= 0x9FFF){
      if (!prgBankMode)
        return ((addr-0x8000)+0x2000*prgBank1)+ramOffset; // swapable
      //fixed to second to last bank
      return ((addr-0x8000)+0x2000*(numPrgBanks*2-2))+ramOffset;
    }

    if (addr >= 0xA000 && addr <= 0xBFFF){
      //swapable
      return ((addr-0xA000)+0x2000*prgBank2)+ramOffset;
    }

    if (addr >= 0xC000 && addr <= 0xDFFF){
      if (prgBankMode)
        return ((addr-0xC000)+0x2000*prgBank1)+ramOffset; // swapable
      //std::cout << std::hex << (((addr-0xC000)+0x4000*(numPrgBanks-2)+0x2000)+ramOffset) << std::endl;
      //fixed to second to last bank
      return ((addr-0xC000)+0x2000*(numPrgBanks*2-2))+ramOffset;previousCHRRead = addr;
    }

    //fixed last bank
    if (addr >= 0xE000 && addr <= 0xFFFF)
      return ((addr-0xE000)+0x2000*(numPrgBanks*2-1))+ramOffset;


    return addr;

  }

  int MapChrAddr(int addr){
    if (!chrBankMode){
      // 2KB
      if (addr >= 0x0000 && addr <= 0x07FF)
        return (addr-0x0000)+0x0400*chrBank2k1;
      if (addr >= 0x0800 && addr <= 0x0FFF)
        return (addr-0x0800)+0x0400*chrBank2k2;
      // 1KB
      if (addr >= 0x1000 && addr <= 0x13FF)
        return (addr-0x1000)+0x0400*chrBank1k1;
      if (addr >= 0x1400 && addr <= 0x17FF)
        return (addr-0x1400)+0x0400*chrBank1k2;
      if (addr >= 0x1800 && addr <= 0x1BFF)
        return (addr-0x1800)+0x0400*chrBank1k3;
      if (addr >= 0x1C00 && addr <= 0x1FFF)
        return (addr-0x1C00)+0x0400*chrBank1k4;
    }

    // 2KB
    if (addr >= 0x1000 && addr <= 0x17FF)
      return (addr-0x1000)+0x0400*chrBank2k1;
    if (addr >= 0x1800 && addr <= 0x1FFF)
      return (addr-0x1800)+0x0400*chrBank2k2;
    // 1KB
    if (addr >= 0x0000 && addr <= 0x03FF)
      return (addr-0x0000)+0x0400*chrBank1k1;
    if (addr >= 0x0400 && addr <= 0x07FF)
      return (addr-0x0400)+0x0400*chrBank1k2;
    if (addr >= 0x0800 && addr <= 0x0BFF)
      return (addr-0x0800)+0x0400*chrBank1k3;
    if (addr >= 0x0C00 && addr <= 0x0FFF)
      return (addr-0x0C00)+0x0400*chrBank1k4;

    return addr;
  }

  int PeekChrAddr(int addr){
    return MapChrAddr(addr);
  }

  int GetChrAddr(int addr){
    //std::cout << std::hex << addr << ' ' << previousCHRRead << std::endl;
    if ((addr&0x1000) == 0){
      // update how many clocks have happened since last check
      a12NumCyclesDown += currentClock-a12LastClock;
    }
    else if ((addr&0x1000)){
      //only count after 3 succsessive down clocks
      if (a12NumCyclesDown > 3){
        if (irqCounter == 0 || irqReload){
          irqCounter = irqReloadVal;
          irqReload = false;
        }
        else
          irqCounter--;

        if (irqCounter == 0 && irqEnable)
          irqPending = true;
      }
      a12NumCyclesDown = 0;
    }

    //reset clock counter
    a12LastClock = currentClock;

    return MapChrAddr(addr);
}

  int Write(int addr, int value){
    //RAM
    if (addr >= 0x6000 && addr <= 0x7FFF && enableRAM == true && ramWriteProtection == false)
      return addr-0x6000;

    //Bank Select (even) and Data (odd)
    if (addr >= 0x8000 && addr <= 0x9FFF){
      if (addr%2 == 0){
        currentBank = value&0x07;

        prgBankMode = (bool)value&0x40;
        chrBankMode = (bool)value&0x80;
      }
      else{
        switch (currentBank){
          case 0:
            chrBank2k1 = value&0xFE;
            break;
          case 1:
            chrBank2k2 = value&0xFE;
            break;
          case 2:
            chrBank1k1 = value;
            break;
          case 3:
            chrBank1k2 = value;
            break;
          case 4:
            chrBank1k3 = value;
            break;
          case 5:
            chrBank1k4 = value;
            break;
          case 6:
            prgBank1 = value&0x3F;
            break;
          case 7:
            prgBank2 = value&0x3F;
            break;
          default:
            break;
        }
      }
    }
    //Mirroring (even) and prgram protect (odd)
    else if (addr >= 0xA000 && addr <= 0xBFFF){
      if (addr%2 == 0)
        nameTableMirroring = value&0x01;
      else{
        ramWriteProtection = value&0x40;
        enableRAM = value&0x80;
      }
    }
    //irq latch and reload
    else if (addr >= 0xC000 && addr <= 0xDFFF){
      if (addr%2 == 0){
        irqReloadVal = value;
      }
      else{
        irqReload = true;
      }

    }
    //irq disable and enable
    else if (addr >= 0xE000 && addr <= 0xFFFF){
      if (addr%2 == 0){
        irqPending = false;
        irqEnable = false;
      }
      else
        irqEnable = true;
    }

    return -1;
  }
  int PpuWrite(int addr, int value){return addr;}//GetChrAddr(addr);}
  int GetNtMirrorMode() {return nameTableMirroring+1;}

  bool PollInterrupts(){
    return irqPending;
  }

  void SaveMapState(std::ofstream &file) override {
    file.write(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
    file.write(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
    file.write(reinterpret_cast<char*>(&currentBank),sizeof(int));
    file.write(reinterpret_cast<char*>(&nameTableMirroring),sizeof(int));
    file.write(reinterpret_cast<char*>(&ramWriteProtection),sizeof(int));
    file.write(reinterpret_cast<char*>(&enableRAM),sizeof(int));
    file.write(reinterpret_cast<char*>(&previousCHRRead),sizeof(int));
    file.write(reinterpret_cast<char*>(&irqEnable),sizeof(int));
    file.write(reinterpret_cast<char*>(&irqPending),sizeof(int));
    file.write(reinterpret_cast<char*>(&irqReloadVal),sizeof(int));
    file.write(reinterpret_cast<char*>(&irqReload),sizeof(int));
    file.write(reinterpret_cast<char*>(&irqCounter),sizeof(int));
    file.write(reinterpret_cast<char*>(&prgBankMode),sizeof(int));
    file.write(reinterpret_cast<char*>(&prgBank1),sizeof(int));
    file.write(reinterpret_cast<char*>(&prgBank2),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBankMode),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank2k1),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank2k2),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank1k1),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank1k2),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank1k3),sizeof(int));
    file.write(reinterpret_cast<char*>(&chrBank1k4),sizeof(int));
  }
  void LoadMapState(std::ifstream &file) override {
    file.read(reinterpret_cast<char*>(&numPrgBanks),sizeof(int));
    file.read(reinterpret_cast<char*>(&numChrBanks),sizeof(int));
    file.read(reinterpret_cast<char*>(&currentBank),sizeof(int));
    file.read(reinterpret_cast<char*>(&nameTableMirroring),sizeof(int));
    file.read(reinterpret_cast<char*>(&ramWriteProtection),sizeof(int));
    file.read(reinterpret_cast<char*>(&enableRAM),sizeof(int));
    file.read(reinterpret_cast<char*>(&previousCHRRead),sizeof(int));
    file.read(reinterpret_cast<char*>(&irqEnable),sizeof(int));
    file.read(reinterpret_cast<char*>(&irqPending),sizeof(int));
    file.read(reinterpret_cast<char*>(&irqReloadVal),sizeof(int));
    file.read(reinterpret_cast<char*>(&irqReload),sizeof(int));
    file.read(reinterpret_cast<char*>(&irqCounter),sizeof(int));
    file.read(reinterpret_cast<char*>(&prgBankMode),sizeof(int));
    file.read(reinterpret_cast<char*>(&prgBank1),sizeof(int));
    file.read(reinterpret_cast<char*>(&prgBank2),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBankMode),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank2k1),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank2k2),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank1k1),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank1k2),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank1k3),sizeof(int));
    file.read(reinterpret_cast<char*>(&chrBank1k4),sizeof(int));
  }
};
