#pragma once
#include <gl/glut.h>



class KGlutInput
{
private:
	KGlutInput(void);
	~KGlutInput(void);

public:
	static void glutMouse(int button, int state, int x , int y);
	static void glutKeyboard(unsigned char key, int x, int y);
	static void glutMouseMotion(int x, int y);
	static int getMouseDeltaX();
	static int getMouseDeltaY();

private:
	static int ButtonPressed_x;
	static int ButtonPressed_y;
	static int Delta_x;
	static int Delta_y;
	static int OldDelta_x;
	static int OldDelta_y;
	static bool ButtonPressed;
};

