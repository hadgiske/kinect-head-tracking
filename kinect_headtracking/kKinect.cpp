#include "kKinect.h"
#include "KProgram.h"


kKinect::kKinect(void)
{
	initKinect();
	pUser[0] = 0;
	nUsers = 1;
	skeleton = 0;
}


kKinect::~kKinect(void)
{
	if(skeleton!=0) 
		delete skeleton;
}


void kKinect::initKinect(void)
{

	XnStatus nRetVal;
	nRetVal = XN_STATUS_OK;

	/* Context initialisieren (Kameradaten) */
	nRetVal = mContext.Init();
	checkError("Fehler beim Initialisieren des Context", nRetVal)?0:exit(-1);



	/* Tiefengenerator erstellen */
	nRetVal = mDepth.Create(mContext);
	checkError("Fehler beim Erstellen des Tiefengenerators", nRetVal)?0:exit(-1);

	/* Tiefengenerator einstellen */
	XnMapOutputMode outputModeDepth;
	outputModeDepth.nXRes = 640;
	outputModeDepth.nYRes = 480;
	outputModeDepth.nFPS = 30;
	nRetVal = mDepth.SetMapOutputMode(outputModeDepth);
	checkError("Fehler beim Konfigurieren des Tiefengenerators", nRetVal)?0:exit(-1);


	/* Imagegenerator erstellen */
	nRetVal = mImage.Create(mContext);
	checkError("Fehler beim Erstellen des Bildgenerators", nRetVal)?0:exit(-1);

	/* Imagegenerator einstellen */
	XnMapOutputMode outputModeImage;
	outputModeImage.nXRes = 640;
	outputModeImage.nYRes = 480;
	outputModeImage.nFPS = 30;
	nRetVal = mImage.SetMapOutputMode(outputModeImage);
	checkError("Fehler beim Konfigurieren des Bildgenerators", nRetVal)?0:exit(-1);	

	/* SceneAnalzer einstellen */
	nRetVal = user.Create(mContext);
	checkError("Fehler beim Konfigurieren des Usergenerators", nRetVal)?0:exit(-1);


	/* Starten der Generatoren - volle Kraft vorraus! */
	nRetVal = mContext.StartGeneratingAll();
	checkError("Fehler beim Starten der Generatoren", nRetVal)?0:exit(-1);
}


bool kKinect::checkError(std::string message, XnStatus RetVal)
{
	if(RetVal != XN_STATUS_OK) {
		std::cout << message << ": " << xnGetStatusString(RetVal) << std::endl;
		return false;
	}
	return true;
}


KVertex kKinect::getPosition(void)
{
	mContext.WaitAndUpdateAll();
	/* Anzahl der User auslesen und in Objekten speichern */
	nUsers=1;
	user.GetUsers(pUser, nUsers);
	if(nUsers>0) {
		/* User dem Skeleton zuweisen */
		xn::SkeletonCapability pSkeleton = user.GetSkeletonCap();
		if(skeleton!=0) {
			delete skeleton;
			skeleton = 0;
		}
		skeleton=new xn::SkeletonCapability(pSkeleton);
		if(skeleton->IsCalibrated(pUser[0])) {
			/* Alle Körperteile auswählen */
			skeleton->SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	
			/* Kopf initialisieren */
			XnSkeletonJointTransformation head;
			skeleton->StartTracking(pUser[0]);
			skeleton->GetSkeletonJoint(pUser[0], XN_SKEL_HEAD, head);

			if(head.position.fConfidence && head.orientation.fConfidence) {
				/*std::cout <<	"x: " << head.position.position.X << 
								"\t y: " << head.position.position.Y << 
								"x2: " << head.position.position.X/SCREEN_HEIGTH_MM <<
								"y2: " << (head.position.position.Y+200.0)/SCREEN_HEIGTH_MM <<
								std::endl;*/
				return KVertex(head.position.position.X*KProgram::x2, -(head.position.position.Y+200.0)*KProgram::y2, head.position.position.Z*KProgram::z2, KRGBColor());
			}
		}
	}
	return KVertex();
}


void kKinect::reset(void)
{
	if(pUser[0]!=0) {
		user.GetSkeletonCap().Reset(pUser[0]);
		pUser[0]=0;
		nUsers=1;
		if(skeleton!=0) {
			delete skeleton;
			skeleton = 0;
		}
	}
}


void kKinect::calibrateUser()
{	
	mContext.WaitAndUpdateAll();
	if(skeleton==0) {
		xn::SkeletonCapability pSkeleton = user.GetSkeletonCap();
		skeleton=new xn::SkeletonCapability(pSkeleton);
	}
	if(user.GetNumberOfUsers() != 0) {
		skeleton->RequestCalibration(pUser[0],true);
		std::cout << "Kalibration wird gestartet, bitte Arme im 90 Grad Winkel nach oben halten." << std::endl;
	}
}