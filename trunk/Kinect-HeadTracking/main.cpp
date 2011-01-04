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

#include "textures.h"


using namespace std;

/* Defines */
#define WINDOW_SIZE_X 1024
#define WINDOW_SIZE_Y 768
//#define DEBUGOUT
//#define CONSOLEOUT
#define ROOM_X 800
#define ROOM_Y 600
#define ROOM_Z 1000

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

class WorldPos {
public:
	WorldPos(float xk, float yk, float zk)	{
		x = xk;
		y = yk;
		z = zk;
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
	}

	WorldPos(){
		x = 0;
		y = 0;
		z = 0;
		offset_x = 0;
		offset_y = 0;
		offset_z = 0;
	}

	float x;
	float y;
	float z;

	WorldPos operator-(WorldPos first){
		WorldPos retval;

		retval.x = x - first.x;
		retval.y = y - first.y;
		retval.z = z - first.z;

		return retval;
	}

private: 
	float offset_x, offset_y, offset_z;

};

WorldPos wpos[4];

void init_wpos(){
	wpos[0].x = -ROOM_X/2;
	wpos[0].y = -ROOM_Y/2;
	wpos[0].z = -ROOM_Z/2;

	wpos[1].x = +ROOM_X/2;
	wpos[1].y = -ROOM_Y/2;
	wpos[1].z = -ROOM_Z/2;

	wpos[2].x = +ROOM_X/2;
	wpos[2].y = +ROOM_Y/2;
	wpos[2].z = -ROOM_Z/2;

	wpos[3].x = -ROOM_X/2;
	wpos[3].y = +ROOM_Y/2;
	wpos[3].z = -ROOM_Z/2;

}

WorldPos getNormal(WorldPos first, WorldPos second){
	WorldPos retval;
	retval.x = first.y*second.z - first.z*second.y;
	retval.y = -(first.x*second.z - first.z*second.x);
	retval.z = first.x*second.y - first.y*second.x;

	return retval;

}
/* Globals */
static int win;
xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator image;
xn::UserGenerator user;
XnStatus nRetVal;
GLuint texture_rgb, texture_depth, texture_quads;
GLubyte aDepthMap[640*480];
bool printFile = false;
bool calibration = false;
int maxdepth=-1;

float light_lin = 0.0030;
float light_quad = 0.002;

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
	case '1':
		light_lin += 0.0001;
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_lin);
		break;
	case '!':
		light_lin -= 0.0001;
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_lin);
		break;
	case '2':
		light_quad += 0.0001;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_quad);
		break;
	case '\"':
		light_quad -= 0.0001;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_quad);
		break;
	}

	cout	<< "Scale X: " << scalex << "\tScale Y: " << scaley << "\tTrans X: " << transx << "\tTrans Y: " << transy << endl
			<< "Light_lin: " << light_lin << " \tLight_quad: " << light_quad << endl;
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


