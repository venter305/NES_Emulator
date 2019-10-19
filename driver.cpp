#include <iostream>
#include 	<bitset>
//#include <GL/glut.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "NES.h"
#include "cpu.h"
#include <time.h>
#include <chrono>
#include <thread>
#include "./GUI/panel.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <sstream>
#include "./GUI/textObject.h"
#include "./GUI/button.h"
#include "./GUI/textInput.h"


using namespace std;

NES n;
vector<Button*> buttons;
string filePath;
bool paused = false;

void renderGame(Panel,GLuint,GLuint,GLuint,int);
void renderMenuBar(Panel,Button,GLuint);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
	switch (key){
		case GLFW_KEY_Z:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000001;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111110;
			break;
		case GLFW_KEY_X:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000010;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111101;
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00000100;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11111011;
			break;
		case GLFW_KEY_S:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00001000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11110111;
			break;
		case GLFW_KEY_UP:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00010000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11101111;
			break;
		case GLFW_KEY_DOWN:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b00100000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b11011111;
			break;
		case GLFW_KEY_LEFT:	
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b01000000;
			else if(action == GLFW_RELEASE)n.CONTRL.buttons_p1 &= 0b10111111;
			break;
		case GLFW_KEY_RIGHT:
			if (action == GLFW_PRESS) n.CONTRL.buttons_p1 |= 0b10000000;
			else if(action == GLFW_RELEASE) n.CONTRL.buttons_p1 &= 0b01111111;
			break;
	};
}

static void char_callback(GLFWwindow *window, unsigned int codepoint){
	filePath += (char)codepoint;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
			double xPos, yPos;
			glfwGetCursorPos(window,&xPos,&yPos);
			int windowX,windowY;
			glfwGetWindowSize(window,&windowX,&windowY);
			yPos = windowY-yPos;
    	for (Button *b : buttons){
				if (xPos >= b->xPos && xPos <= (b->xPos+b->width) && yPos >= b->yPos && yPos <= (b->yPos+b->height))
					b->action(b);
			}	
		}
}

void openButtonAction(Button *b){
	int dialogX = 300;
	int dialogY = 50;
	GLFWwindow *window = glfwCreateWindow(dialogX,dialogY,"Enter Rom",NULL, NULL);
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int windowPosX,windowPosY;
	glfwGetWindowPos(n.window,&windowPosX,&windowPosY);
	filePath = "";
	int windowX,windowY;
	glfwGetWindowSize(n.window,&windowX,&windowY);
	glfwSetWindowPos(window,windowPosX+((windowX-dialogX)/2),windowPosY+((windowY-dialogY)/2));

	GLuint fbo;
	GLuint tex;
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dialogX,dialogY,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex,0);
	glBindTexture(GL_TEXTURE_2D,0);

	int currViewport[4];
	glGetIntegerv(GL_VIEWPORT,currViewport);
	glViewport(0,0,dialogX,dialogY);
	
	
	glColor3f(.9,.9,.9);
	glRectf(-1,-1,1,1);
	Text label(0,(dialogY-15)/2,15,"Path to Rom:","/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
	label.flipY();
	label.draw();
	TextInput fileEntry(label.width,(dialogY-17)/2,dialogX-label.width,17);
	fileEntry.draw();

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
	
	Panel fileInput(0,0,dialogX,dialogY);
	fileInput.setTexture(tex);
	fileInput.draw();
	glfwSetCharCallback(window,char_callback);
	int deleteChar = true;
	while (!glfwGetKey(window,GLFW_KEY_ENTER)){
		if (glfwWindowShouldClose(window)){
			glfwDestroyWindow(window);
			glfwMakeContextCurrent(n.window);
			return;
		};
		glfwPollEvents();
		if (glfwGetKey(window,GLFW_KEY_BACKSPACE) && deleteChar){
			deleteChar = false;
			if (filePath.length() > 0)filePath.pop_back();
		}
		
		if (!glfwGetKey(window,GLFW_KEY_BACKSPACE))
			deleteChar = true;
		
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);
		glViewport(0,0,dialogX,dialogY);
		fileEntry.text->setText(filePath);
		fileEntry.drawText();
		fileEntry.draw();
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(currViewport[0],currViewport[1],currViewport[2],currViewport[3]);
		fileInput.draw();
		glFlush();
		glfwSwapBuffers(window);
	}
	glfwSetCharCallback(window,NULL);
	glfwMakeContextCurrent(n.window);
	glfwDestroyWindow(window);
	ifstream f;
	f.open(filePath);
	if (f){
		f.close();
		n.reset();
		n.CART.readRom(filePath);
		n.CPU.reset();
	}
}

void pauseButtonAction(Button *b){
	if (!paused)
		b->text->setText("Resume");
	else
		b->text->setText("Pause");
	b->drawText();
	b->draw();
	paused = !paused;
}

