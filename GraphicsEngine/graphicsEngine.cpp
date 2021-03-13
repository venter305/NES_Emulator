#include "graphicsEngine.h"
#include <iostream>

using namespace std;

Input GraphicsEngine::input;
GUIManager GraphicsEngine::guiMan;

GLFWwindow *GraphicsEngine::window = nullptr;
std::string GraphicsEngine::title;
void (*GraphicsEngine::mainLoop)(GLFWwindow*,double);

void GraphicsEngine::Init(int w,int h,string name,void (*renderFunc)(GLFWwindow*,double)){
	title = name;
	mainLoop = renderFunc;

	if (!glfwInit()){
		return;
	}
	window = glfwCreateWindow(w,h,name.c_str(),NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window,[](GLFWwindow* window, int key, int scancode, int action, int mods){
		KeyEvent *ev = new KeyEvent(key,(KeyEvent::KeyState)action);
		input.onEvent(*ev);
		delete ev;
	});

	glfwSetCharCallback(window,[](GLFWwindow* window,unsigned int keycode){
		CharEvent *ev = new CharEvent(keycode);
		input.onEvent(*ev);
		delete ev;
	});

	glfwSetMouseButtonCallback(window,[](GLFWwindow* window, int button, int action, int mods){
		double mouseX,mouseY;
		input.GetMousePos(mouseX,mouseY);
		MouseButtonEvent *ev = new MouseButtonEvent((MouseButtonEvent::ButtonType)button,(MouseButtonEvent::ButtonState)action,mouseX,mouseY);
		input.onEvent(*ev);
		delete ev;
	});

	input.window = window;

	//Enable transparent textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewInit();

}

void GraphicsEngine::Run(){
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
