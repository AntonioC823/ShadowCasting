#pragma once
#include "SFML\Graphics.hpp"

class Ray
{
public:
	Ray(float angle, float x, float y);
	float getAngle() const;
	float getX() const;
	float getY() const;
private:
	float angle_;
	float x_;
	float y_;
};
