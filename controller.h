#ifndef CONTROLLER_H_
#define CONTROLLER_H_

class controller{

	public:
	 unsigned	char buttons_p1,buttons_p2;
	 unsigned char nextButton_p1,nextButton_p2;
		controller();
		void pollController(int,int);
		int readController(int);

};
#endif
