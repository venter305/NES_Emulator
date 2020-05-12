#include "panel.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Panel::Panel(GLuint x, GLuint y, GLuint w, GLuint h, GLFWwindow *window,string vsName,string fsName) : context(window){
	GLint windowData[4];
	glGetIntegerv(GL_VIEWPORT,windowData);
	
	//Init
	xPos = x;
	yPos = y;
	width = w;
	height = h;
	vShader = vsName;
	fShader = fsName;
	if (!context)
		context = glfwGetCurrentContext();
	
	//Geometry
	float xVal = ((windowData[0]+xPos)-(windowData[2]/2.0))/(windowData[2]/2.0);
	float yVal = ((windowData[1]+yPos)-(windowData[3]/2.0))/(windowData[3]/2.0);
	float xOffset = (width/(windowData[2]/2.0));
	float yOffset = (height/(windowData[3]/2.0));

	vertices[0] = xVal;
	vertices[1] = yVal+yOffset;
	vertices[2] = 0;
	vertices[3] = xVal;
	vertices[4] = yVal;
	vertices[5] = 0;
	vertices[6] = xVal+xOffset;
	vertices[7] = yVal;
	vertices[8] = 0;
	vertices[9] = xVal+xOffset;
	vertices[10] = yVal+yOffset;
	vertices[11] = 0;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4]= 3;
	indices[5] = 0;
	
	texCoords[0] = 0;
	texCoords[1] = 1;
	texCoords[2] = 0;
	texCoords[3] = 0;
	texCoords[4] = 1;
	texCoords[5] = 0;
	texCoords[6] = 1;
	texCoords[7] = 1;

	colors[0] = 1;
	colors[1] = 1;
	colors[2] = 1;
	colors[3] = 1;
	colors[4] = 1;
	colors[5] = 1;
	colors[6] = 1;
	colors[7] = 1;
	colors[8] = 1;
	colors[9] = 1;
	colors[10] = 1;
	colors[11] = 1;
	
	//Setup Context specific values
	changeContext(context);
}
	
Panel::Panel(GLuint x, GLuint y, GLuint w, GLuint h,string vsName,string fsName) : Panel(x,y,w,h,NULL,vsName,fsName){
}

Panel::~Panel(){
	/*glDeleteTextures(1,&tex);
	glDeleteFramebuffers(1,&fbo);
	glDeleteBuffers(1,&vboId);
	glDeleteBuffers(1,&iboId);
  glDeleteProgram(shaderId);*/
}

//Draw the Panel
void Panel::draw(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	glUseProgram(shaderId);
	
	int currTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexture);

	glBindTexture(GL_TEXTURE_2D,tex);
	glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,iboId);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0,3,GL_FLOAT,false,0,(void*)0);
	glVertexAttribPointer(1,3,GL_FLOAT,false,0,(void*)(sizeof(GLfloat)*12));
	glVertexAttribPointer(2,2,GL_FLOAT,false,0,(void*)(sizeof(GLfloat)*24));

	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindTexture(GL_TEXTURE_2D,currTexture);

	glUseProgram(0);
	glfwMakeContextCurrent(currWindow);
}

//Set the texture
void Panel::setTexture(GLuint texture){
		tex = texture;
}

