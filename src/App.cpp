#include "headers/App.hpp"

#define RGB_IS_WHITE_TEXT_COLOR sf::Color::Magenta

#define tilesize 16

App::App(unsigned int targetFPS, sf::Vector2f viewSize, sf::Vector2u windowSize, std::string windowTitle)
: Root(targetFPS, viewSize, windowSize, windowTitle)
{
	mapSize.x = viewSize.x;
	mapSize.y = viewSize.y;

	for (int x = 0; x < viewSize.x; x++)
	{
		noiseMap.push_back({});
		perlinNoiseMap.push_back({});
		for (int y = 0; y < viewSize.y; y++)
		{
			noiseMap[x].push_back(0.0f);
			perlinNoiseMap[x].push_back(0.0f);
		}
	}

	mapXDist = int_dist(0, mapSize.x - 1);
	mapYDist = int_dist(0, mapSize.y - 1);

	generateNoiseMap();
	perlinifyNoiseMap();

	tiles.x = mapSize.x / tilesize;
	tiles.y = mapSize.y / tilesize;

	tilesetTexture.loadFromFile("assets/textures/tileset.png");
	tilesetSprite.setTexture(tilesetTexture);
}

void App::update()
{
	if (!window.hasFocus()) { return; }

	updateMousePosition();
	placeCloseView(getMouseCoord().x, getMouseCoord().y);

	if (islandMode)
	{
		handlePress(pressing(keyboard::Space), closeViewPress);
	}

	if (!inCloseView)
	{
		handlePress(pressing(keyboard::T), islandPress);

		handlePress(pressing(keyboard::Num1), R_Press);
		handlePress(pressing(keyboard::Num2), G_Press);
		handlePress(pressing(keyboard::Num3), B_Press);

		handlePress(pressing(keyboard::R), resetPress);

		handlePress(pressing(keyboard::Q), octavesIncreasePress);
		handlePress(pressing(keyboard::A), octavesDecreasePress);

		handlePress(pressing(keyboard::W), biasIncreasePress);
		handlePress(pressing(keyboard::S), biasDecreasePress);

		handlePress(pressing(keyboard::P), screenshotPress);

		if (islandPress.pressedThisFrame) { toggle(islandMode); }

		if (!islandMode)
		{
			if (R_Press.pressedThisFrame) { toggle(R); }
			if (G_Press.pressedThisFrame) { toggle(G); }
			if (B_Press.pressedThisFrame) { toggle(B); }
		}
		bool RGB_IS_WHITE = R && G && B;
		if (!islandMode && RGB_IS_WHITE && text.getDefaultTextColor() == initialDefaultTextColor) { text.setDefaultTextColor(RGB_IS_WHITE_TEXT_COLOR); }
		else if ((!RGB_IS_WHITE || islandMode) && text.getDefaultTextColor() != initialDefaultTextColor) { text.setDefaultTextColor(initialDefaultTextColor); }

		int oldOctaves = octaves;
		float oldBias = bias;

		if (octavesIncreasePress.pressedThisFrame) { octaves++; }
		if (octavesDecreasePress.pressedThisFrame) { octaves--; }
		if (octaves == 0) { octaves = 9; }
		if (octaves == 10) { octaves = 1; }

		if (biasIncreasePress.pressedThisFrame) { bias += biasStep; }
		if (biasDecreasePress.pressedThisFrame) { bias -= biasStep; }
		if (bias < biasStep) { bias = biasStep; }

		if (octaves != oldOctaves || bias != oldBias) { perlinifyNoiseMap(); }

		if (resetPress.pressedThisFrame)
		{
			generateNoiseMap();
			perlinifyNoiseMap();
		}
	}

	if (closeViewPress.pressedThisFrame)
	{
		toggle(inCloseView);
	}

	if (screenshotPress.pressedThisFrame)
	{
		takeScreenshot = true;
	}
}

