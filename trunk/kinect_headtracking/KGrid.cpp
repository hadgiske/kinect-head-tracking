#include "KGrid.h"

KGrid::KGrid(int gridlines)
{
	// Control the gridlines
	if(gridlines < 0 )
		gridlines = -gridlines;

	// Save the gridline-number
	mGridlines = gridlines;

	// Initialize the Vertex-Field
	mVertexField = 0;
	mVertexFieldSize = 0;

	// Create the Grid
	createGrid();
}


KGrid::~KGrid(void)
{
	// Delete the vertices
	if(mVertexFieldSize > 0)
	{
		delete [] mVertexField;
		mVertexFieldSize = 0;
	}
}


// Renders the grid
void KGrid::renderGrid(float x, float y, float z)
{
	// Begin Painting

	// First get the modelview-matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Transfer the whole scene backwards
	glTranslatef(0,0,-z/2);
	

	// Paint the back
	// that means pushing the whole scene back
	// Remember: The grid is normalized!
	// and to the middle
	glPushMatrix();
	glTranslatef(-x/2,-y/2,-z/2);
	glScalef(x,y,1.0f);
	paintGrid();
	glPopMatrix();
	
	
	// Paint the right and left wall
	// that means rotate the whole scene
	// place it right
	glPushMatrix();
	glRotatef(90,0,1,0);
	glScalef(z,y,1.0f);
	glTranslatef(-0.5,-0.5,-x/2);
	paintGrid();
	glTranslatef(0,0,x);
	paintGrid();
	glPopMatrix();


	// Paint the floor and ceiling
	// that means rotate the whole scene
	// place it right
	glPushMatrix();
	glRotatef(90,1,0,0);
	glScalef(x,z,1.0f);
	glTranslatef(-0.5,-0.5,-y/2);
	paintGrid();
	glTranslatef(0,0,y);		
	paintGrid();
	glPopMatrix();

	// Get back the first matrix
	glPopMatrix();

}


void KGrid::createGrid(void)
{
	// Ensure, that there are are more gridlines
	if(mGridlines >= 0){

		// Create a new Vertex-field
		mVertexField = new KVertex [4 * (mGridlines + 2)];
		mVertexFieldSize = 4 * (mGridlines + 2);

		// Run over the field
		for(int i = 0 ; i < mVertexFieldSize / 2 ; i = i + 2){
			mVertexField[i].mX = 1.0f * i / (mGridlines + 1) / 2.0f;
			mVertexField[i].mY = 0;
			mVertexField[i].setColor(GRID_LINE_COLOR);
			mVertexField[i+1].mX = 1.0f * i / (mGridlines + 1) / 2.0f;
			mVertexField[i+1].mY = 1.0f;
			mVertexField[i+1].setColor(GRID_LINE_COLOR);
		}

		for(int i = mVertexFieldSize / 2 ; i < mVertexFieldSize ; i = i + 2){
			mVertexField[i].mY = 1.0 * (i - mVertexFieldSize / 2.0f) / (mGridlines + 1) / 2.0f;
			mVertexField[i].mX = 0.0f;
			mVertexField[i].setColor(GRID_LINE_COLOR);
			mVertexField[i+1].mY = 1.0 * (i - mVertexFieldSize / 2.0f) / (mGridlines + 1) / 2.0f;
			mVertexField[i+1].mX = 1.0f;
			mVertexField[i+1].setColor(GRID_LINE_COLOR);
		}

	}
}

void KGrid::paintGrid(void){
	glBegin(GL_LINES);		
	// Render each single point
	for(int i = 0 ; i < mVertexFieldSize ; i++){
		mVertexField[i].paintVertex();
	}
	glEnd();
}