void draw_room() {
	glBegin(GL_QUADS);
	glColor3f(1,1,1);

	glBindTexture(GL_TEXTURE_2D, texture_rgb);
	//Rückwand zeichen
	//Normale berechnen
	WorldPos normal = getNormal(wpos[1]-wpos[0],wpos[3]-wpos[0]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0, 0);glVertex3f( wpos[0].x,wpos[0].y,wpos[0].z);
	glTexCoord2f(6, 0);glVertex3f( wpos[1].x,wpos[1].y,wpos[1].z);
	glTexCoord2f(6, 6);glVertex3f( wpos[2].x,wpos[2].y,wpos[2].z);
	glTexCoord2f(0, 6);glVertex3f( wpos[3].x,wpos[3].y,wpos[3].z);
	glEnd();

	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	//Boden zeichen	
	normal = getNormal(wpos[0]-wpos[1],WorldPos(ROOM_X/2,-ROOM_Y/2,ROOM_Z/2)-wpos[1]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0, 0);glVertex3f(-ROOM_X/2,-ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 0);glVertex3f( ROOM_X/2,-ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 6);glVertex3f(wpos[1].x,wpos[1].y,wpos[1].z);
	glTexCoord2f(0, 6);glVertex3f(wpos[0].x,wpos[0].y,wpos[0].z);
	glEnd();
	glBegin(GL_QUADS);

	glColor3f(1,1,1);
	//Rechte Wand zeichnen	
	normal = getNormal(WorldPos(ROOM_X/2, -ROOM_Y/2, ROOM_Z/2)-wpos[1], wpos[2]-wpos[1]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0, 0);glVertex3f(ROOM_X/2,-ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 0);glVertex3f(ROOM_X/2, ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 6);glVertex3f(wpos[2].x, wpos[2].y,wpos[2].z);
	glTexCoord2f(0, 6);glVertex3f(wpos[1].x,wpos[1].y,wpos[1].z);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1,1,1);
	//Linke Wand
	normal = getNormal(wpos[3]-wpos[0], WorldPos(-ROOM_X/2,-ROOM_Y/2,ROOM_Z/2)-wpos[0]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0, 0);glVertex3f(-ROOM_X/2,-ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 0);glVertex3f( wpos[0].x,wpos[0].y,wpos[0].z);
	glTexCoord2f(6, 6);glVertex3f( wpos[3].x,wpos[3].y,wpos[3].z);
	glTexCoord2f(0, 6);glVertex3f(-ROOM_X/2, ROOM_Y/2,ROOM_Z/2);

	glEnd();
	glBegin(GL_QUADS);

	glColor3f(1,1,1);
	//Decke	
	normal = getNormal(WorldPos(ROOM_X/2, ROOM_Y/2, ROOM_Z/2)-wpos[2], wpos[3]-wpos[2]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0, 0);glVertex3f(-ROOM_X/2,ROOM_Y/2,ROOM_Z/2);
	glTexCoord2f(6, 0);glVertex3f(wpos[3].x,wpos[3].y,wpos[3].z);
	glTexCoord2f(6, 6);glVertex3f( wpos[2].x,wpos[2].y,wpos[2].z);
	glTexCoord2f(0, 6);glVertex3f( ROOM_X/2,ROOM_Y/2,ROOM_Z/2);
	glEnd();


	//glRotatef(mouse.cx,0,1,0);
	//glRotatef(-mouse.cy,1,0,0);
	glTranslatef(0,0,300);
	glPushMatrix();
	glTranslatef(30,0,-130);
	glutSolidCube(80);
	glPopMatrix();

	glutSolidCube(80);
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

	glEnable(GL_TEXTURE_2D);

	pUser[0] = 0;
	pUser[1] = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*glFrustum(	-ROOM_X/2+(1500.0/1750.0)*static_cast<int>(headtrans.x),
				ROOM_X/2+(1500.0/1750.0)*static_cast<int>(headtrans.x),
				-ROOM_Y/2-(1500.0/1750.0)*static_cast<int>(headtrans.y),
				ROOM_Y/2-(1500.0/1750.0)*static_cast<int>(headtrans.y),
				1525,
				2525);*/

	float nearplane = 0.05;
	float screenaspect = ROOM_X/ROOM_Y;
	glFrustum(	nearplane*(-0.5 * screenaspect + headtrans.x)/headtrans.z,
				nearplane*( 0.5 * screenaspect + headtrans.x)/headtrans.z,
				nearplane*(-0.5 + headtrans.y)/headtrans.z,
				nearplane*( 0.5 + headtrans.y)/headtrans.z,
				nearplane,
				2525);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
/*			headtrans.rotmat[0] = head.orientation.orientation.elements[0];
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
			headtrans.rotmat[15] = 1;*/

			headtrans.x = head.position.position.X;
			headtrans.y = head.position.position.Y;
			headtrans.z = head.position.position.Z;
			
			 
#ifdef CONSOLEOUT
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
#endif
		}
	}

	//------------------------------------------------------------------------------------------
	//BEGIN: Kamera-Test
	//------------------------------------------------------------------------------------------
	//glTranslatef(0,0,-2000);
	glTranslatef(headtrans.x,headtrans.y,1-headtrans.z);
	cout << headtrans.x << " " << headtrans.y << " " << headtrans.z << endl;
	cout	<< nearplane*(-0.5 * screenaspect + headtrans.x)/headtrans.z	<< " "	
			<< nearplane*( 0.5 * screenaspect + headtrans.x)/headtrans.z	<< " "	
			<< nearplane*(-0.5 + headtrans.y)/headtrans.z					<< " "	
			<< nearplane*( 0.5 + headtrans.y)/headtrans.z					<< " "	<< endl;

	
		
		
	draw_room();

	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glutSolidCube(5);
	//glPopAttrib();
	//------------------------------------------------------------------------------------------
	
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
	glutInitWindowPosition(300,0);
	win = glutCreateWindow("kinect-head-tracking");
	glClearColor(0, 0, 0, 0.0); //Hintergrundfarbe: Hier ein leichtes Blau
	glEnable(GL_DEPTH_TEST);          //Tiefentest aktivieren
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);           //Backface Culling aktivieren

	float light_position[4] = {0.0,0.0,-1550.0,1.0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.006);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001);

	init_wpos();

	/* Texturen */
	glGenTextures(1, &texture_quads);
	glBindTexture(GL_TEXTURE_2D, texture_quads);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 200, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, quads_texture_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glutDisplayFunc(glut_display);
	glutIdleFunc(glut_idle);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_mouse_motion);
	glutKeyboardFunc(glut_keyboard);
	glutMainLoop();
	return 0;
}