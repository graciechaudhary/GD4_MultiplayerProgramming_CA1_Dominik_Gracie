#include "RGBColour.hpp"

RGBColour::RGBColour() : r(0), g(0), b(0)
{
}

RGBColour::RGBColour(int r, int g, int b)
	: r(r), g(g), b(b)
{
}

int RGBColour::GetR() const
{
    return r;
}

int RGBColour::GetG() const
{
    return g;
}

int RGBColour::GetB() const
{
    return b;
}

sf::Color RGBColour::GetColour() const
{
    return sf::Color(r,g,b);
}

void RGBColour::addRed(int val)
{
	r += val;
	if (r > 255)
	{
		r = r - 256;
	}

}

void RGBColour::addGreen(int val)
{
	g += val;
	if (g > 255)
	{
		g = g - 256;
	}
}

void RGBColour::addBlue(int val)
{
	b += val;
	if (b > 255)
	{
		b = b - 256;
	}
}
