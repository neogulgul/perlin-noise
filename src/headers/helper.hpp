#pragma once

#define int_dist std::uniform_int_distribution<int>

const float PI = std::numbers::pi;

using key = sf::Keyboard;
using button = sf::Mouse;

struct Press
{
	bool pressedThisFrame = false;
	bool pressedLastFrame = false;
};

enum Alignment { Start, Center, End };

float degToRad(float deg);

float radToDeg(float rad);

void toggle(bool &boolean);

bool pressing(sf::Keyboard::Key key);

bool pressing(sf::Mouse::Button button);

void handlePress(bool pressing, Press &press);

std::string boolToString(bool boolean);

sf::Vector2f align(sf::Vector2f size, Alignment horizontalAlignment, Alignment verticalAlignment, sf::Vector2f position);
