#include "World.h"

World::World()
{
	for (int i = 0; i < WORLD_WIDTH; i++)
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
		{
			world_[i][j] = UNOCCUPIED;
		}
	}
	width_ = WORLD_WIDTH;
	height_ = WORLD_HEIGHT;
	for (int i = 0; i < WORLD_WIDTH; i++)
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				edgeID_[i][j][k] = 0;
				edgeExist_[i][j][k] = false;
			}
		}
	}
}

int World::getWidth() const
{
	return width_;
}

int World::getHeight() const
{
	return height_;
}

void World::setStatus(int x, int y, LocationStatus status)
{
	world_[x][y] = status;
}

LocationStatus World::getStatus(int x, int y) const
{
	return world_[x][y];
}

void World::setEdgeID(int x, int y, int index, int edgeID)
{
	edgeID_[x][y][index] = edgeID;
}

int World::getEdgeID(int x, int y, int index) const
{
	return edgeID_[x][y][index];
}

void World::setEdgeExist(int x, int y, int index, bool edgeExist)
{
	edgeExist_[x][y][index] = edgeExist;
}

bool World::getEdgeExist(int x, int y, int index) const
{
	return edgeExist_[x][y][index];
}
