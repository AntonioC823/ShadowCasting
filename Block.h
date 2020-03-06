#pragma once
#include "SFML\Graphics.hpp"

class Block
{
public:
	Block(int x, int y);
	sf::RectangleShape draw() const;
	int getX() const;
	int getY() const;
private:
	int x_;
	int y_;
	int width_;
};
