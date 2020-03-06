#include <vector>
#include <algorithm>
#include "SFML\Graphics.hpp"
#include "World.h"
#include "Block.h"
#include "Ray.h"
#include "Constants.h"

struct Edge
{
	float sx, sy;
	float ex, ey;
};

void convertBlockToPoly(std::vector<Edge>& edges, World& world, int sx, int sy, int width, int height);

void calculateRays(std::vector<Ray>& rayPoints, std::vector<Edge> edges, float sourceX, float sourceY, float radius);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Shadow Casting");
	window.setFramerateLimit(FRAME_RATE);

	World world;
	std::vector<Block> blocks;
	sf::Mouse mouse;
	std::vector<Edge> edges;
	std::vector<Ray> rayPoints;

	// Load font once for later use of text
	sf::Font font;
	font.loadFromFile("OpenSans-Bold.ttf");

	// Initialize borders to avoid rays from searching for a non existent edge
	// Convert the blocks used to create the borders into a singular polygon
	// Draw horizontal border
	for (int x = 1; x < WORLD_WIDTH - 1; x++)
	{
		blocks.push_back(Block(x, 1));
		world.setStatus(x, 1, OCCUPIED);

		blocks.push_back(Block(x, WORLD_HEIGHT - 2));
		world.setStatus(x, WORLD_HEIGHT - 2, OCCUPIED);
	}

	// Draw vertical border
	for (int y = 2; y < WORLD_HEIGHT - 2; y++)
	{
		blocks.push_back(Block(1, y));
		world.setStatus(1, y, OCCUPIED);

		blocks.push_back(Block(WORLD_WIDTH - 2, y));
		world.setStatus(WORLD_WIDTH - 2, y, OCCUPIED);
	}

	convertBlockToPoly(edges, world, 0, 0, world.getWidth(), world.getHeight());

	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// Update

		float sourceX = mouse.getPosition(window).x;
		float sourceY = mouse.getPosition(window).y;

		// Use mouse presses to allow block placement and removal
		// Blocks will be reconverted to polygons with each mouse press
		// Left mouse press
		if (mouse.isButtonPressed(sf::Mouse::Left))
		{
			int locationX = ((int)sourceX / BLOCK_WIDTH);
			int locationY = ((int)sourceY / BLOCK_WIDTH);

			if (world.getStatus(locationX, locationY) != OCCUPIED)
			{
				world.setStatus(locationX, locationY, OCCUPIED);
				blocks.push_back(Block(locationX, locationY));
				convertBlockToPoly(edges, world, 0, 0, world.getWidth(), world.getHeight());
			}
		}

		// Right mouse press
		if (mouse.isButtonPressed(sf::Mouse::Right))
		{
			int locationX = ((int)sourceX / BLOCK_WIDTH);
			int locationY = ((int)sourceY / BLOCK_WIDTH);

			if (world.getStatus(locationX, locationY) != UNOCCUPIED)
			{
				world.setStatus(locationX, locationY, UNOCCUPIED);
				for (int i = 0; i < blocks.size(); i++)
				{
					if (blocks[i].getX() == locationX && blocks[i].getY() == locationY)
					{
						blocks.erase(blocks.begin() + i);
					}
				}

				convertBlockToPoly(edges, world, 0, 0, world.getWidth(), world.getHeight());
			}
		}

		// Clear elements in ray vector

		rayPoints.clear();

		// Handle key presses

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			calculateRays(rayPoints, edges, sourceX, sourceY, 1);
		}

		// Clear window
		window.clear();

		// Draw objects

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && rayPoints.size() > 1)
		{
			// Draw each triangle
			for (int i = 0; i < rayPoints.size() - 1; i++)
			{
				sf::ConvexShape triangle;
				triangle.setPointCount(3);
				triangle.setFillColor(sf::Color::White);
				triangle.setPoint(0, sf::Vector2f(sourceX, sourceY));
				triangle.setPoint(1, sf::Vector2f(rayPoints[i].getX(), rayPoints[i].getY()));
				triangle.setPoint(2, sf::Vector2f(rayPoints[i + 1].getX(), rayPoints[i + 1].getY()));

				window.draw(triangle);
			}

			sf::ConvexShape triangle;
			triangle.setPointCount(3);
			triangle.setFillColor(sf::Color::White);
			triangle.setPoint(0, sf::Vector2f(sourceX, sourceY));
			triangle.setPoint(1, sf::Vector2f(rayPoints[rayPoints.size() - 1].getX(), rayPoints[rayPoints.size() - 1].getY()));
			triangle.setPoint(2, sf::Vector2f(rayPoints[0].getX(), rayPoints[0].getY()));

			window.draw(triangle);
		}

		// Output current ray count
		sf::Text rayCountText("Ray Count: " + std::to_string(rayPoints.size()), font);
		rayCountText.setPosition(0, 0);
		rayCountText.setCharacterSize(20);
		rayCountText.setFillColor(sf::Color::White);
		window.draw(rayCountText);

		for (int i = 0; i < blocks.size(); i++)
		{
			window.draw(blocks[i].draw());
		}

		for (auto& i : edges)
		{
			sf::VertexArray line(sf::LineStrip, 2);
			line[0].position = sf::Vector2f(i.sx, i.sy);
			line[1].position = sf::Vector2f(i.ex, i.ey);
			window.draw(line);
		}

		window.display();
	}

	return 0;
}