void Panel::genTexture(int width, int height){
	glGenFramebuffers(1,&fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
	glBindTexture(GL_TEXTURE_2D,0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

//Set the position
void Panel::setPos(GLuint x,GLuint y){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	GLint windowData[4];
	glGetIntegerv(GL_VIEWPORT,windowData);
	xPos = x;
	yPos = y;
	
	//Set the vertices
	float xVal = ((windowData[0]+xPos)-(windowData[2]/2.0))/(windowData[2]/2.0);
	float yVal = ((windowData[1]+yPos)-(windowData[3]/2.0))/(windowData[3]/2.0);
	float xOffset = (width/(windowData[2]/2.0));
	float yOffset = (height/(windowData[3]/2.0));
	vertices[0] = xVal;
	vertices[1] = yVal+yOffset;
	vertices[3] = xVal;
	vertices[4] = yVal;
	vertices[6] = xVal+xOffset;
	vertices[7] = yVal;
	vertices[9] = xVal+xOffset;
	vertices[10] = yVal+yOffset;
	glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*12,vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwMakeContextCurrent(currWindow);
}

//Set the size
void Panel::setSize(GLuint w, GLuint h){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	GLint windowData[4];
	glGetIntegerv(GL_VIEWPORT,windowData);
	width = w;
	height = h;
	//Set the vertices
	float xVal = ((windowData[0]+xPos)-(windowData[2]/2.0))/(windowData[2]/2.0);
	float yVal = ((windowData[1]+yPos)-(windowData[3]/2.0))/(windowData[3]/2.0);
	float xOffset = (width/(windowData[2]/2.0));
	float yOffset = (height/(windowData[3]/2.0));
	vertices[0] = xVal;
	vertices[1] = yVal+yOffset;
	vertices[3] = xVal;
	vertices[4] = yVal;
	vertices[6] = xVal+xOffset;
	vertices[7] = yVal;
	vertices[9] = xVal+xOffset;
	vertices[10] = yVal+yOffset;
	glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*12,vertices);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glfwMakeContextCurrent(currWindow);
}

//Flip along the X axis
void Panel::flipX(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	GLfloat tmpVert[12];
	tmpVert[0] = vertices[9];
	tmpVert[1] = vertices[10];
	tmpVert[2] = 0;
	tmpVert[3] = vertices[6];
	tmpVert[4] = vertices[7];
	tmpVert[5] = 0;
	tmpVert[6] = vertices[3];
	tmpVert[7] = vertices[4];
	tmpVert[8] = 0;
	tmpVert[9] = vertices[0];
	tmpVert[10] = vertices[1];
	tmpVert[11] = 0;
	for (int i=0;i<12;i++)
		vertices[i] = tmpVert[i];
	glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*12,vertices);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glfwMakeContextCurrent(currWindow);
}

//Flip along the Y axis
void Panel::flipY(){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	GLfloat tmpVert[12];
	tmpVert[0] = vertices[3];
	tmpVert[1] = vertices[4];
	tmpVert[2] = 0;
	tmpVert[3] = vertices[0];
	tmpVert[4] = vertices[1];
	tmpVert[5] = 0;
	tmpVert[6] = vertices[9];
	tmpVert[7] = vertices[10];
	tmpVert[8] = 0;
	tmpVert[9] = vertices[6];
	tmpVert[10] = vertices[7];
	tmpVert[11] = 0;
	for (int i=0;i<12;i++)
		vertices[i] = tmpVert[i];
		glBindBuffer(GL_ARRAY_BUFFER,vboId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*12,vertices);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glfwMakeContextCurrent(currWindow);
}

//Set the Color tint of the texture
void Panel::setColor(float c1,float c2,float c3){
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(context);
	colors[0] = c1;
	colors[1] = c2;
	colors[2] = c3;
	colors[3] = c1;
	colors[4] = c2;
	colors[5] = c3;
	colors[6] = c1;
	colors[7] = c2;
	colors[8] = c3;
	colors[9] = c1;
	colors[10] = c2;
	colors[11] = c3;
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sizeof(GLfloat)*12,colors);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glfwMakeContextCurrent(currWindow);
}

void Panel::changeContext(GLFWwindow *window){
	context = window;
	GLFWwindow *currWindow = glfwGetCurrentContext();
	glfwMakeContextCurrent(window);
	
	//Shaders
	setShaders(vShader,fShader);

	//Generate Buffers
	glGenBuffers(1,&vboId);
	glGenBuffers(1,&iboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat)*32), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*12,vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sizeof(GLfloat)*12,colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*24, sizeof(GLfloat)*8,texCoords);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Texture
	int currFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currFramebuffer);
	int currTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexture);
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
	glBindTexture(GL_TEXTURE_2D,currTexture);

	glColor3f(1,1,1);
	glRecti(0,0,width,height);
	glBindFramebuffer(GL_FRAMEBUFFER,currFramebuffer);
	
	glfwMakeContextCurrent(currWindow);
}

void Panel::setShaders(string vsName, string fsName){
	//Shaders
	ifstream vsFile;
	ifstream fsFile;
	
	vsFile.open(vsName);
	fsFile.open(fsName);
	stringstream vsStream, fsStream;
	
	vsStream << vsFile.rdbuf();
	fsStream << fsFile.rdbuf();
	
	vsFile.close();
	fsFile.close();
	
	string vShader = vsStream.str();
	string fShader = fsStream.str();
	
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
