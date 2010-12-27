#pragma once
#include "KVector.h"
#include <GL/Glut.h>


enum Vertices
{
	PtA = 1,
	PtB = 2,
	PtC = 3
};


class KTriangle
{
public:
	KTriangle(KVector x, KVector y, KVector z);
	~KTriangle(void);

	// Getter and SETTER!
	KVector& getA();
	KVector& getB();
	KVector& getC();


	// Renders the three Vectors of the Triangle
	// with Glut-functions
	void renderTriangle();
	void renderTriangle(float r, float g, float b); //same but with specific color
	void renderTriangleS(Vertices Point); // Renders only one vec
	void renderTriangleS(Vertices Point, float r, float g, float b);

//Private Members:
private:
	KVector A;
	KVector B;
	KVector C;
};
