#pragma once

#include <SFML/Graphics.hpp>

#include "Root.hpp"

const int mapWidth  = viewWidth;
const int mapHeight = viewHeight;

struct App : Root
{
	int octaves = 4;
	float bias = 0.4;
	float biasStep = 0.2;
	float noiseMap[mapWidth][mapHeight];
	float perlinNoiseMap[mapWidth][mapHeight];

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

	App();

	virtual void update();

	virtual void draw();

	void generateNoiseMap();

	void perlinifyNoiseMap();

	void drawPerlinNoiseMap();
};
