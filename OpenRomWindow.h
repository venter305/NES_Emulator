#pragma once

extern NES n;
extern bool paused;

class OpenRomWindow : public Window{
  public:
    Window *parentWindow;

    OpenRomWindow(int w,int h,std::string name,Window *parent):Window(w,h,name){
      parentWindow = parent;
    };

    void OnStartup(){
      glClearColor(0.8f,0.8f,0.8f,1.0f);

      int windowPosX,windowPosY;
    	glfwGetWindowPos(parentWindow->GetGLFWwindow(),&windowPosX,&windowPosY);
    	int windowX,windowY;
    	glfwGetWindowSize(parentWindow->GetGLFWwindow(),&windowX,&windowY);
    	glfwSetWindowPos(window,windowPosX+((windowX-width)/2),windowPosY+((windowY-height)/2));

      std::shared_ptr<ListLayout> layout = std::make_shared<ListLayout>(ListLayout::ListMode::Horizontal,0,height/2);

      layout->AddElement(std::make_shared<Text>(0,0,17,"Path to Rom: "));
      std::shared_ptr<TextInput> pathInput = std::make_shared<TextInput>(0,5,150,20,"");
      pathInput->setTextSize(17);
      float enabledColor[3] = {1.0f,1.0f,1.0f};
      float disabledColor[3] = {0.9f,0.9f,0.9f};
      pathInput->setEnabledColor(enabledColor);
      pathInput->setDisabledColor(disabledColor);
      pathInput->setTextAlignment(Button::TextAlignment::Left);
      layout->AddElement(pathInput,100);

      std::shared_ptr<Button> openBtn = std::make_shared<Button>(10,5,75,20,[&](Button *btn){
        glfwSetWindowShouldClose(window,true);

        auto pathInput = guiMan.GetElement<TextInput>(100);
        std::string romName = "Roms/" + pathInput->text->text;
        if (!n.CART.readRom(romName))
          return;
        n.CPU.reset();
      });

      openBtn->setText("Open Rom");
      openBtn->setBackgroundColor(0.9,0.9,0.9);
      layout->AddElement(openBtn);

      guiMan.AddLayout(layout);
    }

    void OnUpdate(double dTime){
      glClear(GL_COLOR_BUFFER_BIT);
      guiMan.drawElements();
    }

    void OnShutdown(){
      paused = false;
    }

    void OnEvent(Event &ev){
      guiMan.HandleEvent(ev);
    }
};
