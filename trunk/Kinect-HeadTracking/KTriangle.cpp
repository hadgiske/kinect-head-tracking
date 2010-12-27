#include "KTriangle.h"


KTriangle::KTriangle(KVector x, KVector y, KVector z)
{

	// Get in here, if linearity-problems
	A = x;
	B = y;
	C = z;
}

KTriangle::~KTriangle(void)
{
}

KVector& KTriangle::getA(){
	return A;
}

KVector& KTriangle::getB(){
	return B;
}

KVector& KTriangle::getC(){
	return C;
}

void KTriangle::renderTriangle(){
	glVertex3f(A.getX(),A.getY(),A.getZ());
	glVertex3f(B.getX(),B.getY(),B.getZ());
	glVertex3f(C.getX(),C.getY(),C.getZ());

}

void KTriangle::renderTriangle(float r, float g, float b){
	glColor3f(r,g,b);
	renderTriangle();
}

void KTriangle::renderTriangleS(Vertices Point){
	switch(Point){
		case PtA:
			glVertex3f(A.getX(),A.getY(),A.getZ());
			break;
		case PtB:
			glVertex3f(B.getX(),B.getY(),B.getZ());
			break;
		case PtC:
			glVertex3f(C.getX(),C.getY(),C.getZ());
			break;

	}
}

void KTriangle::renderTriangleS(Vertices Point, float r, float g, float b){
	glColor3f(r,g,b);
	renderTriangleS(Point);

}


