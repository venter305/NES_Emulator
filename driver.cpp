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

Panel *gameScreen;
Panel *menuBar;

void renderGame(Panel*);
void renderMenuBar(Panel,Button,GLuint);

//Keyboard Callback
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
	switch (key){
		case GLFW_KEY_Z:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000001;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111110;
			break;
		case GLFW_KEY_X:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000010;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111101;
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000100;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111011;
			break;
		case GLFW_KEY_S:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00001000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11110111;
			break;
		case GLFW_KEY_UP:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00010000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11101111;
			break;
		case GLFW_KEY_DOWN:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00100000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11011111;
			break;
		case GLFW_KEY_LEFT:	
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b01000000;
			else if(action == GLFW_RELEASE)n.CONTRL.buttons_p1 &= 0b10111111;
			break;
		case GLFW_KEY_RIGHT:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b10000000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b01111111;
			break;
	};
}

static void char_callback(GLFWwindow *window, unsigned int codepoint){
	filePath += (char)codepoint;
}

//Mouse Button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
			double xPos, yPos;
			glfwGetCursorPos(window,&xPos,&yPos);
			int windowX,windowY;
			glfwGetWindowSize(window,&windowX,&windowY);
			yPos = windowY-yPos;
    	for (Button *b : buttons){
				if (xPos >= b->xPos && xPos <= (b->xPos+b->width) && yPos >= b->yPos && yPos <= (b->yPos+b->height))
					b->action(b);
			}	
		}
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

	glViewport(0,0,dialogX,dialogY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,dialogX,0.0,dialogY);

	int windowPosX,windowPosY;
	glfwGetWindowPos(n.window,&windowPosX,&windowPosY);
	filePath = "";
	int windowX,windowY;
	glfwGetWindowSize(n.window,&windowX,&windowY);
	glfwSetWindowPos(window,windowPosX+((windowX-dialogX)/2),windowPosY+((windowY-dialogY)/2));

	GLuint fbo;
	GLuint tex;
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dialogX,dialogY,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
	glBindTexture(GL_TEXTURE_2D,0);

	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	glViewport(0,0,dialogX,dialogY);
	
	
	glColor3f(.9,.9,.9);
	glRecti(0,0,dialogX,dialogY);
	Text label(0,(dialogY-15)/2,15,"Path to Rom:","/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
	label.draw();
	TextInput fileEntry(label.width,(dialogY-17)/2,dialogX-label.width,17);
	fileEntry.draw();

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
	
	Panel fileInput(0,0,dialogX,dialogY);
	fileInput.setTexture(tex);
	fileInput.draw();
	glfwSetCharCallback(window,char_callback);
	int deleteChar = true;
	while (!glfwGetKey(window,GLFW_KEY_ENTER)){
		if (glfwWindowShouldClose(window)){
			glfwDestroyWindow(window);
			glfwMakeContextCurrent(n.window);
			return;
		};
		glfwPollEvents();
		if (glfwGetKey(window,GLFW_KEY_BACKSPACE) && deleteChar){
			deleteChar = false;
			if (filePath.length() > 0)filePath.pop_back();
		}
		
		if (!glfwGetKey(window,GLFW_KEY_BACKSPACE))
			deleteChar = true;
		
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);
		glViewport(0,0,dialogX,dialogY);
		fileEntry.text->setText(filePath);
		fileEntry.drawText();
		fileEntry.draw();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
		fileInput.draw();
		glFlush();
		glfwSwapBuffers(window);
	}
	glfwSetCharCallback(window,NULL);
	glfwMakeContextCurrent(n.window);
	glfwDestroyWindow(window);
	ifstream f;
	f.open(filePath);
	if (f){
		f.close();
		n.reset();
		n.CART.readRom(filePath);
		n.CPU.reset();
	}
}

