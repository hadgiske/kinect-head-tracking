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
xn::DepthGenerator depth;
XnStatus nRetVal;


bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		cout << message << ": " << xnGetStatusString(nRetVal) << endl;
		return false;
	}
	return true;
}

void glut_idle() {
	glutPostRedisplay();
}

void glut_display() {
	xn::DepthMetaData pDepthMapMD;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
//	gluPerspective(45, 640/480, 1, 1000);
	glOrtho(0, 640, 480, 0, -128, 128);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(1.5, 0, -5);


	// Warten auf neue Daten vom Tiefengenerator
	nRetVal = context.WaitOneUpdateAll(depth);
	checkError("Fehler beim Aktualisieren der Daten", nRetVal);

	// Aktuelle Metadaten auslesen
	depth.GetMetaData(pDepthMapMD);
	// Aktuelle Depthmap auslesen
	const XnDepthPixel* pDepthMap = depth.GetDepthMap();

	XnDepthPixel maxdepth = depth.GetDeviceMaxDepth();
	glBegin(GL_POINTS);
	for(unsigned int y=0; y<pDepthMapMD.YRes(); y++) {
		for(unsigned int x=0; x<pDepthMapMD.XRes(); x++) {
			glColor3f(static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth), 0, 0);
			glVertex3f(pDepthMapMD.XRes()-x-1,y,0);		
		}
	}
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char **argv) {
	nRetVal = XN_STATUS_OK;

	/* Context initialisieren (Kameradaten) */
	nRetVal = context.Init();
	checkError("Fehler beim Initialisieren des Context", nRetVal)?0:exit(-1);



	/* Tiefengenerator erstellen */
	nRetVal = depth.Create(context);
	checkError("Fehler beim Erstellen des Tiefengenerators", nRetVal)?0:exit(-1);

	/* Tiefengenerator einstellen */
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
	glutInitWindowPosition(300,150);
	win = glutCreateWindow("Daniels und Marcels 3D Welt");
	glClearColor(0.3, 0.4, 0.7, 0.0); //Hintergrundfarbe: Hier ein leichtes Blau
	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
	glEnable(GL_CULL_FACE);           //Backface Culling aktivieren


	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMainLoop();
	return 0;
}