void App::draw()
{
	#define padding 5

	static std::stringstream ss;

	drawPerlinNoiseMap();
	text.draw("FPS: " + std::to_string(FPS), Start, Start, { padding, padding });
	text.draw("Average FPS(last " + std::to_string(maxCountPastFPS) + " frames): " + std::to_string(averageFPS), Start, Start, { padding, text.getDefaultTextHeight() + padding * 2 });

	if (!inCloseView)
	{
		ss << "Generate new map[R]";
		text.draw(ss.str(), Start, End, {padding, viewSize.y - text.getDefaultTextHeight() * 3 - padding * 4});
		ss.str("");
		ss << "Island mode[T]: " << boolToString(islandMode);
		text.draw(ss.str(), Start, End, {padding, viewSize.y - text.getDefaultTextHeight() * 2 - padding * 3});
		ss.str("");
		ss << "Octaves (++[Q], --[A]): " << octaves;
		text.draw(ss.str(), Start, End, {padding, viewSize.y - text.getDefaultTextHeight() * 1 - padding * 2});
		ss.str("");
		ss << "Bias (++[W], --[S]): " << bias;
		text.draw(ss.str(), Start, End, {padding, viewSize.y - text.getDefaultTextHeight() * 0 - padding * 1});
		ss.str("");
	}

	if (islandMode)
	{
		text.draw("Toggle close view[Space]", Center, End, {viewSize.x / 2, viewSize.x - padding});

		if (inCloseView)
		{
		}
		else
		{
			Alignment xAlignment = Start;
			Alignment yAlignment = Start;
			float xAdjustment = 10;
			float yAdjustment = 10;
			if (closeViewCoord.x >= mapSize.x / 2)
			{
				xAlignment = End;
				xAdjustment *= -1;
			}
			if (closeViewCoord.y >= mapSize.y / 2)
			{
				yAlignment = End;
				yAdjustment *= -1;
			}
			ss << "(" << closeViewCoord.x << ", " << closeViewCoord.y << ")";
			text.draw(ss.str(), xAlignment, yAlignment, {(float)closeViewCoord.x + xAdjustment, (float)closeViewCoord.y + yAdjustment});
			ss.str("");
		}
	}
	else
	{
		ss << "RGB(R[1], G[2], B[3]): " << R << G << B;
		text.draw(ss.str(), End, End, {viewSize.x - padding, viewSize.y - text.getDefaultTextHeight() * 0 - padding * 1});
		ss.str("");
	}
}

sf::Vector2i App::getMouseCoord()
{
	return sf::Vector2i(std::floor(mousePosition.x), std::floor(mousePosition.y));
}

void App::placeCloseView(int x, int y)
{
	closeViewCoord.x = std::clamp(x, 0, mapSize.x);
	closeViewCoord.y = std::clamp(y, 0, mapSize.y);
}

void App::generateNoiseMap()
{
	int_dist noiseDist(0, 100);
	for (int x = 0; x < mapSize.x; x++)
	{
		for (int y = 0; y < mapSize.y; y++)
		{
			float value = noiseDist(rng) / 100.f;
			noiseMap[x][y] = value;
		}
	}
}

void App::perlinifyNoiseMap() // don't know how this works, but it does, which is fine for me :-) (source: https://www.youtube.com/watch?v=6-0UaeJBumA)
{
	for (int x = 0; x < mapSize.x; x++)
	{
		for (int y = 0; y < mapSize.y; y++)
		{
			float noise    = 0.0;
			float scaleAcc = 0.0;
			float scale    = 1.0;

			for (int o = 0; o < octaves; o++)
			{
				int pitch = mapSize.x >> o;

				int sampleX1 = (x / pitch) * pitch;
				int sampleY1 = (y / pitch) * pitch;

				int sampleX2 = (sampleX1 + pitch) % mapSize.x;
				int sampleY2 = (sampleY1 + pitch) % mapSize.x;

				float blendX = (float)(x - sampleX1) / (float)pitch;
				float blendY = (float)(y - sampleY1) / (float)pitch;

				float sampleT = (1.0f - blendX) * noiseMap[sampleX1][sampleY1] + blendX * noiseMap[sampleX2][sampleY1];
				float sampleB = (1.0f - blendX) * noiseMap[sampleX1][sampleY2] + blendX * noiseMap[sampleX2][sampleY2];

				noise    += (blendY * (sampleB - sampleT) + sampleT) * scale;
				scaleAcc += scale;
				scale    /= bias;
			}

			float newValue = noise / scaleAcc;

			perlinNoiseMap[x][y] = newValue;
		}
	}
}