//Pause emulation
void pauseButtonAction(Button *b){
	if (!paused)
		b->text->setText("Resume");
	else
		b->text->setText("Pause");
	b->drawText();
	b->draw();
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

void loop(){
	if (paused == false){
		//run next instruction
		while(!n.PPU.frameComplete){
			//if (n.APU.soundMan->samplesNeeded)
				n.clock(1);
		}
		//frame is done rendering
		n.PPU.frameComplete = false;
		//render game screen and menud
		renderGame(gameScreen);
		if (debugging)
			d->display();
	}
	menuBar->draw();
	for (Button *b : buttons)
		b->draw();
}

int main(int argc, char** argv) {

	//Read Rom
	n.CART.readRom("Tetris.nes");
	
	n.CPU.reset();

	SoundManager soundMan;
	n.APU.soundMan = &soundMan;

	//Open Window, Set up Callback functions
	GraphicsEngine graphicsEngine(APP_WIDTH,APP_HEIGHT,"Nes Emulator");
	if (graphicsEngine.window == NULL)
		return 0;
	n.window = graphicsEngine.window;
	glfwSetKeyCallback(graphicsEngine.window,key_callback);
	glfwSetMouseButtonCallback(graphicsEngine.window, mouse_button_callback);
	glfwSetWindowSizeCallback(graphicsEngine.window, window_size_callback);

	d = new Debugger();
	d->n = &n;
	glfwSetWindowCloseCallback(d->window,debug_window_close_callback);
	
	glViewport(0,0,APP_WIDTH,APP_HEIGHT);

	//Game Screen panel
  gameScreen = new Panel(0,0,APP_WIDTH,APP_HEIGHT-20);
	gameScreen->genTexture(256,240);
	gameScreen->flipY();

	//Menu and buttons
	menuBar = new Panel(0,APP_HEIGHT-20,APP_WIDTH,20);
	menuBar->setColor(.9,.9,.9);
	
	string fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf";	
	//Open
	Button openButton(5,APP_HEIGHT-18,36,15,openButtonAction);
	openButton.text->setText("Open");
	openButton.setBackgroundColor(.9,.9,.9);
	openButton.drawText();
	buttons.push_back(&openButton);
	//Pause
	Button pauseButton(50,APP_HEIGHT-18,50,15, pauseButtonAction);
	pauseButton.text->setText("Pause");
	pauseButton.setBackgroundColor(.9,.9,.9);
	pauseButton.drawText();
	buttons.push_back(&pauseButton);	
	//Save State
	Button saveStateButton(110,APP_HEIGHT-18,70,15, saveStateAction);
	saveStateButton.text->setText("Save State");
	saveStateButton.setBackgroundColor(.9,.9,.9);
	saveStateButton.drawText();
	buttons.push_back(&saveStateButton);
	//Load State
	Button loadStateButton(200,APP_HEIGHT-18,70,15, loadStateAction);
	loadStateButton.text->setText("Load State");
	loadStateButton.setBackgroundColor(.9,.9,.9);
	loadStateButton.drawText();
	buttons.push_back(&loadStateButton);
	//Debugging
	Button debugButton(280,APP_HEIGHT-18,70,15, debugButtonAction);
	debugButton.text->setText("Debugger");
	debugButton.setBackgroundColor(.9,.9,.9);
	debugButton.drawText();
	buttons.push_back(&debugButton);

	//glfwSwapInterval(0);
	graphicsEngine.mainLoop = loop;
	graphicsEngine.run();
	
	delete gameScreen;
	delete menuBar;

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glfwTerminate();
	return 0;
}

//Draw game screen
void renderGame(Panel *p){
	glViewport(0,0,256,240);
	glBindFramebuffer(GL_FRAMEBUFFER,p->fbo);
	glRasterPos2i(0,0);	
	glDrawPixels(256,240,GL_RGB,GL_UNSIGNED_BYTE,n.PPU.pixelVal);
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	
	glViewport(0,0,APP_WIDTH,APP_HEIGHT);
	p->draw();
}

