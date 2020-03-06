#include "Ray.h"

Ray::Ray(float angle, float x, float y)
{
	angle_ = angle;
	x_ = x;
	y_ = y;
}

float Ray::getAngle() const
{
	return angle_;
}

float Ray::getX() const
{
	return x_;
}

float Ray::getY() const
{
	return y_;
}
