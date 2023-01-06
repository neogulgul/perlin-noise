#pragma once

#define initialDefaultTextColor sf::Color(255, 255, 255, 191)
#define initialDefaultTextScale sf::Vector2f(1, 1)

const int fontSize = 10;
const int fontPixelDeviation = 0; // some fonts have some "deviation" ¯\_(ツ)_/¯
const char fontPath[] = "assets/fonts/Poco.ttf";

class Text
{
	private:
	sf::RenderWindow* window;
	sf::Text text;
	sf::Font font;

	sf::Color    defaultTextColor;
	sf::Vector2f defaultTextScale;

	public:
	Text()
	{
		loadPixelatedFont(font, fontSize, fontPath);
		setDefaultTextColor(initialDefaultTextColor);
		setDefaultTextScale(initialDefaultTextScale);
	}

	void loadPixelatedFont(sf::Font &font, int fontSize, const char fontPath[])
	{
		font.loadFromFile(fontPath);
		sf::Texture& fontTexture = const_cast<sf::Texture&>(font.getTexture(fontSize));
		fontTexture.setSmooth(false);
	}

	void init(sf::RenderWindow* window)
	{
		this->window = window;
	}

	void setDefaultTextColor(sf::Color color) { defaultTextColor = color; }

	sf::Color getDefaultTextColor() { return defaultTextColor; }

	void setDefaultTextScale(sf::Vector2f scale) { defaultTextScale = scale; }

	sf::Vector2f getDefaultTextScale() { return defaultTextScale; }

	float getDefaultTextHeight() { return fontSize * defaultTextScale.y / 2; }

	float getTextHeight(float yScale) { return fontSize * defaultTextScale.y * yScale / 2; }

	void draw(std::string string, Alignment horizontalAlignment, Alignment verticalAlignment, sf::Vector2f position, sf::Color color, sf::Vector2f scale = { 1, 1 })
	{
		text.setString(string);
		text.setFont(font);
		text.setCharacterSize(fontSize);
		text.setFillColor(color);
		text.setScale(defaultTextScale.x * scale.x, defaultTextScale.y * scale.y);
		text.setOrigin(0, text.getCharacterSize() / 2 + fontPixelDeviation);

		sf::FloatRect textBounds = text.getGlobalBounds();
		sf::Vector2f size(textBounds.width, textBounds.height);

		position = align(size, horizontalAlignment, verticalAlignment, position);

		text.setPosition(position);

		window->draw(text);
	}

	void draw(std::string string, Alignment horizontalAlignment, Alignment verticalAlignment, sf::Vector2f position, sf::Vector2f scale)
	{
		draw(string, horizontalAlignment, verticalAlignment, position, defaultTextColor, scale);
	}

	void draw(std::string string, Alignment horizontalAlignment, Alignment verticalAlignment, sf::Vector2f position)
	{
		draw(string, horizontalAlignment, verticalAlignment, position, defaultTextColor);
	}
};
