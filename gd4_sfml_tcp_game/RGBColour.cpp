//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "RGBColour.hpp"

RGBColour::RGBColour() : r(220), g(220), b(220)
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
    return sf::Color(r,g,b);
}

void RGBColour::addRed(int val)
{
	r += val;
	if (r > 220)
	{
		r = r - (220 - val);
	}
	if (r < 40)
	{
		r = (220 + val) + r;
	}

}

void RGBColour::addGreen(int val)
{
	g += val;
	if (g > 220)
	{
		g = g - (220 - val);
	}
	if (g < 40)
	{
		g = (220 + val) + g;
	}
}

void RGBColour::addBlue(int val)
{
	b += val;
	if (b > 220)
	{
		b = b - (220 - val);
	}
	if (b < 40)
	{
		b = (220 + val) + b;
	}

}