void convertBlockToPoly(std::vector<Edge>& edges, World& world, int sx, int sy, int width, int height)
{
	// Clear current edges
	edges.clear();

	// Initialize world edge existance and edge ids to false and zero for recalculations
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int i = 0; i < 4; i++)
			{
				world.setEdgeExist(x + sx, y + sy, i, false);
				world.setEdgeID(x + sx, y + sy, i, 0);
			}
		}
	}

	for (int x = 1; x < width - 1; x++)
	{
		for (int y = 1; y < height - 1; y++)
		{
			int indexX = (x + sx), indexY = (y + sy);
			int northX = (x + sx), northY = (y + sy - 1);
			int southX = (x + sx), southY = (y + sy + 1);
			int westX = (x + sx - 1), westY = (y + sy);
			int eastX = (x + sx + 1), eastY = (y + sy);

			if (world.getStatus(indexX, indexY) == OCCUPIED)
			{
				// Check if western neighbor exists
				if (world.getStatus(westX, westY) != OCCUPIED)
				{
					if (world.getEdgeExist(northX, northY, WEST))
					{
						edges[world.getEdgeID(northX, northY, WEST)].ey += BLOCK_WIDTH;
						world.setEdgeID(indexX, indexY, WEST, world.getEdgeID(northX, northY, WEST));
						world.setEdgeExist(indexX, indexY, WEST, true);
					}
					else
					{
						Edge edge;
						edge.sx = (x + sx) * BLOCK_WIDTH;
						edge.sy = (y + sy) * BLOCK_WIDTH;
						edge.ex = edge.sx;
						edge.ey = edge.sy + BLOCK_WIDTH;

						int edgeID = edges.size();
						edges.push_back(edge);

						world.setEdgeID(indexX, indexY, WEST, edgeID);
						world.setEdgeExist(indexX, indexY, WEST, true);
					}
				}

				// Check if eastern neighbor exists
				if (world.getStatus(eastX, eastY) != OCCUPIED)
				{
					if (world.getEdgeExist(northX, northY, EAST))
					{
						edges[world.getEdgeID(northX, northY, EAST)].ey += BLOCK_WIDTH;
						world.setEdgeID(indexX, indexY, EAST, world.getEdgeID(northX, northY, EAST));
						world.setEdgeExist(indexX, indexY, EAST, true);
					}
					else
					{
						Edge edge;
						edge.sx = (x + sx + 1) * BLOCK_WIDTH;
						edge.sy = (y + sy) * BLOCK_WIDTH;
						edge.ex = edge.sx;
						edge.ey = edge.sy + BLOCK_WIDTH;

						int edgeID = edges.size();
						edges.push_back(edge);

						world.setEdgeID(indexX, indexY, EAST, edgeID);
						world.setEdgeExist(indexX, indexY, EAST, true);
					}
				}

				// Check if northern neighbor exists
				if (world.getStatus(northX, northY) != OCCUPIED)
				{
					if (world.getEdgeExist(westX, westY, NORTH))
					{
						edges[world.getEdgeID(westX, westY, NORTH)].ex += BLOCK_WIDTH;
						world.setEdgeID(indexX, indexY, NORTH, world.getEdgeID(westX, westY, NORTH));
						world.setEdgeExist(indexX, indexY, NORTH, true);
					}
					else
					{
						Edge edge;
						edge.sx = (x + sx) * BLOCK_WIDTH;
						edge.sy = (y + sy) * BLOCK_WIDTH;
						edge.ex = edge.sx + BLOCK_WIDTH;
						edge.ey = edge.sy;

						int edgeID = edges.size();
						edges.push_back(edge);

						world.setEdgeID(indexX, indexY, NORTH, edgeID);
						world.setEdgeExist(indexX, indexY, NORTH, true);
					}
				}

				// Check if southern neighbor exists
				if (world.getStatus(southX, southY) != OCCUPIED)
				{
					if (world.getEdgeExist(westX, westY, SOUTH))
					{
						edges[world.getEdgeID(westX, westY, SOUTH)].ex += BLOCK_WIDTH;
						world.setEdgeID(indexX, indexY, SOUTH, world.getEdgeID(westX, westY, SOUTH));
						world.setEdgeExist(indexX, indexY, SOUTH, true);
					}
					else
					{
						Edge edge;
						edge.sx = (x + sx) * BLOCK_WIDTH;
						edge.sy = (y + sy + 1) * BLOCK_WIDTH;
						edge.ex = edge.sx + BLOCK_WIDTH;
						edge.ey = edge.sy;

						int edgeID = edges.size();
						edges.push_back(edge);

						world.setEdgeID(indexX, indexY, SOUTH, edgeID);
						world.setEdgeExist(indexX, indexY, SOUTH, true);
					}
				}
			}
		}
	}
}

