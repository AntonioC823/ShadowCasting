#include "Block.h"
#include "Constants.h"

Block::Block(int x, int y)
{
	x_ = x;
	y_ = y;
	width_ = BLOCK_WIDTH;
}

sf::RectangleShape Block::draw() const
{
	sf::RectangleShape block;
	block.setSize(sf::Vector2f(width_, width_));
	block.setOutlineColor(sf::Color::Green);
	block.setFillColor(sf::Color::Blue);
	block.setOutlineThickness(0);
	block.setPosition(x_ * width_, y_ * width_);

	return block;
}

int Block::getX() const
{
	return x_;
}

int Block::getY() const
{
	return y_;
}
