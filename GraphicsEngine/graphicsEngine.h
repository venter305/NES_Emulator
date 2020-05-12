#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class GraphicsEngine{
	
	public:
		GLFWwindow *window = NULL;
	
	GraphicsEngine(int,int,std::string);
	void (*mainLoop)();
	void run();
};

#endif
