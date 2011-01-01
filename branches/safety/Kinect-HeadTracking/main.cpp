// Allgemein
#include <iostream>
#include <string>
// Glut
#include <GL/glut.h>
// OpenNI
#include <XnCppWrapper.h>

#include <fstream>
#include <cmath>


using namespace std;

/* Defines */
#define CONFIG_XML_FILE "xml/config.xml"

/* Globals */
static int win;
xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator image;
XnStatus nRetVal;



bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		cout << message << ": " << xnGetStatusString(nRetVal) << endl;
		return false;
	}
	return true;
}

void glut_mouse(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
//		rotieren = 1;
	}
}

void glut_idle() {
	glutPostRedisplay();
}

float rot_angle=0;
void glut_display() {
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
//	ofstream datei;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, 640/480, 0, 5000);
//	glOrtho(0, 640, 480, 0, -128, 128);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	
	rot_angle+=0.7;

	// Warten auf neue Daten vom Tiefengenerator
	nRetVal = context.WaitAndUpdateAll();
	checkError("Fehler beim Aktualisieren der Daten", nRetVal);

	// Aktuelle Depth Metadaten auslesen
	depth.GetMetaData(pDepthMapMD);
	// Aktuelle Depthmap auslesen
	const XnDepthPixel* pDepthMap = depth.GetDepthMap();

	// Aktuelle Image Metadaten auslesen 
	image.GetMetaData(pImageMapMD);
	//Aktuelles Bild auslesen
	const XnRGB24Pixel* pImageMap = image.GetRGB24ImageMap();

	glColor3f(1, 1, 1);
	XnDepthPixel maxdepth = depth.GetDeviceMaxDepth();
	float farbe;
	float alphakanal;
	int x2 = 0;
	int y2 = 0;
	const unsigned int xres = pDepthMapMD.XRes();
	const unsigned int yres = pDepthMapMD.YRes();

//	datei.open("ausgabe_raw.txt", ios::out);
	glTranslatef(0, 0, -2000);
	glRotatef(-30, 0, 1, 0);
	glTranslatef(300, -200, -50);
	for(unsigned int y=0; y<yres-1; y++) {
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int x=0; x<xres; x++) {
//			farbe = static_cast<float>(pDepthMap[x+y*(xres)])/static_cast<float>(maxdepth);
//			glColor3f(farbe, farbe, farbe);
//			glVertex3f(x, (yres-y), (1020-static_cast<float>(pDepthMap[x+y*xres])/maxdepth*1020));
//			datei << pDepthMap[x+y*xres] << endl;
//			glVertex3f(x, (yres-y), x%100);
			if(pDepthMap[x+y*xres]<=200 || pDepthMap[x+(y+1)*xres]>=1800)
				alphakanal = 0;
			else
				alphakanal = 1;

//  			glColor4f(static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth),
//  				static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth),
//  				static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth), alphakanal);

			glColor4f(static_cast<float>(pImageMap[x+y+y*(pImageMapMD.XRes()-1)].nRed)/static_cast<float>(255),
				static_cast<float>(pImageMap[x+y+y*(pImageMapMD.XRes()-1)].nGreen)/static_cast<float>(255),
				static_cast<float>(pImageMap[x+y+y*(pImageMapMD.XRes()-1)].nBlue)/static_cast<float>(255),alphakanal);
			//glVertex3f(x, (yres-y), -pDepthMap[x+y*xres]);
			glVertex3f(x, (yres-y), (1020-static_cast<float>(pDepthMap[x+y*xres])/maxdepth*1020));

//			farbe = static_cast<float>(pDepthMap[x+(y+1)*(xres)])/static_cast<float>(maxdepth);
//			glColor3f(farbe, farbe, farbe);
//			glVertex3f(x, (yres-y-1), (1020-static_cast<float>(pDepthMap[x+(y+1)*xres])/maxdepth*1020));
//			glVertex3f(x, (yres-y-1), x%100);

// 			glColor3f(static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nRed)/static_cast<float>(255),
// 				static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nGreen)/static_cast<float>(255),
// 				static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nBlue)/static_cast<float>(255));

		


			if(pDepthMap[x+(y+1)*xres]<=200 || pDepthMap[x+(y+1)*xres]>=1800 )
				alphakanal = 0;
			else
				alphakanal = 1;

//  			glColor4f(static_cast<float>(pDepthMap[x+(y+1)*(pDepthMapMD.XRes())])/static_cast<float>(maxdepth),
//  				static_cast<float>(pDepthMap[x+(y+1)*(pDepthMapMD.XRes())])/static_cast<float>(maxdepth),
//  				static_cast<float>(pDepthMap[x+(y+1)*(pDepthMapMD.XRes())])/static_cast<float>(maxdepth), alphakanal);

			glColor4f(static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nRed)/static_cast<float>(255),
				static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nGreen)/static_cast<float>(255),
				static_cast<float>(pImageMap[x+(y+1)*(pImageMapMD.XRes())].nBlue)/static_cast<float>(255),alphakanal);
			//glVertex3f(x, (yres-y-1), -pDepthMap[x+(y+1)*xres]);
			glVertex3f(x, (yres-y-1), (1020-static_cast<float>(pDepthMap[x+(y+1)*xres])/maxdepth*1020));
		}
		glEnd();
	}

	/*			glColor3f(static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth),
				static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth),
				static_cast<float>(pDepthMap[x+y+y*(pDepthMapMD.XRes()-1)])/static_cast<float>(maxdepth));	// DEPTHMAP */

	/*			
*/

	glutSwapBuffers();
//	datei.close();
//	exit(-1);
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
	XnMapOutputMode outputModeDepth;
	outputModeDepth.nXRes = 640;
	outputModeDepth.nYRes = 480;
	outputModeDepth.nFPS = 30;
	nRetVal = depth.SetMapOutputMode(outputModeDepth);
	checkError("Fehler beim Konfigurieren des Tiefengenerators", nRetVal)?0:exit(-1);


	/* Imagegenerator erstellen */
	nRetVal = image.Create(context);
	checkError("Fehler beim Erstellen des Bildgenerators", nRetVal)?0:exit(-1);

	/* Imagegenerator einstellen */
	XnMapOutputMode outputModeImage;
	outputModeImage.nXRes = 640;
	outputModeImage.nYRes = 480;
	outputModeImage.nFPS = 30;
	nRetVal = image.SetMapOutputMode(outputModeImage);
	checkError("Fehler beim Konfigurieren des Bildgenerators", nRetVal)?0:exit(-1);	


	/* Starten der Generatoren - volle Kraft vorraus! */
	nRetVal = context.StartGeneratingAll();
	checkError("Fehler beim Starten der Generatoren", nRetVal)?0:exit(-1);

	/* Glut initialisieren */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(300,150);
	win = glutCreateWindow("Daniels und Marcels 3D Welt");
	glClearColor(0.3, 0.4, 0.7, 0.0); //Hintergrundfarbe: Hier ein leichtes Blau
//	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
//	glEnable(GL_CULL_FACE);           //Backface Culling aktivieren
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 1);


	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMouseFunc(glut_mouse);
	glutMainLoop();
	return 0;
}