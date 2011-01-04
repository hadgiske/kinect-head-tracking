// Std-includes of c++
#include <iostream>
using namespace std;

// Internal includes of this program
#include "KProgram.h"

int main(int argc, char* argv[]){

	// Setup glut
	KProgram::initGlut(argc, argv);

	// Show window
	KProgram::showWindow();

	return 0;
}