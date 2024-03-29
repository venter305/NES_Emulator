#pragma once

extern const int APP_WIDTH;
extern const int APP_HEIGHT;

extern NES n;
extern bool paused;

class MainWindow : public Window{
	public:

		MainWindow(int w,int h,std::string name):Window(w,h,name){
		}

		void GUIinit(){
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
			menuBar->AddMenuButton("Reset",[&](Button *btn){
				 n.needReset = true;
			});
			menuBar->AddMenuButton("Save State",[&](Button *btn){
				n.saveState();
			});
			menuBar->AddMenuButton("Load State",[&](Button *btn){
				n.loadState();
			});
			menuBar->AddMenuButton("Debugging",[&](Button *btn){
				GraphicsEngine::AddWindow(new DebuggingWindow(720,580,"Debug"));
			});
			menuBar->AddMenuButton("Toggle Fast Forward",[&](Button *btn){
				if (n.runSpeed > 1)
					n.runSpeed = 1;
				else
					n.runSpeed = 2;
			});

			mainList->AddElement(menuBar,0);

			//Game Screen panel
			std::shared_ptr<Panel> gameScreen = std::make_shared<Panel>(0,0,APP_WIDTH,APP_HEIGHT-25);
			gameScreen->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
			gameScreen->ChangeTextureParamater(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			gameScreen->ChangeTextureParamater(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			gameScreen->flipY();
			mainList->AddElement(gameScreen,100);

			guiMan.AddLayout(mainList);
		}

		void OnStartup(){

			GUIinit();

			//Button layout
			n.CONTRL.buttonMapping.btn_A = GLFW_KEY_Z;
			n.CONTRL.buttonMapping.btn_B = GLFW_KEY_X;
			n.CONTRL.buttonMapping.btn_SELECT = GLFW_KEY_A;
			n.CONTRL.buttonMapping.btn_START = GLFW_KEY_S;
			n.CONTRL.buttonMapping.btn_UP = GLFW_KEY_UP;
			n.CONTRL.buttonMapping.btn_DOWN = GLFW_KEY_DOWN;
			n.CONTRL.buttonMapping.btn_LEFT = GLFW_KEY_LEFT;
			n.CONTRL.buttonMapping.btn_RIGHT = GLFW_KEY_RIGHT;

			n.CONTRL.joystickMapping.btn_A = GLFW_GAMEPAD_BUTTON_A;
			n.CONTRL.joystickMapping.btn_B = GLFW_GAMEPAD_BUTTON_X;
			n.CONTRL.joystickMapping.btn_SELECT = GLFW_GAMEPAD_BUTTON_BACK;
			n.CONTRL.joystickMapping.btn_START = GLFW_GAMEPAD_BUTTON_START;
			n.CONTRL.joystickMapping.btn_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
			n.CONTRL.joystickMapping.btn_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
			n.CONTRL.joystickMapping.btn_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
			n.CONTRL.joystickMapping.btn_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
		}

		void OnUpdate(double dTime){
			glClear(GL_COLOR_BUFFER_BIT);

			if (paused == false){
				//Update Game Screen
				auto gameScreen = guiMan.GetElement<Panel>(100);
				gameScreen->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);
			}

			guiMan.drawElements();
		}

		void OnShutdown(){
			GraphicsEngine::CloseAllWindows();
		}

		void OnEvent(Event &ev){
			guiMan.HandleEvent(ev);

			switch (ev.GetType()){
				case Event::Key:
					{
						int keyCode = static_cast<KeyEvent*>(&ev)->GetKeyCode();
						int keyState = static_cast<KeyEvent*>(&ev)->GetKeyState();
						n.CONTRL.UpdateButton(keyState,keyCode,0,false);
						break;
					}
				case Event::JoystickButton:
					{
						int button = static_cast<JoystickButtonEvent*>(&ev)->GetButton();
						int state = static_cast<JoystickButtonEvent*>(&ev)->GetState();
						n.CONTRL.UpdateButton(state,button,0,true);

						if (button == GLFW_GAMEPAD_BUTTON_LEFT_BUMPER)
							n.runSpeed = (state == GLFW_PRESS)?2:1;

						break;
					}
			}
		}

};
