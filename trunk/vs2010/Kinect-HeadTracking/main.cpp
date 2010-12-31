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
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600


/* Globals */
static int win;
xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator image;
XnStatus nRetVal;
GLuint texture_rgb, texture_depth;
GLubyte aDepthMap[640*480];

int fx = 0;
int fy = 0;
int cx = 0;
int cy = 0;
int ox = 0;
int oy = 0;
bool lp = false;


bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		cout << message << ": " << xnGetStatusString(nRetVal) << endl;
		return false;
	}
	return true;
}

void glut_mouse_motion(int x, int y) {
	if(lp) {
		cx = x-fx+ox;
		cy = fy-y+oy;
	}
}

void glut_mouse(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
		if(state==GLUT_DOWN) {
			lp = true;
			fx = x;
			fy = y;
		}
		else {
			lp = false;
			ox = cx;
			oy = cy;
		}
	}
}

void glut_idle() {
	glutPostRedisplay();
}

float rot_angle=0;
void glut_display() {
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
	ofstream datei;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, WINDOW_SIZE_X/WINDOW_SIZE_Y, 0, 5000);
//	glOrtho(0, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, -128, 128);

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
	const unsigned int xres = pDepthMapMD.XRes();
	const unsigned int yres = pDepthMapMD.YRes();
//	datei.open("daniel.txt", ios::out);

	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<xres; x++) {
			aDepthMap[x+y*xres] = static_cast<GLubyte>(static_cast<float>(pDepthMap[x+y*xres])/static_cast<float>(maxdepth)*255);
//			datei << x << " "  << y << " " << pDepthMap[x+y*xres] << " " << static_cast<int>(pImageMap[x+y*xres].nRed) << " " << static_cast<int>(pImageMap[x+y*xres].nGreen) << " " << static_cast<int>(pImageMap[x+y*xres].nBlue) << endl;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-800, 0, -2000);
	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageMap);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1); glVertex3f(0,0,0);
		glTexCoord2f(1,1); glVertex3f(640,0,0);
		glTexCoord2f(1,0); glVertex3f(640,480,0);
		glTexCoord2f(0,0); glVertex3f(0,480,0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-800, -600, -2000);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, 640, 480, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, aDepthMap);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1); glVertex3f(0,0,0);
		glTexCoord2f(1,1); glVertex3f(640,0,0);
		glTexCoord2f(1,0); glVertex3f(640,480,0);
		glTexCoord2f(0,0); glVertex3f(0,480,0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(50, -200, -3000);
	glRotatef(cx,0,1,0);
	glRotatef(cy,1,0,0);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glBegin(GL_POINTS);
	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<630; x++) {
			if(pDepthMap[x+y*xres]>=100) {
				glTexCoord2f(static_cast<float>(x)/static_cast<float>(630), static_cast<float>(y)/static_cast<float>(480)); 
				glVertex3f(x, (yres-y), (1020-static_cast<float>(pDepthMap[x+y*xres])/maxdepth*1020));
			}
		}
	}
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
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
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutInitWindowPosition(300,150);
	win = glutCreateWindow("kinect-head-tracking");
	glClearColor(0, 0, 0, 0.0); //Hintergrundfarbe: Hier ein leichtes Blau
//	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
//	glEnable(GL_CULL_FACE);           //Backface Culling aktivieren
//	glEnable(GL_ALPHA_TEST);
//	glAlphaFunc(GL_GEQUAL, 1);

	/* Texturen */
	glGenTextures(1, &texture_rgb);
	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texture_depth);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_mouse_motion);
	glutMainLoop();
	return 0;
}