void App::drawPerlinNoiseMap()
{
	if (inCloseView)
	{
		int xStart = std::clamp(closeViewCoord.x - tiles.x / 2, 0, mapSize.x - tiles.x);
		int yStart = std::clamp(closeViewCoord.y - tiles.y / 2, 0, mapSize.y - tiles.y);
		for (int x = 0; x < tiles.x; x++)
		{
			for (int y = 0; y < tiles.y; y++)
			{
				float value = perlinNoiseMap[xStart + x][yStart + y];
				sf::Vector2i cropPosition;

				if (value < 0.30) // deep water
				{
					cropPosition.x = 0;
					cropPosition.y = 0;
				}
				else if (value < 0.50) // shallow water
				{
					cropPosition.x = 1;
					cropPosition.y = 0;
				}
				else if (value < 0.525) // beach dark
				{
					cropPosition.x = 2;
					cropPosition.y = 0;
				}
				else if (value < 0.55) // beach light
				{
					cropPosition.x = 3;
					cropPosition.y = 0;
				}
				else if (value < 0.75) // grass light
				{
					cropPosition.x = 4;
					cropPosition.y = 0;
				}
				else if (value < 0.80) // grass dark
				{
					cropPosition.x = 4;
					cropPosition.y = 1;
				}
				else if (value < 0.825) // mountain base
				{
					cropPosition.x = 3;
					cropPosition.y = 1;
				}
				else if (value < 0.90) // mountain middle
				{
					cropPosition.x = 2;
					cropPosition.y = 1;
				}
				else if (value < 0.95) // mountain top
				{
					cropPosition.x = 1;
					cropPosition.y = 1;
				}
				else // mountain snow
				{
					cropPosition.x = 0;
					cropPosition.y = 1;
				}

				sf::IntRect crop({cropPosition.x * tilesize, cropPosition.y * tilesize}, {tilesize, tilesize});
				tilesetSprite.setTextureRect(crop);
				tilesetSprite.setPosition(x * tilesize, y * tilesize);

				window.draw(tilesetSprite);
			}
		}
	}
	else
	{
		sf::Image image;
		sf::Texture texture;
		sf::Sprite sprite;

		image.create(mapSize.x, mapSize.y);
		texture.create(mapSize.x, mapSize.y);

		for (int x = 0; x < mapSize.x; x++)
		{
			for (int y = 0; y < mapSize.y; y++)
			{
				float value = perlinNoiseMap[x][y];
				sf::Color color;
				if (islandMode)
				{
					if (value < 0.30) // deep water
					{
						color = sf::Color(0x3e3b65ff);
					}
					else if (value < 0.50) // shallow water
					{
						color = sf::Color(0x3859b3ff);
					}
					else if (value < 0.525) // beach dark
					{
						color = sf::Color(0xce9248ff);
					}
					else if (value < 0.55) // beach light
					{
						color = sf::Color(0xdab163ff);
					}
					else if (value < 0.75) // grass light
					{
						color = sf::Color(0x5ab552ff);
					}
					else if (value < 0.80) // grass dark
					{
						color = sf::Color(0x26854cff);
					}
					else if (value < 0.825) // mountain base
					{
						color = sf::Color(0xb0a7b8ff);
					}
					else if (value < 0.90) // mountain middle
					{
						color = sf::Color(0x8c78a5ff);
					}
					else if (value < 0.95) // mountain top
					{
						color = sf::Color(0x5e5b8cff);
					}
					else // mountain snow
					{
						color = sf::Color(0xffffffff);
					}
				}
				else
				{
					color.r = std::ceil(256.f * value) * R;
					color.g = std::ceil(256.f * value) * G;
					color.b = std::ceil(256.f * value) * B;
				}

				image.setPixel(x, y, color);
			}
		}

		texture.update(image);
		sprite.setTexture(texture);
		window.draw(sprite);
	}
}
