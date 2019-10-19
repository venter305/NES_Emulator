#ifndef PANEL_H_
#define PANEL_H_

#include <GL/glew.h>
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
	
		GLuint shaderId;
		GLuint vboId;
		GLuint iboId;

		GLuint tex;
				
		Panel(GLuint,GLuint,GLuint,GLuint,std::string="./GUI/panelVertShader",std::string="./GUI/panelFragShader");
		void draw();
		void setTexture(GLuint);
		void setPos(GLuint,GLuint);
		void setSize(GLuint,GLuint);
		void flipY();
		void flipX();
		void setColor(float,float,float);
};

#endif
