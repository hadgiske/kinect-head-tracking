#pragma once
#include "KGrid.h"
#include "KItems.h"


class KHeadTrack
{
public:
	// ScreenSize in Millimeters
	KHeadTrack(unsigned int ScreenSize);
	~KHeadTrack(void);

	// renders the headtracking-scene
	void renderScene(void);
	// Refreshes the data
	void refreshData(float headX, float headY, float headZ);


private:
	// Saves the head-positions
	float mHeadX, mHeadY, mHeadZ;
	// Saves the grid
	KGrid mGrid;
	// Saves the "targets"
	KItems mItems;
	// Saves the ScreenSize
	unsigned int mScreenSize;
	// Saves if there are any new data
	bool mRefresh;
};

