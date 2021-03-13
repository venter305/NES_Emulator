#include <iostream>
#include 	<bitset>
//#include <GL/glut.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "NES.h"
#include <time.h>
#include <chrono>
#include <thread>
#include "./GraphicsEngine/GUI/panel.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <sstream>
#include "./GraphicsEngine/GUI/textObject.h"
#include "./GraphicsEngine/GUI/button.h"
#include "./GraphicsEngine/GUI/textInput.h"
#include "debugger.h"
#include "SoundManager/soundManager.h"
#include "./GraphicsEngine/graphicsEngine.h"

using namespace std;

NES n;
Debugger *d;
vector<Button*> buttons;
string filePath;
bool paused = false;
bool debugging = false;

int APP_WIDTH = 256*2;
int APP_HEIGHT = 240*2+20;

//Panel *gameScreen;
//Panel *menuBar;

void renderGame(Panel*);
void renderMenuBar(Panel,Button,GLuint);

void OnEvent(Event &ev){
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

		case Event::Character:
		{
			int charCode = static_cast<CharEvent*>(&ev)->GetCharCode();
			break;
		}

		case Event::MouseButton:
		{
			MouseButtonEvent::ButtonType btnType = static_cast<MouseButtonEvent*>(&ev)->GetButtonType();
			MouseButtonEvent::ButtonState btnState = static_cast<MouseButtonEvent*>(&ev)->GetButtonState();
		  double mouseX = static_cast<MouseButtonEvent*>(&ev)->GetMouseX();
			double mouseY = static_cast<MouseButtonEvent*>(&ev)->GetMouseY();

			if(btnType == MouseButtonEvent::ButtonType::Left && btnState == MouseButtonEvent::ButtonState::Pressed){

				for (int i=200;i<=204;i++){
					std::static_pointer_cast<Panel>(GraphicsEngine::guiMan.elements[i])->clickAction(mouseX,mouseY);
				}
			}
			break;
		}
	}
}

static void char_callback(GLFWwindow *window, unsigned int codepoint){
	filePath += (char)codepoint;
}

//Open new rom
void openButtonAction(Button *b){
	int dialogX = 300;
	int dialogY = 50;
	GLFWwindow *window = glfwCreateWindow(dialogX,dialogY,"Enter Rom",NULL, NULL);
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//
	glViewport(0,0,dialogX,dialogY);

	int windowPosX,windowPosY;
	glfwGetWindowPos(n.window,&windowPosX,&windowPosY);
	filePath = "";
	int windowX,windowY;
	glfwGetWindowSize(n.window,&windowX,&windowY);
	glfwSetWindowPos(window,windowPosX+((windowX-dialogX)/2),windowPosY+((windowY-dialogY)/2));


	 Panel fileInput(0,0,dialogX,dialogY);
	// fileInput.setColor(0.9f,0.9f,0.9f,1.0f);

	//Text label(0,(dialogY-15)/2,15,"Path to Rom:","/usr/share/fonts/TTF/DejaVuSerif.ttf");

	//TextInput fileEntry(label.width,(dialogY-17)/2,dialogX-label.width,17);



	glfwSetCharCallback(window,char_callback);
	int deleteChar = true;

	paused = true;
	while (!glfwGetKey(window,GLFW_KEY_ENTER)){
		if (glfwWindowShouldClose(window)){
			return;
		}
		glfwPollEvents();
		if (glfwGetKey(window,GLFW_KEY_BACKSPACE) && deleteChar){
			deleteChar = false;
			if (filePath.length() > 0)filePath.pop_back();
		}

		if (!glfwGetKey(window,GLFW_KEY_BACKSPACE))
			deleteChar = true;

		//fileInput.draw();

		//label.draw();

		//fileEntry.text->setText(filePath);
		//fileEntry.draw();

		glfwSwapBuffers(window);
	}
	//glfwSetCharCallback(window,NULL);
	glfwDestroyWindow(window);
	glfwMakeContextCurrent(GraphicsEngine::window);
	glViewport(0,0,APP_WIDTH,APP_HEIGHT);

	ifstream f;
  filePath = "zelda.nes";
	f.open(filePath);
	if (f){
		f.close();
		n.reset();
		n.CART.readRom(filePath);
		n.CPU.reset();
	}

	paused = false;
}

//Pause emulation
void pauseButtonAction(Button *b){
	if (!paused)
		b->setText("Resume");
	else
		b->setText("Pause");
	paused = !paused;
}

//Save State
void saveStateAction(Button *b){
	n.saveState();
}

//Load Save State
void loadStateAction(Button *b){
	n.loadState();
}

//Close Debug Window
void debug_window_close_callback(GLFWwindow *window){
	glfwHideWindow(window);
	debugging = false;
}

