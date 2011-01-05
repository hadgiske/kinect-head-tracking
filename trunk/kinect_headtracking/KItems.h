#pragma once
#include "KCircle.h"
#include <ctime>
#include <iostream>

class KItems
{
public:
	KItems(int ItemNumber, int inFront);
	~KItems(void);

	// Renders the Items
	void renderItems(float x, float y, float z);

private:
	KCircle* mItemField;
	int mItemFieldSize;
	int mItemNumber;
	int mItemsInFront;

	void createItems(void);
};

