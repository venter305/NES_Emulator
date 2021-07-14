#pragma once

#include <fstream>

class controller{

	public:
		enum Button : unsigned char {
			A = 		 0b00000001,
			B = 		 0b00000010,
			SELECT = 0b00000100,
			START =  0b00001000,
			UP = 		 0b00010000,
			DOWN = 	 0b00100000,
			LEFT = 	 0b01000000,
			RIGHT =  0b10000000,
			NONE =   0b00000000
		};

		struct ButtonMapping {
			int btn_A = -1;
			int btn_B = -1;
			int btn_SELECT = -1;
			int btn_START = -1;
			int btn_UP = -1;
			int btn_DOWN = -1;
			int btn_LEFT = -1;
			int btn_RIGHT = -1;

			Button GetButton(int btnMap){
				if (btnMap == btn_A)
					return A;
				if (btnMap == btn_B)
					return B;
				if (btnMap == btn_SELECT)
					return SELECT;
				if (btnMap == btn_START)
					return START;
				if (btnMap == btn_UP)
					return UP;
				if (btnMap == btn_DOWN)
					return DOWN;
				if (btnMap == btn_LEFT)
					return LEFT;
				if (btnMap == btn_RIGHT)
					return RIGHT;

				return NONE;
			}

		} buttonMapping, joystickMapping;
		unsigned char buttons_p1,buttons_p2;
		unsigned char nextButton_p1,nextButton_p2;
		controller();
		void pollController(int,int);
		int readController(int);

		void UpdateButton(int state, int button, int contrlNum = 0,bool joystick = false);

		void SaveState(std::ofstream file);
		void LoadState(std::ifstream file);

};
