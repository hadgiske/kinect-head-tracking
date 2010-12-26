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
	checkError("Fehler beim Initialisieren des Context", nRetVal);

	xn::DepthGenerator depth;
	nRetVal = depth.Create(context);
	checkError("Fehler beim Erstellen des Tiefengenerators", nRetVal);

	nRetVal = context.StartGeneratingAll();
	while(1) {
		nRetVal = context.WaitOneUpdateAll(depth);
		errorCheck("Fehler beim Aktualisieren der Daten", nRetVal);
	}

	return 0;
}