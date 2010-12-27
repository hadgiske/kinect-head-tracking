#include "KVector.h"

KVector::KVector(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

KVector::KVector(){
	x = 0;
	y = 0;
	z = 0;
}

inline double KVector::getX(){
	return x;
}
inline double KVector::getY(){
	return y;
}

inline double KVector::getZ(){
	return z;
}

KVector::~KVector(void)
{
}

KVector KVector::operator+(KVector add){
	return KVector(x + add.getX(),y + add.getY(), z + add.getZ());
}

KVector KVector::operator-(KVector sub){
	return KVector(x - sub.getX(),y - sub.getY(), z - sub.getZ());
}

double KVector::operator*(KVector mult){
	return (x*mult.getX() + y*mult.getY() + z*mult.getZ());
}

KVector KVector::operator*(double mult){
	return KVector(x * mult, y*mult, z*mult);
}

KVector operator*(double mult, KVector vec){
	return KVector(mult*vec.getX(),mult*vec.getY(),mult*vec.getZ());
}


double KVector::getAbs(){
	return x*x + y*y + z*z;
}