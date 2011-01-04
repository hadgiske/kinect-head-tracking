#pragma once
#include "defines.h"
#include "KVertex.h"


class KGrid
{
public:
	KGrid(int gridlines);
	~KGrid(void);

	
	// Renders the grid with dimensions x , y , z
	void renderGrid(float x, float y, float z);


private:
	// Saves the number of gridlines
	int mGridlines;

	// Saves the vertices
	KVertex* mVertexField;
	int mVertexFieldSize;

	// Create the grid
	void createGrid(void);
	void paintGrid(void);
};

