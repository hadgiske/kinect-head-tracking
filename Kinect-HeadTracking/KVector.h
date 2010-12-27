#pragma once

class KVector
{
public:
	// Konstruction within 3dimensional Vector and standardkonsctructor
	KVector(double x, double y, double z);
	KVector();
	~KVector(void);

	// Public Methods
inline	double getX();
inline 	double getY();
inline	double getZ();


	// Operators
	KVector operator+(KVector add);
	KVector operator-(KVector sub);
	double operator*(KVector mult);
	KVector operator*(double mult);

	// Get amount
	double getAbs();


	
	
// Private Members
public:
	double x;
	double y;
	double z;
};

KVector operator*(double mult, KVector vec);