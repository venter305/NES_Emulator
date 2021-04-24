#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <string>

class Panel{
	public:
		GLuint xPos;
		GLuint yPos;
		GLuint width;
		GLuint height;
		GLfloat vertices[12];
		GLuint indices[6];
		GLfloat texCoords[8];
		GLfloat colors[12];

		int id;

		std::string vShader;
		std::string fShader;
	
		GLuint shaderId;
		GLuint vboId;
		GLuint iboId;
		
		GLuint fbo;
		GLuint tex;
		
		//Window the panel is attached to 
		GLFWwindow *context = 0;
				
		Panel(GLuint,GLuint,GLuint,GLuint,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");

		Panel(GLuint,GLuint,GLuint,GLuint,GLFWwindow*,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");

		~Panel();
	
		void draw();
		void setTexture(GLuint);
		void genTexture(int,int);
		void setPos(GLuint,GLuint);
		void setSize(GLuint,GLuint);
		void flipY();
		void flipX();
		void setColor(float,float,float);
		void changeContext(GLFWwindow*);
		void setShaders(std::string,std::string);
		bool checkBoundingBox(int,int);
		virtual void clickAction(int,int);
};

