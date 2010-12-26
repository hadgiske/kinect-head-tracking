// Allgemein
#include <iostream>

// OpenNI
#include <XnCppWrapper.h>


using namespace std;

int main() {
	XnStatus nRetVal = XN_STATUS_OK;
	xn::Context context;

	nRetVal = context.Init();
	if(nRetVal != XN_STATUS_OK) {
		printf("Failed to initialize OpenNI: %s\n", xnGetStatusString(nRetVal));
		exit(-1);
	}


	return 0;
}