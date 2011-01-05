#pragma once
#include <gl/glut.h>

// Helper-struct
struct KRGBColor 
{
	KRGBColor(float _r, float _g, float _b){
		r = _r;
		g = _g;
		b = _b;
	}

	KRGBColor(){
		r = 0;
		g = 0;
		b = 0;
	}

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

