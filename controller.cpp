#include <iostream>
#include <GLFW/glfw3.h>
#include "controller.h"

using namespace std;

/*Button Mappings:
	0 A
	1 B
	2 Select
	3 Start
	4	Up
	5 Down
	6	Left
	7 Right
*/
controller::controller(){
	//Init
	buttons_p1 = 0;
	nextButton_p1 = 0;
	buttons_p2 = 0;
	nextButton_p2 = 0;
}

//0=P1
//1=P2

//Store controller values
void controller::pollController(int value, int contrlNum){
		
		if (value & 0b00000001){
			if (contrlNum == 0)
				nextButton_p1 = buttons_p1;
			else
				nextButton_p2 = buttons_p2;
		}
}

//Read stored controller values
int controller::readController(int contrlNum){
	//Get button value
	int tmp = 0x40 + (((contrlNum)?nextButton_p2:nextButton_p1) & 0b00000001);
	//Load next button
	if (contrlNum == 0)
		nextButton_p1 >>= 1;
	else
		nextButton_p2 >>= 1;
	//Return button value
	return tmp;
}
