#include "KVertex.h"


KVertex::KVertex(void)
{
	//If nothing specified make zero-point and black
	mX = 0;
	mY = 0;
	mZ = 0;

	mColor.r = 0;
	mColor.g = 0;
	mColor.b = 0;
}

KVertex::KVertex(float x, float y, float z, KRGBColor color){
	mX = x;
	mY = y;
	mZ = z;

	mColor = color;
}


KVertex::~KVertex(void)
{
}


void KVertex::paintVertex(void)
{
	glColor3f(mColor.r, mColor.g, mColor.b);
	glVertex3f(mX,mY,mZ);
}


void KVertex::setColor(float r, float g, float b)
{
	mColor.r = r;
	mColor.g = g;
	mColor.b = b;
}
