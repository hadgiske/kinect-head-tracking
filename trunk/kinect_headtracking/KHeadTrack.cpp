#include "KHeadTrack.h"


KHeadTrack::KHeadTrack(unsigned int ScreenSize):
mGrid(GRID_LINE_NUMBER),
mItems(ITEM_NUMBER, ITEMS_INFRONT)
{
	// Control the ScreenSize and save it
	if(ScreenSize == 0 )
		ScreenSize = 1;
	
	mScreenSize = ScreenSize;

	// Initialize the head-cods
	mHeadX = 0;
	mHeadY = 0;
	mHeadZ = 0;

	// Initialize mRefresh
	mRefresh = true;
}


KHeadTrack::~KHeadTrack(void)
{
}


// renders the headtracking-scene
void KHeadTrack::renderScene(void){
	
	// If there is something to refresh, then refresh
	if(mRefresh){
		// Delete the old stuff
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Fog
		glFogf(GL_FOG_START, mHeadZ);
		glFogf(GL_FOG_END, mHeadZ+5);

		// Define a near-plane
		float nearPlane = 0.05f;

		// Setup the projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(	nearPlane*(-0.5 * WINDOW_SCREEN_ASPECT + mHeadX) / mHeadZ,
					nearPlane*( 0.5 * WINDOW_SCREEN_ASPECT + mHeadX) / mHeadZ,
					nearPlane*(-0.5 + mHeadY) / mHeadZ,
					nearPlane*( 0.5 + mHeadY) / mHeadZ,
					nearPlane, 100.0);

		// Setup ModelView
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(mHeadX,mHeadY,1-mHeadZ);

		// Lets look there
//		gluLookAt(mHeadX,-mHeadY,mHeadZ,mHeadX,-mHeadY,0,0,1,0);

		// Finally render
		mGrid.renderGrid(	CUBE_SIZE_X,
							CUBE_SIZE_Y,
							CUBE_SIZE_Z		);

		mItems.renderItems(	CUBE_SIZE_X,
							CUBE_SIZE_Y,
							CUBE_SIZE_Z		);

		// Save, that we have got refreshed
		mRefresh = false;
	}
}


// Refreshes the data
void KHeadTrack::refreshData(float headX, float headY, float headZ){

	// Convert the head-coods from mm's to screensize's
	mHeadX = headX / static_cast<float>(mScreenSize);
	mHeadY = headY / static_cast<float>(mScreenSize);
	mHeadZ = headZ / static_cast<float>(mScreenSize);

	// Save, that there are new values
	mRefresh = true;
}