//Start Debugger
void debugButtonAction(Button *b){
	glfwShowWindow(d->window);
	debugging = true;
}

//Resize window
void window_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,width,0.0,height);
	APP_WIDTH = width;
	APP_HEIGHT = height;
}

void renderLoop(GLFWwindow *window,double dtime){
	if (paused == false){
		//Update Game Screen
		auto gameScreen = std::static_pointer_cast<Panel>(GraphicsEngine::guiMan.elements[100]);
		gameScreen->UpdateTexture(0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);

		if (debugging)
			d->display();
	}

	GraphicsEngine::guiMan.drawElements();
}

uint8_t soundLoop (){
	if (!paused){
		while(!n.clock(1) && !paused){};
	}

	return n.APU.output;

}

int main(int argc, char** argv) {

	//Read Rom
	n.CART.readRom("SMB.nes");

	n.CPU.reset();

	SoundManager soundMan;
	n.APU.soundMan = &soundMan;
	soundMan.UserFunc = soundLoop;

	//Open Window
	GraphicsEngine::Init(APP_WIDTH,APP_HEIGHT,"Nes Emulator",renderLoop);
	if (GraphicsEngine::window == NULL)
		return 0;

	n.window = GraphicsEngine::window;

	//Game Screen panel
  std::shared_ptr<Panel> gameScreen = std::make_shared<Panel>(0,0,APP_WIDTH,APP_HEIGHT-20);
	gameScreen->CreateTexture(256,240,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
	glBindTexture(GL_TEXTURE_2D,gameScreen->tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gameScreen->flipY();
	GraphicsEngine::guiMan.addElement(gameScreen,100);

	//Menu and buttons
	std::shared_ptr<Panel> menuBar = std::make_shared<Panel>(0,APP_HEIGHT-20,APP_WIDTH,20);
	menuBar->setColor(.9,.9,.9,1);
	GraphicsEngine::guiMan.addElement(menuBar,101);


	string fontPath = "/usr/share/fonts/TTF/DejaVuSerif.ttf";
	//Open
	std::shared_ptr<Button> openButton = std::make_shared<Button>(5,APP_HEIGHT-18,36,15,openButtonAction);
	openButton->setText("Open");
	openButton->setBackgroundColor(.9,.9,.9);
	GraphicsEngine::guiMan.addElement(openButton,200);

	//Pause
	std::shared_ptr<Button> pauseButton = std::make_shared<Button>(50,APP_HEIGHT-18,50,15, pauseButtonAction);
	pauseButton->setText("Pause");
	pauseButton->setBackgroundColor(.9,.9,.9);
	GraphicsEngine::guiMan.addElement(pauseButton,201);

	//Save State
	std::shared_ptr<Button> saveStateButton = std::make_shared<Button>(110,APP_HEIGHT-18,70,15, saveStateAction);
	saveStateButton->setText("Save State");
	saveStateButton->setBackgroundColor(.9,.9,.9);
	GraphicsEngine::guiMan.addElement(saveStateButton,202);

	//Load State
	std::shared_ptr<Button> loadStateButton = std::make_shared<Button>(200,APP_HEIGHT-18,70,15, loadStateAction);
	loadStateButton->setText("Load State");
	loadStateButton->setBackgroundColor(.9,.9,.9);
	GraphicsEngine::guiMan.addElement(loadStateButton,203);

	//Debugging
	std::shared_ptr<Button> debugButton = std::make_shared<Button>(280,APP_HEIGHT-18,70,15, debugButtonAction);
	debugButton->setText("Debugger");
	debugButton->setBackgroundColor(.9,.9,.9);
	GraphicsEngine::guiMan.addElement(debugButton,204);

	//glfwSwapInterval(0);
	GraphicsEngine::input.onEvent = OnEvent;
	GraphicsEngine::Run();

	//glUnmapBuffer(GL_ARRAY_BUFFER);
	glfwTerminate();
	return 0;
}

//Draw game screen
void renderGame(Panel *p){
	// glViewport(0,0,256,240);
	// glBindFramebuffer(GL_FRAMEBUFFER,p->fbo);
	// glRasterPos2i(0,0);
	// GLbyte texData[240][256][3];
	// for (int y=0;y < 240;y++)
	// 	for(int x=0;x<256;x++){
	// 		texData[y][x][0] = 255;
	// 		texData[y][x][1] = 0;
	// 		texData[y][x][2] = 0;
	// 	}
	//
	//
	// glDrawPixels(256,240,GL_RGB,GL_UNSIGNED_BYTE,texData);
	// //glDrawPixels(256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);
	//
	// glBindFramebuffer(GL_FRAMEBUFFER,0);
	//
	// glViewport(0,0,APP_WIDTH,APP_HEIGHT);
	glBindTexture(GL_TEXTURE_2D,p->tex);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);
	p->draw();
}
