// Allgemein
#include <iostream>
#include <string>

// OpenNI
#include <XnCppWrapper.h>


using namespace std;

bool checkError(string message, XnStatus nRetVal) {
	if(nRetVal != XN_STATUS_OK) {
		printf("%s: %s\n", message, xnGetStatusString(nRetVal));
		return false;
	}
	return true;
}

int main() {
	XnStatus nRetVal = XN_STATUS_OK;
	xn::Context context;

	nRetVal = context.Init();
	checkError("Fehler beim Initialisieren des Context", nRetVal)?0:exit(-1);

	xn::DepthGenerator depth;
	nRetVal = depth.Create(context);
	checkError("Fehler beim Erstellen des Tiefengenerators", nRetVal)?0:exit(-1);

	nRetVal = context.StartGeneratingAll();
	while(1) {
		nRetVal = context.WaitOneUpdateAll(depth);
		if(!checkError("Fehler beim Aktualisieren der Daten", nRetVal))
			continue;


	}

	return 0;
}