void saveStateAction(Button *b){
	ofstream file;
	string fileName = "./SaveStates/"+n.CART.name;
	fileName.erase(fileName.length()-4,4);
	file.open(fileName,ios::binary);
	char *x = new char[4];
	x[0] = n.CPU.a;
	file.write(x,1);
	x[0] = n.CPU.x;
	file.write(x,1);
	x[0] = n.CPU.y;
	file.write(x,1);
	x[0] = n.CPU.s;
	file.write(x,1);
	x[0] = n.CPU.p;
	file.write(x,1);
	x[0] = (n.CPU.pc&0xFF00)>>8;
	x[1] = n.CPU.pc&0xFF;
	file.write(x,2);
	for (int i=0;i<0x10000;i++){
		x[0] = n.memory[i];
		file.write(x,1);
	}
	x[0] = (n.PPU.cycles&0xFF00)>>8;
	x[1] = n.PPU.cycles&0xFF;
	file.write(x,2);
	x[0] = (n.PPU.scanlines&0xFF000000)>>24;
	x[1] = (n.PPU.scanlines&0xFF0000)>>16;
	x[2] = (n.PPU.scanlines&0xFF00)>>8;
	x[3] = n.PPU.scanlines&0xFF;
	file.write(x,sizeof(int));
	x[0] = (n.PPU.frames&0xFF000000)>>24;
	x[1] = (n.PPU.frames&0xFF0000)>>16;
	x[2] = (n.PPU.frames&0xFF00)>>8;
	x[3] = n.PPU.frames&0xFF;
	file.write(x,sizeof(int));
	x[0] = (n.PPU.v&0xFF00)>>8;
	x[1] = n.PPU.v&0xFF;
	file.write(x,2);
	x[0] = (n.PPU.t&0xFF00)>>8;
	x[1] = n.PPU.t&0xFF;
	file.write(x,2);
	x[0] = n.PPU.x;
	file.write(x,1);
	x[0] = n.PPU.w;
	file.write(x,1);
	x[0] = n.PPU.frameComplete;
	file.write(x,1);
	x[0] = n.PPU.s0Hit;
	file.write(x,1);
	x[0] = (n.PPU.currScanline&0xFF000000)>>24;
	x[1] = (n.PPU.currScanline&0xFF0000)>>16;
	x[2] = (n.PPU.currScanline&0xFF00)>>8;
	x[3] = n.PPU.currScanline&0xFF;
	file.write(x,sizeof(int));
	x[0] = n.PPU.vBlank;
	file.write(x,1);
	x[0] = (n.PPU.tmpScanline&0xFF000000)>>24;
	x[1] = (n.PPU.tmpScanline&0xFF0000)>>16;
	x[2] = (n.PPU.tmpScanline&0xFF00)>>8;
	x[3] = n.PPU.tmpScanline&0xFF;
	file.write(x,sizeof(int));
	for (int i=0;i<0x4000;i++){
		x[0] = n.PPU.memory[i];
		file.write(x,1);
	}
	for (int i=0;i<0x100;i++){
		x[0] = n.PPU.OAM[i];
		file.write(x,1);
	}
	for (int i=0;i<0x800;i++){
		x[0] = n.PPU.ntRAM[i];
		file.write(x,1);
	}
	x[0] = (n.PPU.bgShiftReg16[0]&0xFF00)>>8;
	x[1] = n.PPU.bgShiftReg16[0]&0xFF;
	file.write(x,2);
	x[0] = (n.PPU.bgShiftReg16[1]&0xFF00)>>8;
	x[1] = n.PPU.bgShiftReg16[1]&0xFF;
	file.write(x,2);
	x[0] = n.PPU.bgShiftReg8[0];
	file.write(x,1);
	x[0] = n.PPU.bgShiftReg8[1];
	file.write(x,1);
	x[0] = n.PPU.bgLatch[0];
	file.write(x,1);
	x[0] = n.PPU.bgLatch[1];
	file.write(x,1);

	for (int i=0;i<8;i++){
		x[0] = n.PPU.sShiftReg8_1[i];
		file.write(x,1);
		x[0] = n.PPU.sShiftReg8_2[i];
		file.write(x,1);
		x[0] = n.PPU.sLatch[i];
		file.write(x,1);
		x[0] = n.PPU.sCount[i];
		file.write(x,1);
	}

	for (int i=0;i<16;i++){
		x[0] = n.CART.header[i];
		file.write(x,1);
	}

	for (int i=0;i<n.CART.prg_size;i++){
		x[0] = n.CART.prgMemory[i];
		file.write(x,1);
	}

	for (int i=0;i<n.CART.chr_size;i++){
		x[0] = n.CART.chrMemory[i];
		file.write(x,1);
	}

	x[0] = n.CART.ntMirrorMode;
	file.write(x,1);

	n.CART.Mapper->saveMapState(&file,x);
	
	file.close();
}

