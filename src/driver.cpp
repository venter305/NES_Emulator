#include <iostream>
#include "GraphicsEngine/graphicsEngine.h"
#include "Enums.h"
#include "Macros.h"
#include "NES.h"
#include "SoundManager/soundManager.h"
#include "Windows/OpenRomWindow.h"
#include "Windows/DebuggingWindow.h"
#include "Windows/MainWindow.h"


const int APP_WIDTH = 256*2;
const int APP_HEIGHT = 240*2+25;

NES n;
bool paused = false;

uint16_t soundLoop (){
	while(!paused && !n.clock(1)){};
	// if (paused)
	// 	std::cout << '\r' << n.peekMemory(0x0020) << std::flush;
	float tmpOut = (float)n.APU.output*0.75f;
	return (tmpOut/0xFF)*0xFFFF;
}

int main(int argc, char** argv) {
	std::string romName = "";
	if (argc > 1)
		romName += argv[1];
	else
		romName += "Roms/SMB.nes";

	//Read Rom
	if (!n.CART.LoadRom(romName)){
		std::cout << "No Valid Rom" << std::endl;
		return 0;
	}

	n.reset();

	SoundManager soundMan("Nes Emulator");
	soundMan.UserFunc = soundLoop;

	//Open Window
	GraphicsEngine::Init();

	glfwSwapInterval(0);
	GraphicsEngine::AddWindow(new MainWindow(APP_WIDTH,APP_HEIGHT,"Nes Emulator"));
	GraphicsEngine::Run();
	//
	 glfwTerminate();
	return 0;
}
