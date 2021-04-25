#include <iostream>
#include 	<bitset>
#include <fstream>
#include <time.h>
#include <chrono>
#include <thread>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <sstream>
#include "./GraphicsEngine/graphicsEngine.h"
#include "NES.h"
#include "SoundManager/soundManager.h"
#include "OpenRomWindow.h"


int APP_WIDTH = 256*2;
int APP_HEIGHT = 240*2+20;

//Panel *gameScreen;
//Panel *menuBar;
NES n;
bool paused = false;

class MainWindow : public Window{
	public:

		MainWindow(int w,int h,std::string name):Window(w,h,name){
		}

		void OnStartup(){
			glClearColor(0.9f,0.9f,0.9f,1.0f);

			std::shared_ptr<ListLayout> mainList = std::make_shared<ListLayout>(ListLayout::ListMode::Vertical,0,APP_HEIGHT);

			std::shared_ptr<MenuBar> menuBar = std::make_shared<MenuBar>(0,0,baseWidth,25);
			menuBar->SetBackgroundColor(0.8f,0.8f,0.8f,1);
			menuBar->SetMenuButtonColor(0.8f,0.8f,0.8f,1);
			menuBar->AddMenuButton("Open",[&](Button *btn){
				paused = true;
				GraphicsEngine::AddWindow(new OpenRomWindow(350,50,"Open Rom",this));
			});
			menuBar->AddMenuButton("Pause",[&](Button *btn){
				if (!paused)
					btn->setText("Resume");
				else
					btn->setText("Pause");
				paused = !paused;
			});
			menuBar->AddMenuButton("Save State",[&](Button *btn){
				n.saveState();
			});
			menuBar->AddMenuButton("Load State",[&](Button *btn){
				n.loadState();
			});
			mainList->AddElement(menuBar,0);

			//Game Screen panel
		  std::shared_ptr<Panel> gameScreen = std::make_shared<Panel>(0,0,APP_WIDTH,APP_HEIGHT-20);
			gameScreen->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
			glBindTexture(GL_TEXTURE_2D,gameScreen->tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			gameScreen->flipY();
			mainList->AddElement(gameScreen,100);

			guiMan.AddLayout(mainList);
		}

		void OnUpdate(double dTime){
			glClear(GL_COLOR_BUFFER_BIT);

			if (paused == false){
				//Update Game Screen
				auto gameScreen = std::static_pointer_cast<Panel>(guiMan.elements[100]);
				gameScreen->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);
			}

			guiMan.drawElements();
		}

		void OnShutdown(){
		}

		void OnEvent(Event &ev){
			guiMan.HandleEvent(ev);

			switch (ev.GetType()){
				case Event::Key:
					{
						int keyCode = static_cast<KeyEvent*>(&ev)->GetKeyCode();
						int keyState = static_cast<KeyEvent*>(&ev)->GetKeyState();
						switch (keyCode){
							case GLFW_KEY_Z:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00000001;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11111110;
								break;
							case GLFW_KEY_X:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00000010;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11111101;
								break;
							case GLFW_KEY_A:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00000100;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11111011;
								break;
							case GLFW_KEY_S:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00001000;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11110111;
								break;
							case GLFW_KEY_UP:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00010000;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11101111;
								break;
							case GLFW_KEY_DOWN:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b00100000;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b11011111;
								break;
							case GLFW_KEY_LEFT:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b01000000;
								else if(keyState == KeyEvent::KeyState::Released)n.CONTRL.buttons_p1 &= 0b10111111;
								break;
							case GLFW_KEY_RIGHT:
								if (keyState == KeyEvent::KeyState::Pressed) n.CONTRL.buttons_p1 |= 0b10000000;
								else if(keyState == KeyEvent::KeyState::Released) n.CONTRL.buttons_p1 &= 0b01111111;
						}
						break;
					}
			}
		}

};

uint8_t soundLoop (){
	if (!paused){
		while(!n.clock(1) && !paused){};
	}
	return n.APU.output;
}

int main(int argc, char** argv) {
	std::string romName = "Roms/";
	if (argc > 1)
		romName += argv[1];
	else
		romName += "SMB.nes";

	//Read Rom
	n.CART.readRom(romName);

	n.CPU.reset();

	SoundManager soundMan;
	n.APU.soundMan = &soundMan;
	soundMan.UserFunc = soundLoop;

	//Open Window
	GraphicsEngine::Init();

	//glfwSwapInterval(0);
	GraphicsEngine::AddWindow(new MainWindow(APP_WIDTH,APP_HEIGHT,"Nes Emulator"));
	GraphicsEngine::Run();

	//glUnmapBuffer(GL_ARRAY_BUFFER);
	glfwTerminate();
	return 0;
}
