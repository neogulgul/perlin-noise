#include <SFML/Graphics.hpp>

#include "headers/helper.hpp"

float degToRad(float deg)
{
	return deg * (PI / 180);
}

float radToDeg(float rad)
{
	return rad * (180 / PI);
}

void toggle(bool &boolean)
{
	if (boolean)
	{
		boolean = false;
	}
	else
	{
		boolean = true;
	}
}

bool pressing(sf::Keyboard::Key key)
{
	return sf::Keyboard::isKeyPressed(key);
}

bool pressing(sf::Mouse::Button button)
{
	return sf::Mouse::isButtonPressed(button);
}

void handlePress(bool pressing, Press &press)
{
	press.pressedThisFrame = false;

	if (pressing)
	{
		if (!press.pressedLastFrame)
		{
			press.pressedLastFrame = true;
			press.pressedThisFrame = true;
		}
	}
	else
	{
		press.pressedLastFrame = false;
	}
}

std::string boolToString(bool boolean)
{
	if (boolean) { return "True"; }
	return "False";
}

sf::Vector2f align(sf::Vector2f size, Alignment horizontalAlignment, Alignment verticalAlignment, sf::Vector2f position)
{
	switch (horizontalAlignment)
	{
		case Start:
			position.x -= 0;
			break;

		case Center:
			position.x -= size.x / 2;
			break;

		case End:
			position.x -= size.x;
			break;
	}

	switch (verticalAlignment)
	{
		case Start:
			position.y -= 0;
			break;

		case Center:
			position.y -= size.y / 2;
			break;

		case End:
			position.y -= size.y;
			break;
	}

	return position;
}
