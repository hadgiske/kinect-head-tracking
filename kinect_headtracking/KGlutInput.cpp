#include "KGlutInput.h"


KGlutInput::KGlutInput(void)
{
}


KGlutInput::~KGlutInput(void)
{
}

int KGlutInput::ButtonPressed_x = 0;
int KGlutInput::ButtonPressed_y = 0;
int KGlutInput::Delta_x = 0;
int KGlutInput::Delta_y = 0;
int KGlutInput::OldDelta_x = 0;
int KGlutInput::OldDelta_y = 0;
bool KGlutInput::ButtonPressed = false;

void KGlutInput::glutMouse(int button, int state, int x , int y){
	if(button==GLUT_LEFT_BUTTON) {
		if(state==GLUT_DOWN) {
			ButtonPressed = true;
			ButtonPressed_x = x;
			ButtonPressed_y = y;
		}
		else {
			ButtonPressed = false;
			OldDelta_x = Delta_x;
			OldDelta_y = Delta_y;
		}
	}
}


void KGlutInput::glutKeyboard(unsigned char key, int x, int y){

}


void KGlutInput::glutMouseMotion(int x, int y){
	if(ButtonPressed) {
		Delta_x = x-ButtonPressed_x+OldDelta_x;
		Delta_y = ButtonPressed_y-y+OldDelta_y;
	}
}


int KGlutInput::getMouseDeltaX(){
	return Delta_x;	
}


int KGlutInput::getMouseDeltaY(){
	return Delta_y;	
}