void calculateRays(std::vector<Ray>& rayPoints, std::vector<Edge> edges, float sourceX, float sourceY, float radius)
{
	for (auto& e1 : edges)
	{
		for (int i = 0; i < 2; i++)
		{
			// variables that represent ray vector
			float rdx, rdy;
			if (i == 0)
			{
				rdx = e1.sx - sourceX;
				rdy = e1.sy - sourceY;
			}
			else
			{
				rdx = e1.ex - sourceX;
				rdy = e1.ey - sourceY;
			}

			float baseAngle = atan2f(rdy, rdx);

			float angle = 0;

			// Cast three rays per point
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)
				{
					angle = baseAngle - 0.0001;
				}
				else if (j == 1)
				{
					angle = baseAngle;
				}
				else
				{
					angle = baseAngle + 0.0001;
				}

				// Create ray along angle
				rdx = radius * cosf(angle);
				rdy = radius * sinf(angle);

				float minT1 = INFINITY;
				float minX = 0, minY = 0, minAngle = 0;
				bool valid = false;

				// Check for intersections between rays and edges
				for (auto& e2 : edges)
				{
					// line segment vector
					float sdx = e2.ex - e2.sx;
					float sdy = e2.ey - e2.sy;

					// Make sure ray doesn't lie on top of line segment
					if (fabs(sdx - rdx) > 0.0 && fabs(sdy - rdy) > 0.0)
					{
						// Noramlized distance from line segment start to line segment end of intersect point
						float t2 = ((rdx * (e2.sy - sourceY)) + (rdy * (sourceX - e2.sx))) / (sdx * rdy - sdy * rdx);
						// Normalized distance from source along ray to ray length of intersect point
						float t1 = (e2.sx + sdx * t2 - sourceX) / rdx;

						if (t1 > 0 && t2 >= 0 && t2 <= 1)
						{
							if (t1 < minT1)
							{
								minT1 = t1;
								minX = sourceX + rdx * t1;
								minY = sourceY + rdy * t1;
								minAngle = atan2f(minY - sourceY, minX - sourceX);
								valid = true;
							}
						}
					}
				}

				// Add intersection point to ray points
				if (valid)
				{
					rayPoints.push_back(Ray(minAngle, minX, minY));
				}
			}

		}
	}

	// Sort rays by their angle
	sort(rayPoints.begin(), rayPoints.end(),
		[&](const Ray& t1, const Ray& t2)
		{
			return t1.getAngle() < t2.getAngle();
		});

	// Remove repeated rays
	for (int i = 0; i < rayPoints.size(); i++)
	{
		for (int j = i + 1; j < rayPoints.size(); j++)
		{
			if (fabs(rayPoints[i].getX() - rayPoints[j].getX()) < 0.1 && fabs(rayPoints[i].getY() - rayPoints[j].getY()) < 0.1)
			{
				rayPoints.erase(rayPoints.begin() + j);
			}
		}
	}
}
