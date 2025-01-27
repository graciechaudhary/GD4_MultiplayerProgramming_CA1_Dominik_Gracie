#pragma once

#include <SFML/Graphics/Color.hpp>

class RGBColour
{
public:
	RGBColour();
	RGBColour(int r, int g, int b);

	int GetR() const;
	int GetG() const;
	int GetB() const;

	sf::Color GetColour() const;

	void addRed(int val);
	void addGreen(int val);
	void addBlue(int val);

private:
	int r;
	int g;
	int b;

};

