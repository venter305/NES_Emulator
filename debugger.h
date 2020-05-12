#ifndef DEBUGGER_H_
#define DEBUFFER_H_

#include "./GraphicsEngine/GUI/panel.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "./GraphicsEngine/GUI/textObject.h"
#include "NES.h"

class Debugger{
	public:
		GLFWwindow *window;

		float *pixelVal;

		//Gluint vboId;
		//Gluint iboId;

		std::vector<Text> textList;

		Panel *charsPanel;
		Panel *ntPanel;

		NES *n;
		

		Debugger();
		~Debugger();
		void init();
		void display();
};

#endif 
