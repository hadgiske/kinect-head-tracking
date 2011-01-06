#include "KProgram.h"

int KProgram::mWindowHandle = 0;
kKinect KProgram::kinect = kKinect();

KProgram::KProgram(void)
{
}

KProgram::~KProgram(void)
{
}

void KProgram::initGlut(int argc, char* argv[]){
	
	// Initalize Glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	// Set window size and position
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutInitWindowPosition(WINDOW_POS_X,WINDOW_POS_Y);

	// Create the window and save the handle
	KProgram::mWindowHandle = glutCreateWindow(WINDOW_TITLE);

	// Setting the Clear-Color
	glClearColor(WINDOW_CLEAR_COLOR);

	// GL-ENABLES:
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	// Setting the glut-funcs
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);	
	glutMouseFunc(KGlutInput::glutMouse);
	glutKeyboardFunc(KGlutInput::glutKeyboard);
	glutMotionFunc(KGlutInput::glutMouseMotion);
}

KHeadTrack KProgram::headtrack(SCREEN_HEIGTH_MM);

void KProgram::glutDisplay(void)
{
	// First delete the old scene
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Then paint the new scene
	headtrack.renderScene();

	// Done painting --> now show result
	glutSwapBuffers();
}


void KProgram::glutIdle(void)
{
	// First do idle-stuff:
	KVertex position = kinect.getPosition();
	headtrack.refreshData(position.mX,position.mY,position.mZ);

	// Done idle --> now repaint the window
	glutPostRedisplay();
}


void KProgram::showWindow(void){
	glutMainLoop();
}
