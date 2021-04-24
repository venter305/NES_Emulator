#include "graphicsEngine.h"
#include <iostream>

using namespace std;

GraphicsEngine::GraphicsEngine(int w,int h,string name,void (*renderFunc)(GLFWwindow*,double)) : title(name),mainLoop(renderFunc){
	if (!glfwInit()){
		return;
	}
	window = glfwCreateWindow(w,h,name.c_str(),NULL, NULL);
	glfwMakeContextCurrent(window);
	//Set up projection
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,w,0.0,h);
	//Enable transparent textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewInit();
	
}

void GraphicsEngine::run(){
	float startTime = glfwGetTime();
	float dTime = 0;
	int y = 0;
	
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		y++;
		mainLoop(window,glfwGetTime());
		if (glfwGetTime()-dTime >= 1){
			glfwSetWindowTitle(window,(title + " - Program FPS:" + to_string(y)).c_str());
			y = 0;
			dTime = glfwGetTime();
		}
		glfwSwapBuffers(window);
	}
}


