// Allgemein
#include <iostream>
#include <string>
// Glut
#include <GL/glut.h>
// OpenNI
#include <XnCppWrapper.h>

using namespace std;

/* Defines */
#define CONFIG_XML_FILE "xml/config.xml"

/* Globals */
static int win;
xn::Context context;



bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		cout << message << ": " << xnGetStatusString(nRetVal) << endl;
//		printf("%s: %s\n", message, xnGetStatusString(nRetVal));
		return false;
	}
	return true;
}

void glut_idle() {
	glutPostRedisplay();
}

void glut_display() {

	// Warten auf neue Daten vom Tiefengenerator
//	nRetVal = context.WaitOneUpdateAll(depth);
//	if(!checkError("Fehler beim Aktualisieren der Daten", nRetVal))
//		continue;

	// Aktuelle Depthmap auslesen
//	const XnDepthPixel* pDepthMap = depth.GetDepthMap();


	// OpenGL
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, 640/480, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(1.5, 0, -5);
 	glColor3f(0.5, 0, 0);
	glutSolidCube(2);

	glutSwapBuffers();
}

int main(int argc, char **argv) {
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = context.Init();
	checkError("Fehler beim Initialisieren des Context", nRetVal)?0:exit(-1);

	xn::DepthGenerator depth;
	nRetVal = depth.Create(context);
	checkError("Fehler beim Erstellen des Tiefengenerators", nRetVal)?0:exit(-1);
	
	XnMapOutputMode outputMode;
	outputMode.nXRes = 640;
	outputMode.nYRes = 480;
	outputMode.nFPS = 30;
	nRetVal = depth.SetMapOutputMode(outputMode);
	checkError("Fehler beim Konfigurieren des Tiefengenerators", nRetVal)?0:exit(-1);

	nRetVal = context.StartGeneratingAll();
	checkError("Fehler beim Starten des Tiefengenerators", nRetVal)?0:exit(-1);

	/* Glut initialisieren */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	win = glutCreateWindow("Daniels und Marcels 3D Welt");
	glClearColor(0.3, 0.4, 0.7, 0.0); //Hintergrundfarbe: Hier ein leichtes Blau
	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
	glEnable(GL_CULL_FACE);           //Backface Culling aktivieren


	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMainLoop();
	return 0;
}