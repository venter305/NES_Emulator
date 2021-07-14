#pragma once
#include <sstream>

extern NES n;


class DebuggingWindow : public Window{
  void SelectTab(int newTab,int startID, int endID){
    tab = newTab;
    auto elements = guiMan.GetElementList();
    for (auto it = elements->begin();it != elements->end();it++){
      if (it->first == 0) continue;
      if(it->first >= startID && it->first < endID)
      it->second->visable = true;
      else
      it->second->visable = false;
    }
  }

  public:
    int tab = 1;
    bool grayscale = false;

    int numStackFrames = 10;

    DebuggingWindow(int w,int h,std::string name):Window(w,h,name){
    }

    const int TAB1 = 100;
    const int TAB2 = 200;
    const int TAB3 = 300;

    void OnStartup(){
      glClearColor(0.18f,0.2,0.23f,1.0f);

      std::shared_ptr<MenuBar> menuBar = std::make_shared<MenuBar>(0,baseHeight-25,baseWidth,25);
      menuBar->SetBackgroundColor(0.8f,0.8f,0.8f,1);
			menuBar->SetMenuButtonColor(0.8f,0.8f,0.8f,1);
      menuBar->AddMenuButton("CPU",[&](Button *btn){
        SelectTab(1,TAB1,TAB2);
      });
      menuBar->AddMenuButton("PPU",[&](Button *btn){
        SelectTab(2,TAB2,TAB3);
      });
      menuBar->AddMenuButton("APU",[&](Button *btn){
        SelectTab(3,TAB3,TAB3+100);
      });
      guiMan.addElement(menuBar,0);

      //CPU
      std::shared_ptr<ListLayout> cpuStatusList = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,0,baseHeight-30);
      cpuStatusList->SetPadding(10,10);

      std::shared_ptr<Text> pcRegText = std::make_shared<Text>(0,0,20,"PC: 0000");
      pcRegText->setTextColor(0.8f,0.8f,0.8f);
      cpuStatusList->AddElement(pcRegText,TAB1+0);

      std::shared_ptr<Text> aRegText = std::make_shared<Text>(0,0,20,"A: 0");
      aRegText->setTextColor(0.8f,0.8f,0.8f);
      cpuStatusList->AddElement(aRegText,TAB1+1);

      std::shared_ptr<Text> xRegText = std::make_shared<Text>(0,0,20,"X: 0");
      xRegText->setTextColor(0.8f,0.8f,0.8f);
      cpuStatusList->AddElement(xRegText,TAB1+2);

      std::shared_ptr<Text> yRegText = std::make_shared<Text>(0,0,20,"Y: 0");
      yRegText->setTextColor(0.8f,0.8f,0.8f);
      cpuStatusList->AddElement(yRegText,TAB1+3);

      std::shared_ptr<Text> pRegText = std::make_shared<Text>(0,0,20,"P: 0");
      pRegText->setTextColor(0.8f,0.8f,0.8f);
      cpuStatusList->AddElement(pRegText,TAB1+4);

      guiMan.AddLayout(cpuStatusList);

