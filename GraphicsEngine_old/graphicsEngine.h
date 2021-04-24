#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include "GUI/GUIManager.h"

class GraphicsEngine{

public:
		GLFWwindow *window = nullptr;
		std::string title;
		void (*mainLoop)(GLFWwindow*,double);
		
		
		
	public:
		GUIManager gui;
	
		GraphicsEngine(int,int,std::string,void(*)(GLFWwindow*,double));
		void run();
};
