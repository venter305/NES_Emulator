#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "GUIElement.h"

class Panel : public GUIElement{
	public:
		int xPos;
		int yPos;
		int width;
		int height;
		static GLfloat vertices[];

		static GLuint indices[];

		GLfloat color[4];

		//nt id;

		std::string vShader;
		std::string fShader;

		GLuint shaderId;
		GLuint vboId;
		GLuint iboId;

		GLuint fbo;
		GLuint tex = -1;

		//Window the panel is attached to
		GLFWwindow *context = 0;

		Panel(GLuint,GLuint,GLuint,GLuint,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");

		Panel(GLuint,GLuint,GLuint,GLuint,GLFWwindow*,std::string="./GraphicsEngine/GUI/panelVertShader",std::string="./GraphicsEngine/GUI/panelFragShader");

		~Panel();

		virtual void draw();

		void setTexture(GLuint);
		void CreateTexture(int width,int height,GLenum format,GLenum type, void* pixels);
		void UpdateTexture(int xOffset,int yOffset,int width,int height,GLenum format,GLenum type,void* pixels);

		void setPos(GLuint,GLuint);
		void setSize(GLuint,GLuint);
		void setColor(float,float,float,float);
		void flipY();
		void flipX();

		void changeContext(GLFWwindow*);
		void setShaders(std::string,std::string);
		bool checkBoundingBox(int,int);
		virtual void clickAction(int,int);

private:
		void SetMVP();

};
