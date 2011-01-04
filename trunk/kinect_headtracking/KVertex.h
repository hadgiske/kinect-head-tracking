#pragma once
#include <gl/glut.h>

// Helper-struct
struct KRGBColor 
{
	float r;
	float g;
	float b;
};

class KVertex
{
public:
	KVertex(void);
	KVertex(float x, float y, float z, KRGBColor color);
	~KVertex(void);

	// Makes glVertex3f and glColor3f
	void paintVertex(void);

	// sets the color
	void setColor(float r, float g, float b);

	// Saves the coordinates of the vertex
	float mX,mY,mZ;

	//Saves the color of the 
	KRGBColor mColor;
	
};

