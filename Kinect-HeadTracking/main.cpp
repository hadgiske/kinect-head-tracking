// Allgemein
#include <iostream>
#include <stdlib.h>
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
//#define DEBUGOUT

/* Structs */
struct HeadTransformation {
	int x;
	int y;
	int z;
	double rotmat[16];
} headtrans = { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

struct Mouse {
	int fx;			/* First click position */
	int fy;
	int cx;			/* Current position */
	int cy;
	int ox;			/* Last click position */
	int oy;
	bool left;
	bool right;
} mouse = { 0, 0, 0, 0, 0, 0, false, false };

/* Globals */
static int win;
xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator image;
xn::UserGenerator user;
XnStatus nRetVal;
GLuint texture_rgb, texture_depth;
GLubyte aDepthMap[640*480];
bool printFile = false;
bool calibration = false;
int maxdepth=-1;

float scalex=0.9, scaley=0.9, transx=41, transy=35;

bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		cout << message << ": " << xnGetStatusString(nRetVal) << endl;
		return false;
	}
	return true;
}

void clearScreen() {
#if defined(WIN32)
	system("cls");
#endif
#if defined(LINUX)
	system("clear");
#endif
}


void glut_mouse_motion(int x, int y) {
	if(mouse.left) {
		mouse.cx = x-mouse.fx+mouse.ox;
		mouse.cy = mouse.fy-y+mouse.oy;
	}
}

void glut_keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'q':
		scalex-=0.01;
		break;
	case 'a':
		scalex+=0.01;
		break;
	case 'w':
		scaley-=0.01;
		break;
	case 's': 
		scaley+=0.01;
		break;
	case 'e':
		transx++;
		break;
	case 'd':
		transx--;
		break;
	case 'r':
		transy++;
		break;
	case 'f':
		transy--;
		break;
	case 'p':
		printFile = true;
		break;
	case 'c':
		calibration = true;
		break;
	}

	cout << "Scale X: " << scalex << "\tScale Y: " << scaley << "\tTrans X: " << transx << "\tTrans Y: " << transy << endl;
}

void glut_mouse(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
		if(state==GLUT_DOWN) {
			mouse.left = true;
			mouse.fx = x;
			mouse.fy = y;
		}
		else {
			mouse.left = false;
			mouse.ox = mouse.cx;
			mouse.oy = mouse.cy;
		}
	}
}

void glut_idle() {
	glutPostRedisplay();
}

int getMaxDepth(const XnDepthPixel* depthmap, int size=640*480) {
	int max_tmp=0;
	for(int i=0; i<size; i++) {
		if(depthmap[i]>max_tmp)
			max_tmp=depthmap[i];
	}
	
	return max_tmp;
}

bool isCalStart=false;
bool isCalEnd=false;
void _stdcall skel_cal_start(xn::SkeletonCapability& skeleton, XnUserID user, void* pCookie) {
	if(isCalStart==false) {
		cout << "Pose wurde erkannt! Bitte warten, Kalibration wird durchgefuehrt!" << endl;
		isCalStart=true;
	}
	isCalEnd=false;
}

void _stdcall skel_cal_end(xn::SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess, void* pCookie) {
	if(isCalEnd==false) {
		cout << "Kalibration abgeschlossen!" << endl;
		isCalEnd=true;
	}
	isCalStart=false;
}

float rot_angle=0;
XnUInt16 pUserOld=0;
void glut_display() {
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
	XnUserID pUser[2];
	XnUInt16 nUsers=2;
#ifdef DEBUGOUT
	ofstream datei;
#endif

	pUser[0] = 0;
	pUser[1] = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, WINDOW_SIZE_X/WINDOW_SIZE_Y, 1, 5000);
//	glOrtho(0, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, -128, 128);

