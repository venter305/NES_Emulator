#include "panel.h"
#include "../glm/glm/glm.hpp"
#include "../glm/glm/ext/matrix_clip_space.hpp"
#include "../glm/glm/ext/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

GLfloat Panel::vertices[] = {
		 0.0f, 0.0f,0.0f,0.0f,
		 1.0f,0.0f,1.0f,0.0f,
		 1.0f, 1.0f,1.0f,1.0f,
		 0.0f, 1.0f,0.0f,1.0f,
		};

GLuint Panel::indices[] = {
			0,1,2,
			2,3,0
		};

Panel::Panel(GLuint x, GLuint y, GLuint w, GLuint h, GLFWwindow *window,std::string vsName,std::string fsName) : xPos(x),yPos(y),width(w),height(h),vShader(vsName),fShader(fsName), context(window){
	GLint windowData[4];
	glGetIntegerv(GL_VIEWPORT,windowData);

	//Init
	id = -1;
	//if (!context)
		//context = glfwGetCurrentContext();

	float pixels[3] = {1.0f,1.0f,1.0f};
	CreateTexture(1,1,GL_RGB,GL_FLOAT,pixels);
	//Setup Context specific values
	changeContext(context);

	setColor(1.0f,1.0f,1.0f,1.0f);
}

Panel::Panel(GLuint x, GLuint y, GLuint w, GLuint h,std::string vsName,std::string fsName) : Panel(x,y,w,h,NULL,vsName,fsName){
}

Panel::~Panel(){
	glDeleteTextures(1,&tex);
	glDeleteBuffers(1,&vboId);
	glDeleteBuffers(1,&iboId);
  glDeleteProgram(shaderId);
}

//Draw the Panel
void Panel::draw(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	//glfwMakeContextCurrent(context);
	glUseProgram(shaderId);

	glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,iboId);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D,tex);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0,2,GL_FLOAT,false,4*sizeof(GLfloat),(void*)0);
	glVertexAttribPointer(1,2,GL_FLOAT,false,4*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));

	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);


	//glfwMakeContextCurrent(currWindow);
}

//Set the texture
void Panel::setTexture(GLuint texture){
		glDeleteTextures(1,&tex);
		tex = texture;
}

void Panel::CreateTexture(int width, int height,GLenum format,GLenum type,void* pixels){

	glDeleteTextures(1,&tex);

	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,format,type,pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Panel::UpdateTexture(int xOffset,int yOffset,int width,int height,GLenum format,GLenum type,void* pixels){
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexSubImage2D(GL_TEXTURE_2D,xOffset,yOffset,0,width,height,format,type,pixels);
}

//Set the position
void Panel::setPos(GLuint x,GLuint y){
	xPos = x;
	yPos = y;
	SetMVP();
}

//Set the size
void Panel::setSize(GLuint w, GLuint h){
	width = w;
	height = h;
	SetMVP();
}


//Set the Color tint of the texture
void Panel::setColor(float r,float g,float b,float a){
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	glUseProgram(shaderId);

	int location = glGetUniformLocation(shaderId,"color");
	glUniform4f(location,color[0],color[1],color[2],color[3]);
}

void Panel::flipX(){
	xPos += width;
	width *= -1;
	SetMVP();
}

void Panel::flipY(){
	yPos += height;
	height *= -1;
	SetMVP();
}

void Panel::SetMVP(){
	int windowWidth,windowHeight;
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwGetWindowSize(currWindow,&windowWidth,&windowHeight);

	std::cout << windowWidth << ' ' << windowHeight << std::endl;

	glm::mat4 proj = glm::ortho(0.0f,(float)windowWidth,0.0f,(float)windowHeight,-1.0f,1.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3((float)xPos,(float)yPos,0.0f));
	model = glm::scale(model,glm::vec3((float)width,(float)height,1.0f));

	glm::mat4 mvp = proj*model;

	glUseProgram(shaderId);
	int location = glGetUniformLocation(shaderId,"mvp");
	glUniformMatrix4fv(location,1,GL_FALSE,&mvp[0][0]);
}

void Panel::changeContext(GLFWwindow *window){
	context = window;
	GLFWwindow *currWindow = glfwGetCurrentContext();
	//glfwMakeContextCurrent(window);

	//Shaders
	setShaders(vShader,fShader);

	//Set the Model,view,projection matrices
	SetMVP();

	//Generate Buffers
	glGenBuffers(1,&vboId);
	glGenBuffers(1,&iboId);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat)*16), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, indices, GL_STATIC_DRAW);

	//glfwMakeContextCurrent(currWindow);
}

void Panel::setShaders(std::string vsName, std::string fsName){
	//Shaders
	std::ifstream vsFile;
	std::ifstream fsFile;

	vsFile.open(vsName);
	fsFile.open(fsName);
	std::stringstream vsStream, fsStream;

	vsStream << vsFile.rdbuf();
	fsStream << fsFile.rdbuf();

	vsFile.close();
	fsFile.close();

	std::string vShader = vsStream.str();
	std::string fShader = fsStream.str();

	const char *vCode = vShader.c_str();
	const char *fCode = fShader.c_str();

	unsigned int vertex,fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex,1,&vCode,NULL);
	glCompileShader(vertex);


	int  success;
char infoLog[512];
glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if(!success)
{
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment,1,&fCode,NULL);
	glCompileShader(fragment);

glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success)
{
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
}

	shaderId = glCreateProgram();
	glAttachShader(shaderId,vertex);
	glAttachShader(shaderId,fragment);
	glLinkProgram(shaderId);

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
if(!success)
{
    glGetProgramInfoLog(shaderId, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

bool Panel::checkBoundingBox(int x, int y){
	return (x >= xPos && x <= (xPos+width) && y >= yPos && y <= (yPos+height));
}

void Panel::clickAction(int posX,int posY){
}
