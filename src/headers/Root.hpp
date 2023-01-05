#pragma once

#include <cmath>
#include <random>

#include "helper.hpp"
#include "Text.hpp"
#include "window.hpp"

struct Root
{
	sf::RenderWindow window;
	sf::View view;
	sf::Event event;

	sf::VertexArray pixel;

	float     updateFPSDelta = 0.1; // the time between FPS counter update, in seconds
	sf::Clock updateFPSClock;       // used to only update the FPS counter after a certain amount of time has passed, to make it more readable
	sf::Clock   calcFPSClock;       // used to calculate FPS
	sf::Clock   displayClock;       // the time it takes the window to display everything drawn which is then subtracted when calculating FPS
	int FPS;

	sf::Vector2f mousePosition;

	std::mt19937 rng;
	int_dist coinFlip;

	Text text;

	Root()
	{
		window.create({windowWidth, windowHeight}, windowTitle, sf::Style::Default);
		window.setFramerateLimit(targetFPS);
		view.setCenter(viewWidth / 2, viewHeight / 2);
		view.setSize(viewWidth, viewHeight);

		pixel.setPrimitiveType(sf::Quads);
		pixel.resize(4);

		rng.seed(std::random_device{}());
		coinFlip = int_dist(0, 1);

		text.init(&window);
	}

	void run()
	{
		while (window.isOpen())
		{
			handleWindowEvents();

			window.clear();

			update();

			window.setView(view);

			draw();

			displayClock.restart();

			window.display();
		}
	}

	void handleWindowEvents()
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
			}
		}
	}

	void updateMousePosition()
	{
		mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	}

	void updateFPS()
	{
		if (updateFPSClock.getElapsedTime().asSeconds() >= updateFPSDelta)
		{
			updateFPSClock.restart();
			FPS = std::floor(1.f / (calcFPSClock.getElapsedTime().asSeconds() - displayClock.getElapsedTime().asSeconds()));
		}
		calcFPSClock.restart();
	}

	void drawFPS(sf::Vector2f position, sf::Color color, sf::Vector2f scale)
	{
		text.draw("FPS:" + std::to_string(FPS), Start, Start, position, color, scale);
	}

	void drawPixel(int x, int y, sf::Color color) // this function is terribly inefficient on a large scale, but it works
	{
		if (x < 0 || x >= viewWidth || y < 0 || y >= viewHeight) { return; } // out of view bounds

		pixel[0].position = { (float)x    , (float)y + 1 };
		pixel[1].position = { (float)x    , (float)y     };
		pixel[2].position = { (float)x + 1, (float)y     };
		pixel[3].position = { (float)x + 1, (float)y + 1 };
		for (int i = 0; i < 4; i++) { pixel[i].color = color; }
		window.draw(pixel);
	}

	virtual void update() {}

	virtual void draw() {}
};
