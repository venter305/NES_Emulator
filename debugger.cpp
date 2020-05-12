#include <iostream>
#include "debugger.h"

using namespace std;

Debugger::Debugger(){
	int dialogX = 500;
	int dialogY = 750;
	GLFWwindow *currWindow = glfwGetCurrentContext();
	window = glfwCreateWindow(dialogX,dialogY,"Debugger",NULL, NULL);
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Set up projection
	glViewport(0,0,dialogX,dialogY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,dialogX,0.0,dialogY);
	
	glfwHideWindow(window);

	charsPanel = new Panel(0,500,256,128);
	ntPanel = new Panel(0,0,512,500);
	glfwMakeContextCurrent(currWindow);

}

Debugger::~Debugger(){
	delete charsPanel;
	delete ntPanel;
}

void Debugger::display(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(window);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	/*n->PPU.drawChars(charsPanel);
	charsPanel->setSize(512,256);
	charsPanel->flipY();
	charsPanel->draw();

	n->PPU.drawBackground(ntPanel);
	ntPanel->setSize(512,500);
	ntPanel->flipY();
	ntPanel->draw();*/
	
	
	glFlush();
	glfwSwapBuffers(window);

	glfwMakeContextCurrent(currWindow);
}
