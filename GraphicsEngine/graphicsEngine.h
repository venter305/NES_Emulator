#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include "../Input/Input.h"
#include "GUI/GUIManager.h"

class GraphicsEngine{

public:
	static GLFWwindow *window;
	static std::string title;

	static Input input;
	static GUIManager guiMan;

	 static void (*mainLoop)(GLFWwindow*,double);

	static void Init(int,int,std::string,void(*)(GLFWwindow*,double));
	static void Run();
};
