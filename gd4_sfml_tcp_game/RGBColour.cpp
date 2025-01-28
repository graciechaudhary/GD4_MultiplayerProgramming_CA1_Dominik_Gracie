#include "RGBColour.hpp"

RGBColour::RGBColour() : r(0), g(0), b(0)
{
}

RGBColour::RGBColour(int r, int g, int b)
	: r(r), g(g), b(b)
{
}

int RGBColour::GetRed() const
{
    return r;
}

int RGBColour::GetGreen() const
{
    return g;
}

int RGBColour::GetBlue() const
{
    return b;
}

sf::Color RGBColour::GetColour() const
{
    return sf::Color(r,g,b,50);
}

void RGBColour::addRed(int val)
{
	r += val;
	if (r > 255)
	{
		r = r - 256;
	}
	if (r < 0)
	{
		r = 256 + r;
	}

}

void RGBColour::addGreen(int val)
{
	g += val;
	if (g > 255)
	{
		g = g - 256;
	}
	if (g < 0)
	{
		g = 256 + g;
	}
}

void RGBColour::addBlue(int val)
{
	b += val;
	if (b > 255)
	{
		b = b - 256;
	}
	if (b < 0)
	{
		b = 256 + b;
	}
}