/*	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(scalex, scaley, 1.0);
	glTranslatef(transx/630, transy/480, 0.0);*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Warten auf neue Daten vom Tiefengenerator
	nRetVal = context.WaitAndUpdateAll();
	checkError("Fehler beim Aktualisieren der Daten", nRetVal);

	/* Anzahl der User auslesen und in Objekten speichern */
	user.GetUsers(pUser, nUsers);
	if(pUser[0]!=0 && pUserOld!=1) {
		cout << "User 1 erkannt" << endl;
		pUserOld=1;
	}

	xn::SkeletonCapability pSkeleton = user.GetSkeletonCap();
	XnCallbackHandle hnd;
	pSkeleton.RegisterCalibrationCallbacks(skel_cal_start, skel_cal_end, 0,hnd);
	
	if(calibration){
		pSkeleton.RequestCalibration(pUser[0],true);
		calibration = false;
		cout << "Kalibration wird gestartet, bitte Arme im 90 Grad Winkel nach oben halten." << endl;
	}

	pSkeleton.SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	if(pSkeleton.IsCalibrated(pUser[0])) {
		XnSkeletonJointTransformation head;
		pSkeleton.StartTracking(pUser[0]);
		pSkeleton.GetSkeletonJoint(pUser[0], XN_SKEL_HEAD, head);

		if(head.position.fConfidence && head.orientation.fConfidence) {
			headtrans.rotmat[0] = head.orientation.orientation.elements[0];
			headtrans.rotmat[1] = head.orientation.orientation.elements[1];
			headtrans.rotmat[2] = head.orientation.orientation.elements[2];
			headtrans.rotmat[3] = 0;
			
			headtrans.rotmat[4] = head.orientation.orientation.elements[3];
			headtrans.rotmat[5] = head.orientation.orientation.elements[4];
			headtrans.rotmat[6] = head.orientation.orientation.elements[5];
			headtrans.rotmat[7] = 0;
			
			headtrans.rotmat[8] = -head.orientation.orientation.elements[6];
			headtrans.rotmat[9] = -head.orientation.orientation.elements[7];
			headtrans.rotmat[10] =-head.orientation.orientation.elements[8];
			headtrans.rotmat[11] = 0;

			headtrans.rotmat[12] = 0;
			headtrans.rotmat[13] = 0;
			headtrans.rotmat[14] = 0;
			headtrans.rotmat[15] = 1;

			headtrans.x = head.position.position.X;
			headtrans.y = head.position.position.Y;
			headtrans.z = head.position.position.Z;
			
			 

			clearScreen();
			cout	<< "Confidence Position: " << head.position.fConfidence
					<< " X: " << head.position.position.X
					<< " Y: " << head.position.position.Y
					<< " Z: " << head.position.position.Z << endl

					<< "Confidence Rotation: " << head.orientation.fConfidence << endl
					<< "\t" << headtrans.rotmat[0]
					<< "\t" << headtrans.rotmat[4]
					<< "\t" << headtrans.rotmat[8] << endl
					<< "\t" << headtrans.rotmat[1]
					<< "\t" << headtrans.rotmat[5]
					<< "\t" << headtrans.rotmat[9] << endl
					<< "\t" << headtrans.rotmat[2]
					<< "\t" << headtrans.rotmat[6]
					<< "\t" << headtrans.rotmat[10] << endl << endl;
		}

		XnSkeletonJointPosition position;
		pSkeleton.GetSkeletonJointPosition(pUser[0],XN_SKEL_TORSO,position);
		glPushMatrix();
		glLoadIdentity();
		
		glTranslatef(-position.position.X, position.position.Y, -position.position.Z-50);

		cout	<< "Confidence Position: " << head.position.fConfidence
					<< " X: " << position.position.X
					<< " Y: " << position.position.Y
					<< " Z: " << position.position.Z << endl;


		
		
		glutSolidSphere(50, 20, 20);

		glPopMatrix();



	}

	glTranslatef(-headtrans.x, headtrans.y,-headtrans.z -50);
	glRotatef(mouse.cx,0,1,0);
	glRotatef(-mouse.cy,1,0,0);

	glMultMatrixd(headtrans.rotmat);
	glutSolidCube(50);


/*	// Aktuelle Depth Metadaten auslesen
	depth.GetMetaData(pDepthMapMD);
	// Aktuelle Depthmap auslesen
	const XnDepthPixel* pDepthMap = depth.GetDepthMap();

	if(maxdepth==-1)
		maxdepth = getMaxDepth(pDepthMap);

	// Aktuelle Image Metadaten auslesen 
	image.GetMetaData(pImageMapMD);
	//Aktuelles Bild auslesen
	const XnRGB24Pixel* pImageMap = image.GetRGB24ImageMap();

	glColor3f(1, 1, 1);
//	XnDepthPixel maxdepth = depth.GetDeviceMaxDepth();
	const unsigned int xres = pDepthMapMD.XRes();
	const unsigned int yres = pDepthMapMD.YRes();

#ifdef DEBUGOUT
	if(printFile)
		datei.open("daniel.txt", ios::out);
#endif


	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<xres; x++) {
			aDepthMap[x+y*xres] = static_cast<GLubyte>(static_cast<float>(pDepthMap[x+y*xres])/static_cast<float>(maxdepth)*255);
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
	glTranslatef(-800, 600, -2000);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, 640, 480, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, aDepthMap);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1); glVertex3f(0,0,0);
		glTexCoord2f(1,1); glVertex3f(640,0,0);
		glTexCoord2f(1,0); glVertex3f(640,480,0);
		glTexCoord2f(0,0); glVertex3f(0,480,0);
	glEnd();
	glPopMatrix();*/
/*
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-100, -100, -2000);
	glRotatef(cx,0,1,0);
	glRotatef(cy,1,0,0);
	glTranslatef(-320, -240, 1000);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageMap);
	glBegin(GL_POINTS);
	for(unsigned int y=0; y<yres-1; y++) {
		for(unsigned int x=0; x<630; x++) {
			if(pDepthMap[x+y*xres]!=0) {
				glTexCoord2f(static_cast<float>(x)/static_cast<float>(630), static_cast<float>(y)/static_cast<float>(480)); 
				glVertex3f(x, (yres-y), -pDepthMap[x+y*xres]/2.00);
			}
		}
	}
	glEnd();
	glPopMatrix();*/

	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
#ifdef DEBUGOUT
	if(printFile) {
		printFile=false;
		datei.close();
	}
#endif
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

	/* SceneAnalzer einstellen */
	nRetVal = user.Create(context);
	checkError("Fehler beim Konfigurieren des Usergenerators", nRetVal)?0:exit(-1);


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
	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_CULL_FACE);           //Backface Culling aktivieren
//	glEnable(GL_ALPHA_TEST);
//	glAlphaFunc(GL_GEQUAL, 1);

	/* Texturen */
	glGenTextures(1, &texture_rgb);
	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	glGenTextures(1, &texture_depth);
	glBindTexture(GL_TEXTURE_2D, texture_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_mouse_motion);
	glutKeyboardFunc(glut_keyboard);
	glutMainLoop();
	return 0;
}