      std::shared_ptr<ListLayout> stackList = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,200,baseHeight-30);
      stackList->SetPadding(10,10);

      std::shared_ptr<Text> spRegText = std::make_shared<Text>(0,0,20,"SP: 0");
      spRegText->setTextColor(0.8f,0.8f,0.8f);
      stackList->AddElement(spRegText,TAB1+10);

      std::string stackFrames = "";
      for (int i = 0;i<numStackFrames;i++){
        stackFrames += "00\n";
      }

      std::shared_ptr<Text> stackFramesText = std::make_shared<Text>(0,0,20,stackFrames);
      stackFramesText->setTextColor(0.8f,0.8f,0.8f);
      stackList->AddElement(stackFramesText,TAB1+11);

      guiMan.AddLayout(stackList);

      std::shared_ptr<ListLayout> breakAddrList = std::make_shared<ListLayout>(ListLayout::ListMode::Horizontal,0,100);
      breakAddrList->SetPadding(10,10);

      std::shared_ptr<Text> breakAddrLabel = std::make_shared<Text>(0,-5,20,"Break On Address:");
      breakAddrLabel->setTextColor(0.8f,0.8f,0.8f);
      breakAddrList->AddElement(breakAddrLabel,TAB1+20);

      std::shared_ptr<TextInput> breakAddrInput = std::make_shared<TextInput>(0,0,150,20,"");
      breakAddrInput->setTextSize(17);
      float enabledColor[3] = {1.0f,1.0f,1.0f};
      float disabledColor[3] = {0.9f,0.9f,0.9f};
      breakAddrInput->setEnabledColor(enabledColor);
      breakAddrInput->setDisabledColor(disabledColor);
      breakAddrInput->setTextAlignment(Button::TextAlignment::Left);
      breakAddrInput->setEnabled(false);
      breakAddrList->AddElement(breakAddrInput,TAB1+21);


      guiMan.AddLayout(breakAddrList);

      std::shared_ptr<ListLayout> controlList = std::make_shared<ListLayout>(ListLayout::ListMode::Horizontal,10,50);
      controlList->SetPadding(10,10);

      std::shared_ptr<Button> breakBtn = std::make_shared<Button>(0,0,50,20,[&](Button *btn){
        auto breakAddrInput = guiMan.GetElement<TextInput>(TAB1+21);
        n.debugBreakAddr = std::stoi(breakAddrInput->text->text,NULL,16);
      });
      breakBtn->setText("Break");
      breakBtn->setBackgroundColor(0.9f,0.9f,0.9f);
      controlList->AddElement(breakBtn,TAB1+30);

      std::shared_ptr<Button> continueBtn = std::make_shared<Button>(0,0,75,20,[&](Button *btn){
        n.debugBreakAddr = -1;
      });
      continueBtn->setText("Continue");
      continueBtn->setBackgroundColor(0.9f,0.9f,0.9f);
      controlList->AddElement(continueBtn,TAB1+31);

      guiMan.AddLayout(controlList);

      //PPU
      std::shared_ptr<ListLayout> patternTablesList = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,0,baseHeight-30);
      patternTablesList->SetPadding(10,10);
      int patternTableSize = 175;

      std::shared_ptr<Panel> patternTables1 = std::make_shared<Panel>(0,0,patternTableSize,patternTableSize);
      patternTables1->flipY();
      patternTables1->CreateTexture(128,128,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      patternTables1->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      patternTables1->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      patternTablesList->AddElement(patternTables1,TAB2+0);

      std::shared_ptr<Panel> patternTables2 = std::make_shared<Panel>(0,0,patternTableSize,patternTableSize);
      patternTables2->flipY();
      patternTables2->CreateTexture(128,128,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      patternTables2->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      patternTables2->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      patternTablesList->AddElement(patternTables2,TAB2+1);

      std::shared_ptr<Panel> oamTable = std::make_shared<Panel>(0,0,patternTableSize,patternTableSize);
      oamTable->flipY();
      oamTable->CreateTexture(64,128,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      oamTable->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      oamTable->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      patternTablesList->AddElement(oamTable,TAB2+2);

      guiMan.AddLayout(patternTablesList);

      int nameTableSizeX = 256;
      int nameTableSizeY = 240;

      std::shared_ptr<GridLayout> nameTableGrid = std::make_shared<GridLayout>(200,baseHeight-30,2,nameTableSizeY,5);
      nameTableGrid->SetPadding(5,5);
      std::shared_ptr<Panel> nameTable1 = std::make_shared<Panel>(0,0,nameTableSizeX,nameTableSizeY);
      nameTable1->flipY();
      nameTable1->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      nameTable1->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      nameTable1->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      nameTableGrid->AddElementToRow(0,nameTable1,TAB2+10);

      std::shared_ptr<Panel> nameTable2 = std::make_shared<Panel>(0,0,nameTableSizeX,nameTableSizeY);
      nameTable2->flipY();
      nameTable2->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      nameTable2->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      nameTable2->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      nameTableGrid->AddElementToRow(0,nameTable2,TAB2+11);

      std::shared_ptr<Panel> nameTable3 = std::make_shared<Panel>(0,0,nameTableSizeX,nameTableSizeY);
      nameTable3->flipY();
      nameTable3->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      nameTable3->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      nameTable3->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      nameTableGrid->AddElementToRow(1,nameTable3,TAB2+12);

      std::shared_ptr<Panel> nameTable4 = std::make_shared<Panel>(0,0,nameTableSizeX,nameTableSizeY);
      nameTable4->flipY();
      nameTable4->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
      nameTable4->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      nameTable4->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      nameTableGrid->AddElementToRow(1,nameTable4,TAB2+13);

      guiMan.AddLayout(nameTableGrid);

      std::shared_ptr<Text> grayScaleLabel = std::make_shared<Text>(200,30,20,"Grayscale:");
      grayScaleLabel->setTextColor(0.8f,0.8f,0.8f);
      guiMan.addElement(grayScaleLabel,TAB2+20);

      std::shared_ptr<Checkbox> grayScaleCheckbox = std::make_shared<Checkbox>(290,30,10,10,&grayscale);
      guiMan.addElement(grayScaleCheckbox,TAB2+21);

      std::shared_ptr<Text> scanlines = std::make_shared<Text>(200,10,20,"Scanlines: 0");
      scanlines->setTextColor(0.8f,0.8f,0.8f);
      guiMan.addElement(scanlines,TAB2+22);


      //APU
      std::shared_ptr<Checkbox> pulse1MuteBtn = std::make_shared<Checkbox>(0,baseHeight-45,15,15,&n.APU.pulse1.debugEnableOutput);
      pulse1MuteBtn->SetChecked(true);
      guiMan.addElement(pulse1MuteBtn,TAB3+0);
      std::shared_ptr<ListLayout> pulse1Layout = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,20,baseHeight-30);
      pulse1Layout->SetPadding(0,10);
      std::shared_ptr<Text> pulse1Label = std::make_shared<Text>(0,0,20,"Pulse 1:");
      pulse1Label->setTextColor(0.8f,0.8f,0.8f);
      pulse1Layout->AddElement(pulse1Label,TAB3+1);
      std::shared_ptr<Text> p1Enabled = std::make_shared<Text>(0,0,20," Enabled: True");
      p1Enabled->setTextColor(0.8f,0.8f,0.8f);
      pulse1Layout->AddElement(p1Enabled,TAB3+2);
      std::shared_ptr<Text> p1Timer = std::make_shared<Text>(0,0,20," Timer: 0");
      p1Timer->setTextColor(0.8f,0.8f,0.8f);
      pulse1Layout->AddElement(p1Timer,TAB3+3);
      std::shared_ptr<Text> p1lenCounter = std::make_shared<Text>(0,0,20," Length Counter: 0");
      p1lenCounter->setTextColor(0.8f,0.8f,0.8f);
      pulse1Layout->AddElement(p1lenCounter,TAB3+4);
      guiMan.AddLayout(pulse1Layout);

      std::shared_ptr<Checkbox> pulse2MuteBtn = std::make_shared<Checkbox>(200,baseHeight-45,15,15,&n.APU.pulse2.debugEnableOutput);
      pulse2MuteBtn->SetChecked(true);
      guiMan.addElement(pulse2MuteBtn,TAB3+5);
      std::shared_ptr<ListLayout> pulse2Layout = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,200+20,baseHeight-30);
      pulse2Layout->SetPadding(0,10);
      std::shared_ptr<Text> pulse2Label = std::make_shared<Text>(0,0,20,"Pulse 2:");
      pulse2Label->setTextColor(0.8f,0.8f,0.8f);
      pulse2Layout->AddElement(pulse2Label,TAB3+6);
      std::shared_ptr<Text> p2Enabled = std::make_shared<Text>(0,0,20," Enabled: True");
      p2Enabled->setTextColor(0.8f,0.8f,0.8f);
      pulse2Layout->AddElement(p2Enabled,TAB3+7);
      std::shared_ptr<Text> p2Timer = std::make_shared<Text>(0,0,20," Timer: 0");
      p2Timer->setTextColor(0.8f,0.8f,0.8f);
      pulse2Layout->AddElement(p2Timer,TAB3+8);
      std::shared_ptr<Text> p2lenCounter = std::make_shared<Text>(0,0,20," Length Counter: 0");
      p2lenCounter->setTextColor(0.8f,0.8f,0.8f);
      pulse2Layout->AddElement(p2lenCounter,TAB3+9);
      guiMan.AddLayout(pulse2Layout);

      std::shared_ptr<Checkbox> triangleMuteBtn = std::make_shared<Checkbox>(400,baseHeight-45,15,15,&n.APU.triangle.debugEnableOutput);
      triangleMuteBtn->SetChecked(true);
      guiMan.addElement(triangleMuteBtn,TAB3+10);
      std::shared_ptr<ListLayout> triangleLayout = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,400+20,baseHeight-30);
      triangleLayout->SetPadding(0,10);
      std::shared_ptr<Text> triangleLabel = std::make_shared<Text>(0,0,20,"Triangle:");
      triangleLabel->setTextColor(0.8f,0.8f,0.8f);
      triangleLayout->AddElement(triangleLabel,TAB3+11);
      std::shared_ptr<Text> tEnabled = std::make_shared<Text>(0,0,20," Enabled: True");
      tEnabled->setTextColor(0.8f,0.8f,0.8f);
      triangleLayout->AddElement(tEnabled,TAB3+12);
      std::shared_ptr<Text> tTimer = std::make_shared<Text>(0,0,20," Timer: 0");
      tTimer->setTextColor(0.8f,0.8f,0.8f);
      triangleLayout->AddElement(tTimer,TAB3+13);
      std::shared_ptr<Text> tlenCounter = std::make_shared<Text>(0,0,20," Length Counter: 0");
      tlenCounter->setTextColor(0.8f,0.8f,0.8f);
      triangleLayout->AddElement(tlenCounter,TAB3+14);
      guiMan.AddLayout(triangleLayout);

      std::shared_ptr<Checkbox> noiseMuteBtn = std::make_shared<Checkbox>(0,400-45,15,15,&n.APU.noise.debugEnableOutput);
      noiseMuteBtn->SetChecked(true);
      guiMan.addElement(noiseMuteBtn,TAB3+15);
      std::shared_ptr<ListLayout> noiseLayout = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,0+20,400-30);
      noiseLayout->SetPadding(0,10);
      std::shared_ptr<Text> noiseLabel = std::make_shared<Text>(0,0,20,"Noise:");
      noiseLabel->setTextColor(0.8f,0.8f,0.8f);
      noiseLayout->AddElement(noiseLabel,TAB3+16);
      std::shared_ptr<Text> nEnabled = std::make_shared<Text>(0,0,20," Enabled: True");
      nEnabled->setTextColor(0.8f,0.8f,0.8f);
      noiseLayout->AddElement(nEnabled,TAB3+17);
      std::shared_ptr<Text> nlenCounter = std::make_shared<Text>(0,0,20," Length Counter: 0");
      nlenCounter->setTextColor(0.8f,0.8f,0.8f);
      noiseLayout->AddElement(nlenCounter,TAB3+18);
      guiMan.AddLayout(noiseLayout);

      std::shared_ptr<Checkbox> dmcMuteBtn = std::make_shared<Checkbox>(200,400-45,15,15,&n.APU.dmc.debugEnableOutput);
      dmcMuteBtn->SetChecked(true);
      guiMan.addElement(dmcMuteBtn,TAB3+19);
      std::shared_ptr<ListLayout> dmcLayout = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,200+20,400-30);
      dmcLayout->SetPadding(0,10);
      std::shared_ptr<Text> dmcLabel = std::make_shared<Text>(0,0,20,"DMC:");
      dmcLabel->setTextColor(0.8f,0.8f,0.8f);
      dmcLayout->AddElement(dmcLabel,TAB3+20);
      std::shared_ptr<Text> dmcEnabled = std::make_shared<Text>(0,0,20," Enabled: True");
      dmcEnabled->setTextColor(0.8f,0.8f,0.8f);
      dmcLayout->AddElement(dmcEnabled,TAB3+21);
      std::shared_ptr<Text> dmcTimer = std::make_shared<Text>(0,0,20," Timer: 0");
      dmcTimer->setTextColor(0.8f,0.8f,0.8f);
      dmcLayout->AddElement(dmcTimer,TAB3+22);
      guiMan.AddLayout(dmcLayout);

      std::shared_ptr<Text> frameCounterText = std::make_shared<Text>(20,200,20,"Frame Counter: 0");
      frameCounterText->setTextColor(0.8f,0.8f,0.8f);
      guiMan.addElement(frameCounterText,TAB3+23);

      auto elements = guiMan.GetElementList();
      for (auto it = elements->begin();it != elements->end();it++){
        if (it->first == 0) continue;
        if(it->first >= TAB1 && it->first < TAB2)
          it->second->visable = true;
        else
          it->second->visable = false;
      }
    }

    float elapsedTime = 0;
    void OnUpdate(double dTime){

      glClear(GL_COLOR_BUFFER_BIT);
      if (tab == 1){
        auto pcRegText = guiMan.GetElement<Text>(TAB1+0);
        std::stringstream pcRegHex;
        pcRegHex << std::hex << n.CPU.pc;
        pcRegText->setText("PC: " + pcRegHex.str());

        auto aRegText = guiMan.GetElement<Text>(TAB1+1);
        std::stringstream aRegHex;
        aRegHex << std::hex << (int)n.CPU.a;
        aRegText->setText("A: " + aRegHex.str());

        auto xRegText = guiMan.GetElement<Text>(TAB1+2);
        std::stringstream xRegHex;
        xRegHex << std::hex << (int)n.CPU.x;
        xRegText->setText("X: " + xRegHex.str());

        auto yRegText = guiMan.GetElement<Text>(TAB1+3);
        std::stringstream yRegHex;
        yRegHex << std::hex << (int)n.CPU.y;
        yRegText->setText("Y: " + yRegHex.str());

        auto pRegText = guiMan.GetElement<Text>(TAB1+4);
        std::stringstream pRegHex;
        pRegHex << std::hex << (int)n.CPU.p;
        pRegText->setText("P: " + pRegHex.str());

        auto spRegText = guiMan.GetElement<Text>(TAB1+10);
        std::stringstream spRegHex;
        spRegHex << std::hex << (int)n.CPU.s;
        spRegText->setText("SP: " + spRegHex.str());

        auto stackFrames = guiMan.GetElement<Text>(TAB1+11);
        std::stringstream stackFramesHex;
        stackFramesHex << std::hex;
        for (int i=0;i<numStackFrames;i++){
          if (0xFF-n.CPU.s <= i)
            stackFramesHex << 0 << '\n';
          else
            stackFramesHex << (int)n.peekMemory(0x01FF-i) << '\n';
        }

        stackFrames->setText(stackFramesHex.str());
      }
      else if (tab == 2){
        uint8_t patternTableBuffer1[256*8*8*3];
        uint8_t patternTableBuffer2[256*8*8*3];

        n.PPU.DrawChars(0,patternTableBuffer1,grayscale);
        n.PPU.DrawChars(1,patternTableBuffer2,grayscale);

        auto patternTables1 = guiMan.GetElement<Panel>(TAB2+0);
        patternTables1->UpdateTexture(0,0,128,128,GL_RGB,GL_UNSIGNED_BYTE,patternTableBuffer1);

        auto patternTables2 = guiMan.GetElement<Panel>(TAB2+1);
        patternTables2->UpdateTexture(0,0,128,128,GL_RGB,GL_UNSIGNED_BYTE,patternTableBuffer2);

        uint8_t oamBuffer[64*8*16*3];
        for (int i=0;i<64*8*16*3;i++)
          oamBuffer[i] = 0;

        n.PPU.DrawSprites(oamBuffer);

        auto oamTable = guiMan.GetElement<Panel>(TAB2+2);
        oamTable->UpdateTexture(0,0,64,128,GL_RGB,GL_UNSIGNED_BYTE,oamBuffer);

        uint8_t nameTableBuffer1[256*240*3];
        uint8_t nameTableBuffer2[256*240*3];
        uint8_t nameTableBuffer3[256*240*3];
        uint8_t nameTableBuffer4[256*240*3];

        if (dTime - elapsedTime > 0.1f){
          elapsedTime = dTime;
          n.PPU.DrawNametable(0,nameTableBuffer1);
          n.PPU.DrawNametable(1,nameTableBuffer2);
          n.PPU.DrawNametable(2,nameTableBuffer3);
          n.PPU.DrawNametable(3,nameTableBuffer4);

          auto nameTable1 = guiMan.GetElement<Panel>(TAB2+10);
          nameTable1->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,nameTableBuffer1);
          auto nameTable2 = guiMan.GetElement<Panel>(TAB2+11);
          nameTable2->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,nameTableBuffer2);
          auto nameTable3 = guiMan.GetElement<Panel>(TAB2+12);
          nameTable3->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,nameTableBuffer3);
          auto nameTable4 = guiMan.GetElement<Panel>(TAB2+13);
          nameTable4->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,nameTableBuffer4);
        }

        auto scanlines = guiMan.GetElement<Text>(TAB2+22);
        scanlines->setText("Scanlines: " + std::to_string(n.PPU.scanlines));
      }
      else if (tab == 3){
        auto p1Enabled = guiMan.GetElement<Text>(TAB3+2);
        p1Enabled->setText((n.APU.pulse1.enabled?" Enabled: True":" Enabled: False"));
        auto p1Timer = guiMan.GetElement<Text>(TAB3+3);
        p1Timer->setText(" Timer: " + std::to_string(n.APU.pulse1.timer));
        auto p1lenCounter = guiMan.GetElement<Text>(TAB3+4);
        p1lenCounter->setText(" Length Counter: " + std::to_string(n.APU.pulse1.lenCounter));

        auto p2Enabled = guiMan.GetElement<Text>(TAB3+7);
        p2Enabled->setText((n.APU.pulse2.enabled?" Enabled: True":" Enabled: False"));
        auto p2Timer = guiMan.GetElement<Text>(TAB3+8);
        p2Timer->setText(" Timer: " + std::to_string(n.APU.pulse2.timer));
        auto p2lenCounter = guiMan.GetElement<Text>(TAB3+9);
        p2lenCounter->setText(" Length Counter: " + std::to_string(n.APU.pulse2.lenCounter));

        auto tEnabled = guiMan.GetElement<Text>(TAB3+12);
        tEnabled->setText((n.APU.triangle.enabled?" Enabled: True":" Enabled: False"));
        auto tTimer = guiMan.GetElement<Text>(TAB3+13);
        tTimer->setText(" Timer: " + std::to_string(n.APU.triangle.timer));
        auto tlenCounter = guiMan.GetElement<Text>(TAB3+14);
        tlenCounter->setText(" Length Counter: " + std::to_string(n.APU.triangle.lengthCounter));

        auto nEnabled = guiMan.GetElement<Text>(TAB3+17);
        nEnabled->setText((n.APU.noise.enabled?" Enabled: True":" Enabled: False"));
        auto nlenCounter = guiMan.GetElement<Text>(TAB3+18);
        nlenCounter->setText(" Length Counter: " + std::to_string(n.APU.noise.lenCounter));

        auto dmcEnabled = guiMan.GetElement<Text>(TAB3+21);
        dmcEnabled->setText((n.APU.dmc.enabled?" Enabled: True":" Enabled: False"));
        auto dmcTimer = guiMan.GetElement<Text>(TAB3+22);
        dmcTimer->setText(" Timer: " + std::to_string(n.APU.dmc.timer));

        auto frameCounterText = guiMan.GetElement<Text>(TAB3+23);
        frameCounterText->setText("Frame Counter: " + std::to_string(n.APU.frameCounter.count));
      }


      guiMan.drawElements();
    }

    void OnShutdown(){
    }

    void OnEvent(Event &ev){
      guiMan.HandleEvent(ev);
    }
};
