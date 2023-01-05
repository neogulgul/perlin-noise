#include <sstream>

#include "headers/App.hpp"

#define RGB_IS_WHITE_TEXT_COLOR sf::Color::Magenta

App::App()
{
	generateNoiseMap();
	perlinifyNoiseMap();
}

void App::update()
{
	if (!window.hasFocus()) { return; }

	int oldOctaves = octaves;
	float oldBias = bias;

	handlePress(pressing(key::T), islandPress);
	handlePress(pressing(key::R), resetPress);
	handlePress(pressing(key::Q), octavesIncreasePress);
	handlePress(pressing(key::A), octavesDecreasePress);
	handlePress(pressing(key::W), biasIncreasePress);
	handlePress(pressing(key::S), biasDecreasePress);
	handlePress(pressing(key::Num1), R_Press);
	handlePress(pressing(key::Num2), G_Press);
	handlePress(pressing(key::Num3), B_Press);

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

	updateFPS();
	updateMousePosition();
}

#define padding 5

void App::draw()
{
	drawPerlinNoiseMap();
	drawFPS({ padding, padding }, text.getDefaultTextColor(), text.getDefaultTextScale());

	std::stringstream ss;
	ss << "Generate new map[R]";
	text.draw(ss.str(), Start, End, {padding, viewHeight - text.getDefaultTextHeight() * 3 - padding * 4});
	ss.str("");
	ss << "Island mode[T]: " << boolToString(islandMode);
	text.draw(ss.str(), Start, End, {padding, viewHeight - text.getDefaultTextHeight() * 2 - padding * 3});
	ss.str("");
	ss << "Octaves (++[Q], --[A]): " << octaves;
	text.draw(ss.str(), Start, End, {padding, viewHeight - text.getDefaultTextHeight() * 1 - padding * 2});
	ss.str("");
	ss << "Bias (++[W], --[S]): " << bias;
	text.draw(ss.str(), Start, End, {padding, viewHeight - text.getDefaultTextHeight() * 0 - padding * 1});
	ss.str("");
	if (!islandMode)
	{
		ss << "RGB(R[1], G[2], B[3]): " << R << G << B;
		text.draw(ss.str(), End, End, {viewWidth - padding, viewHeight - text.getDefaultTextHeight() * 0 - padding * 1});
		ss.str("");
	}
}

void App::generateNoiseMap()
{
	int_dist noiseDist(0, 100);
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			float value = noiseDist(rng) / 100.f;
			noiseMap[x][y] = value;
		}
	}
}

void App::perlinifyNoiseMap() // don't know how this works, but it does, which is fine for me :-) (source: https://www.youtube.com/watch?v=6-0UaeJBumA)
{
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			float noise    = 0.0;
			float scaleAcc = 0.0;
			float scale    = 1.0;

			for (int o = 0; o < octaves; o++)
			{
				int pitch = mapWidth >> o;

				int sampleX1 = (x / pitch) * pitch;
				int sampleY1 = (y / pitch) * pitch;

				int sampleX2 = (sampleX1 + pitch) % mapWidth;
				int sampleY2 = (sampleY1 + pitch) % mapWidth;

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
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
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

			drawPixel(x, y, color);
		}
	}
}
