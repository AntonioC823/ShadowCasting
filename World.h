#pragma once
#include "SFML\Graphics.hpp"
#include "Constants.h"

enum EdgeID
{
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

enum LocationStatus
{
	OCCUPIED,
	UNOCCUPIED
};

class World
{
public:
	World();
	int getWidth() const;
	int getHeight() const;
	void setStatus(int x, int y, LocationStatus status);
	LocationStatus getStatus(int x, int y) const;
	void setEdgeID(int x, int y, int index, int edgeID);
	int getEdgeID(int x, int y, int index) const;
	void setEdgeExist(int x, int y, int index, bool edgeExist);
	bool getEdgeExist(int x, int y, int index) const;
private:
	LocationStatus world_[WORLD_WIDTH][WORLD_HEIGHT];
	int width_;
	int height_;
	int edgeID_[WORLD_WIDTH][WORLD_HEIGHT][4];
	bool edgeExist_[WORLD_WIDTH][WORLD_HEIGHT][4];
};
