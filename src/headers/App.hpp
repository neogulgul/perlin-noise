#pragma once

#include <SFML/Graphics.hpp>

#include "Root.hpp"

struct App : Root
{
	int octaves = 4;
	float bias = 0.4;
	float biasStep = 0.2;
	std::vector<std::vector<float>> noiseMap, perlinNoiseMap;

	sf::Vector2i mapSize;

	sf::Vector2i tiles;

	int_dist mapXDist, mapYDist;

	bool islandMode = false;
	Press islandPress;

	bool R = 0;
	bool G = 0;
	bool B = 1;
	Press R_Press;
	Press G_Press;
	Press B_Press;

	Press resetPress;
	
	Press octavesIncreasePress;
	Press octavesDecreasePress;

	Press biasIncreasePress;
	Press biasDecreasePress;

	Press screenshotPress;

	sf::Vector2i closeViewCoord;
	Press closeViewPress;
	bool inCloseView = false;

	sf::Texture tilesetTexture;
	sf::Sprite tilesetSprite;

	App(unsigned int targetFPS, sf::Vector2f viewSize, sf::Vector2u windowSize, std::string windowTitle);

	virtual void update();

	virtual void draw();

	sf::Vector2i getMouseCoord();

	void placeCloseView(int x, int y);

	void generateNoiseMap();

	void perlinifyNoiseMap();

	void drawPerlinNoiseMap();
};
