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

#define DEBUGOUT


/* Globals */
static int win;
xn::Context context;
xn::DepthGenerator depth;
xn::ImageGenerator image;
xn::UserGenerator user;
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
bool printFile = false;
bool calibration = false;
int maxdepth=-1;

struct WorldPos {
	int v[640*480];
};

//WorldPos aWorldPos[maxdepth];

float scalex=0.9, scaley=0.9, transx=41, transy=35;

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

	cout << "Scale X: " << scalex << "\tScale Y: " << scaley << "\tTrans X: " << transx << "\tTrans Y: " << 
transy << endl;
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

int getMaxDepth(const XnDepthPixel* depthmap, int size=640*480) {
	int max_tmp=0;
	for(int i=0; i<size; i++) {
		if(depthmap[i]>max_tmp)
			max_tmp=depthmap[i];
	}
	
	return max_tmp;
}

void _stdcall skel_cal(xn::SkeletonCapability& skeleton, XnUserID user, void* pCookie){
	cout << "Calibration" << endl;
}

void _stdcall skel_cal_end(xn::SkeletonCapability& skeleton, XnUserID user, XnBool bSuccess, void* pCookie){
	cout << "Calibration end" << endl;
}

float rot_angle=0;
void glut_display() {
	xn::DepthMetaData pDepthMapMD;
	xn::ImageMetaData pImageMapMD;
	XnUserID pUser[2];
	XnUInt16 nUsers=2;
	pUser[0] = 0;
	pUser[1] = 0;
	
	double* matrix = new double[16];

#ifdef DEBUGOUT
	ofstream datei;
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45, WINDOW_SIZE_X/WINDOW_SIZE_Y, 1, 5000);
//	glOrtho(0, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, -128, 128);

/*	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
//	glTranslatef(-12.8/640.0, 9.0/480.0, 0);
//	glTranslatef(-12.8/630.0, 9.0/480.0,0);
	glScalef(scalex, scaley, 1.0);
	glTranslatef(transx/630, transy/480, 0.0);*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	rot_angle+=0.7;

	// Warten auf neue Daten vom Tiefengenerator
	nRetVal = context.WaitAndUpdateAll();
	checkError("Fehler beim Aktualisieren der Daten", nRetVal);

	user.GetUsers(pUser, nUsers);

	cout << pUser[0];
	xn::SkeletonCapability pSkeleton = user.GetSkeletonCap();
	cout << " " << pSkeleton.IsCalibrated(pUser[0]) << " ";
	XnSkeletonJoint* joints;
	XnCallbackHandle bla;
	pSkeleton.RegisterCalibrationCallbacks(skel_cal,skel_cal_end,0,bla);
	
	if(calibration){
		pSkeleton.RequestCalibration(pUser[0],true);
		calibration = false;
		cout << "Bitte  laecheln" << endl;
	}

//	pSkeleton.SetJointActive(XN_SKEL_LEFT_HAND,true);
	pSkeleton.SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	XnUInt16 qty_joints=24;
	if(pSkeleton.IsCalibrated(pUser[0])) {
		pSkeleton.StartTracking(pUser[0]);
		joints = new XnSkeletonJoint[24];
		pSkeleton.EnumerateActiveJoints(joints, qty_joints);
		for(int i=0; i<qty_joints; i++) {
			if(joints[i]==XN_SKEL_HEAD)
				cout << " Kopf erkannt!" ;
		}

		delete [] joints;
	}

	if(pSkeleton.IsCalibrated(pUser[0])){
		XnSkeletonJointTransformation skeltranslh;
	
		pSkeleton.GetSkeletonJoint(pUser[0],XN_SKEL_HEAD,skeltranslh);

		cout	<< " "   << "Confidence Position: " << skeltranslh.position.fConfidence
				<< " X: " << skeltranslh.position.position.X
				<< " Y: " << skeltranslh.position.position.Y
				<< " Z: " << skeltranslh.position.position.Z << endl
				<< "Confidence Rotation: " << skeltranslh.orientation.fConfidence << endl
				<< "\t" << skeltranslh.orientation.orientation.elements[0]
				<< "\t" << skeltranslh.orientation.orientation.elements[3]
				<< "\t" << skeltranslh.orientation.orientation.elements[6] << endl
				<< "\t" << skeltranslh.orientation.orientation.elements[1]
				<< "\t" << skeltranslh.orientation.orientation.elements[4]
				<< "\t" << skeltranslh.orientation.orientation.elements[7] << endl
				<< "\t" << skeltranslh.orientation.orientation.elements[2]
				<< "\t" << skeltranslh.orientation.orientation.elements[5]
				<< "\t" << skeltranslh.orientation.orientation.elements[8];	

				matrix[0] = skeltranslh.orientation.orientation.elements[0];
				matrix[1] = skeltranslh.orientation.orientation.elements[1];
				matrix[2] = skeltranslh.orientation.orientation.elements[2];
				matrix[3] = 0;

				matrix[4] = skeltranslh.orientation.orientation.elements[3];
				matrix[5] = skeltranslh.orientation.orientation.elements[4];
				matrix[6] = skeltranslh.orientation.orientation.elements[5];
				matrix[7] = 0;

				matrix[8] = -skeltranslh.orientation.orientation.elements[6];
				matrix[9] = -skeltranslh.orientation.orientation.elements[7];
				matrix[10] =-skeltranslh.orientation.orientation.elements[8];
				matrix[11] = 0;

				matrix[12] = 0;
				matrix[13] = 0;
				matrix[14] = 0;
				matrix[15] = 1;



	}



	cout << endl;

	glTranslatef(0, 0, -50);
	glRotatef(cx,0,1,0);
	glRotatef(-cy,1,0,0);

	glMultMatrixd(matrix);
	glutSolidCube(10);

	delete [] matrix;

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