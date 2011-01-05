#include "KItems.h"


KItems::KItems(int ItemNumber, int inFront)
{
	// Control the given vars and correct them
	if(ItemNumber < 0)
		ItemNumber = -ItemNumber;

	if(inFront < 0)
		inFront = -inFront;

	if(inFront > ItemNumber)
		inFront = 0;

	// Save the item-specification
	mItemsInFront = inFront;
	mItemNumber   = ItemNumber;

	// Initalize the Item-Field
	mItemField = 0;
	mItemFieldSize = 0;

	// Create the Items
	createItems();
}


KItems::~KItems(void)
{
	// If there are any Items --> delete them
	if(mItemFieldSize > 0){
		delete [] mItemField;
		mItemFieldSize = 0;
	}
}


void KItems::createItems(void)
{
	// If there is something to create... do it
	if( mItemNumber > 0 ){
		
		// Create a field to save circles in
		mItemField = new KCircle[mItemNumber];
		mItemFieldSize = mItemNumber;

		// Create the depthstep and the startdepth
		float depthStep = 1.0f / mItemFieldSize;
		float startDepth = mItemsInFront*depthStep;

		// Initalize the random
		std::srand(static_cast<unsigned int>(std::time(0)));
		

		//Create the circles:
		for(int i = 0 ; i < mItemFieldSize ; i++){
			mItemField[i].mCenter = KVertex(	0.7f * ((std::rand()%1001) / 1000.0f - 0.5f) + 0.5f,
												0.7f * ((std::rand()%1001) / 1000.0f - 0.5f) + 0.5f,
												startDepth - i * depthStep,
												KRGBColor(ITEM_COLOR));
			mItemField[i].mRadius = ITEM_SIZE;

			//DEBUG
			if(mItemField[i].mCenter.mX  >= 0.95f || mItemField[i].mCenter.mX  <= 0.05 || mItemField[i].mCenter.mY  >= 0.95 || mItemField[i].mCenter.mY  <= 0.05)
				int x = 1+1;


			// Make the nearer nearer to the center
			if(i < mItemsInFront){
				mItemField[i].mCenter.mX = (mItemField[i].mCenter.mX - 0.5f) / 2.0f + 0.5f;
				mItemField[i].mCenter.mY = (mItemField[i].mCenter.mY - 0.5f) / 2.0f + 0.5f;
			}

		}
	}
}


// Renders the Items
void KItems::renderItems(float x, float y, float z)
{
	// Fetch the current matrix and save it
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Translate the whole scene backwards
	glTranslatef(0,0,-z/2);

	// scale the scene
	// and transport it to the middle
	glScalef(x,y,z);
	glTranslatef(-0.5f, -0.5f, 0.5f);

	// paint the items
	for (int i = 0 ; i < mItemFieldSize ; i++)
	{
		mItemField[i].renderCircle();
	}
	

	// Return the old matrix from stack
	glPopMatrix();
}