void loadStateAction(Button *b){
	ifstream file;
	string fileName = "./SaveStates/"+n.CART.name;
	fileName.erase(fileName.length()-4,4);
	file.open(fileName,ios::binary);
	char *x = new char[4];
	x[0] = 0;
	x[1] = 0;
	x[2] = 0;
	x[3] = 0;
	file.read(x,1);
	n.CPU.a = (int)(unsigned char)x[0];
	file.read(x,1);
	n.CPU.x = (int)(unsigned char)x[0];
	file.read(x,1);
	n.CPU.y = (int)(unsigned char)x[0];
	file.read(x,1);
	n.CPU.s = (int)(unsigned char)x[0];
	file.read(x,1);
	n.CPU.p = (int)(unsigned char)x[0];
	file.read(x,2);
	n.CPU.pc = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	for (int i=0;i<0x10000;i++){
		file.read(x,1);
		n.memory[i] = (int)(unsigned char)x[0];
	}
	file.read(x,2);
	n.PPU.cycles = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,sizeof(int));
	n.PPU.scanlines = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,sizeof(int));
	n.PPU.frames = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,2);
	n.PPU.v = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,2);
	n.PPU.t = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,1);
	n.PPU.x = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.w = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.frameComplete = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.s0Hit = (int)(unsigned char)x[0];
	file.read(x,sizeof(int));
	n.PPU.currScanline = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	file.read(x,1);
	n.PPU.vBlank = (int)(unsigned char)x[0];
	file.read(x,sizeof(int));
	n.PPU.tmpScanline = (int)(unsigned char)x[0]*0x1000000+(int)(unsigned char)x[1]*0x10000+(int)(unsigned char)x[2]*0x100+(int)(unsigned char)x[3];
	for (int i=0;i<0x4000;i++){
		file.read(x,1);
		n.PPU.memory[i] = (int)(unsigned char)x[0];
	}
	for (int i=0;i<0x100;i++){
		file.read(x,1);
		n.PPU.OAM[i] = (int)(unsigned char)x[0];
	}
	for (int i=0;i<0x800;i++){
			file.read(x,1);
			n.PPU.ntRAM[i] = (int)(unsigned char)x[0];
	}
	file.read(x,2);
	n.PPU.bgShiftReg16[0] = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,2);
	n.PPU.bgShiftReg16[1] = (int)(unsigned char)x[0]*0x100+(int)(unsigned char)x[1];
	file.read(x,1);
	n.PPU.bgShiftReg8[0] = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.bgShiftReg8[1] = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.bgLatch[0] = (int)(unsigned char)x[0];
	file.read(x,1);
	n.PPU.bgLatch[1] = (int)(unsigned char)x[0];

	for (int i=0;i<8;i++){
		file.read(x,1);
		n.PPU.sShiftReg8_1[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		n.PPU.sShiftReg8_2[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		n.PPU.sLatch[i] = (int)(unsigned char)x[0];
		file.read(x,1);
		n.PPU.sCount[i] = (int)(unsigned char)x[0];
	}

	for (int i=0;i<16;i++){
		file.read(x,1);
		n.CART.header[i] = (int)(unsigned char)x[0];
	}
	
	for (int i=0;i<n.CART.prg_size;i++){
		file.read(x,1);
		n.CART.prgMemory[i] = (int)(unsigned char)x[0];
	}

	for (int i=0;i<n.CART.chr_size;i++){
		file.read(x,1);
		n.CART.chrMemory[i] = (int)(unsigned char)x[0];
	}

	file.read(x,1);
	n.CART.ntMirrorMode = (int)(unsigned char)x[0];

	n.CART.Mapper->loadMapState(&file,x);

	file.close();
}

int main(int argc, char** argv) {
	n.CART.readRom("SMB.nes");
	//n.CART.readRom("CASTLEVA.NES");
	//n.CART.readRom("zelda.nes");
	//n.CART.readRom("./ppu_sprite_hit/rom_singles/05-left_clip.nes");
	n.CPU.reset();
	float dTime = 0;	
	int x = 0;
	if (!glfwInit()){
		return 0;
	}
	
	GLFWwindow *window = glfwCreateWindow(512,500,"Nes Emulator",NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	n.window = window;
	glfwSetKeyCallback(window,key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	int y = 0;
	float elapsedTime = 0;
	float runTime = 0;
	int z = 0;
	int numFrames = 0;
	int currFrame = 0;
	
	GLuint vboId;
	GLuint iboId;
	int vertSize = (256*240)*4*3;
	GLfloat *vertices = new GLfloat[vertSize];
	GLfloat *colors = new GLfloat[vertSize];
	GLuint *indices = new GLuint[(256*240)*2*3];
	for (int i=0;i<240;i++)
		for (int j=0;j<256;j++){
			float posX = ((j)-128)/128.0;
			float posY = ((119.5-i)/119.5);
			float xOffset = (1.0/128);
			float yOffset = (1.0/119.5);
			int index = (i*256+j)*4*3;
			vertices[index] = posX;
			vertices[index+1] = posY;
			vertices[index+2] = 0;
			
			vertices[index+3] = posX;
			vertices[index+4] = posY+yOffset;
			vertices[index+5] = 0;

			vertices[index+6] = posX+xOffset;
			vertices[index+7] = posY+yOffset;
			vertices[index+8] = 0;
		
			vertices[index+9] = posX+xOffset;
			vertices[index+10] = posY;
			vertices[index+11] = 0;

			indices[(((i*256+j)*2)*3)] = ((i*256+j)*4);
			indices[(((i*256+j)*2)*3)+1] = ((i*256+j)*4)+1;
			indices[(((i*256+j)*2)*3)+2] = ((i*256+j)*4)+2;

			indices[(((i*256+j)*2)*3)+3] = ((i*256+j)*4)+2;
			indices[(((i*256+j)*2)*3)+4] = ((i*256+j)*4)+3;
			indices[(((i*256+j)*2)*3)+5] = ((i*256+j)*4);
		}

	glGenBuffers(1,&vboId);
	glGenBuffers(1,&iboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat)*vertSize)*2, 0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*vertSize,vertices);
	n.PPU.pixelVal = (float*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(256*240)*2*3, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
  GLuint fbo;
	glGenFramebuffers(1,&fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLuint texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,240,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture,0);
	glBindTexture(GL_TEXTURE_2D,0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	
	glViewport(0,0,512,500);

	Panel gameScreen(0,0,512,480);
	gameScreen.setTexture(texture);

	Panel menuBar(0,480,512,20);
	menuBar.setColor(.9,.9,.9);
	
	string fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf";	

	Button openButton(5,482,36,15,openButtonAction);
	openButton.text->setText("Open");
	openButton.setBackgroundColor(.9,.9,.9);
	openButton.drawText();
	buttons.push_back(&openButton);

	Button pauseButton(50,482,50,15, pauseButtonAction);
	pauseButton.text->setText("Pause");
	pauseButton.setBackgroundColor(.9,.9,.9);
	pauseButton.drawText();
	buttons.push_back(&pauseButton);	

	Button saveStateButton(110,482,70,15, saveStateAction);
	saveStateButton.text->setText("Save State");
	saveStateButton.setBackgroundColor(.9,.9,.9);
	saveStateButton.drawText();
	buttons.push_back(&saveStateButton);

	Button loadStateButton(200,482,70,15, loadStateAction);
	loadStateButton.text->setText("Load State");
	loadStateButton.setBackgroundColor(.9,.9,.9);
	loadStateButton.drawText();
	buttons.push_back(&loadStateButton);
	
	int frameSkipNum = 0;
	while(!glfwWindowShouldClose(window)){
		if (paused == false){
			float startTime = glfwGetTime();
			n.CPU.runInstructions();
		
			if (n.PPU.frameComplete == true){
				n.PPU.frameComplete = false;
				runTime = startTime;		
					glfwPollEvents();
				if (numFrames >= frameSkipNum){
					renderGame(gameScreen,fbo,vboId,iboId,vertSize);
					menuBar.draw();
					openButton.draw();
					pauseButton.draw();
					saveStateButton.draw();
					loadStateButton.draw();
					
		
					glFlush();
					glfwSwapBuffers(window);
					numFrames = 0;
					//cout << z++ << endl;
				}
				else
					numFrames++;
			}

			elapsedTime += (glfwGetTime()-startTime);
			y++;

			
		
			if (startTime-dTime >= 1){
				cout << n.PPU.frames-x << ' ' << elapsedTime/y*1000 << endl;
				elapsedTime = 0;
				x = n.PPU.frames;
				y = 0;
				z = 0;
				dTime = glfwGetTime();
			}
		}
		else 
			glfwPollEvents();
	}
	
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glfwTerminate();
	return 0;
}

void renderGame(Panel p, GLuint fbo, GLuint vboId, GLuint iboId, int vertSize){
	glViewport(0,0,256,240);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(3,GL_FLOAT,0,(void*)0);		
	glColorPointer(3,GL_FLOAT,0,(void*)(sizeof(GLfloat)*vertSize));

	glDrawElements(GL_TRIANGLES,(256*240)*2*3,GL_UNSIGNED_INT,(void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	
	glViewport(0,0,512,500);
	
	p.draw();
}

