#pragma once
#include <XnCppWrapper.h>
#include "KTriangle.h"


struct KMetaData 
{
	int XRes;
	int YRes;
};


class KHeigthMap
{
public:
	KHeigthMap(void);
	~KHeigthMap(void);

	void feedDepthmap(XnDepthPixel* DataFeed, xn::DepthMetaData MetaData);
	void normalize(); // Makes sure, that all z-coods are between 0 and 255
	void compute(); // Creates the triangles
	void render();	// Paints the triangles

// Private Klassen-member
private:
	KVector* vectors;
	int vcounter;

	XnDepthPixel** data;
	KMetaData**		metadata;

	int dcounter;


// Private Klassen-Funktionen
private:

	



};


