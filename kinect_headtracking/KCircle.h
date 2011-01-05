#pragma once
#include "KVertex.h"
#include "defines.h"
#include <cmath>


class KCircle
{
public:
	KCircle(void);
	KCircle(KVertex Center, float radius);
	~KCircle(void);

	// Paints the Circle
	void renderCircle(void);
	KVertex mCenter;
	float mRadius;	
};

