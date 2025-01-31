//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "RGBColour.hpp"
//Dominik Hampejs D00250604
RGBColour::RGBColour() : r(220), g(220), b(220)
{
}
//Dominik Hampejs D00250604
RGBColour::RGBColour(int r, int g, int b)
	: r(r), g(g), b(b)
{
}
//Dominik Hampejs D00250604
int RGBColour::GetRed() const
{
    return r;
}
//Dominik Hampejs D00250604
int RGBColour::GetGreen() const
{
    return g;
}
//Dominik Hampejs D00250604
int RGBColour::GetBlue() const
{
    return b;
}
//Dominik Hampejs D00250604
sf::Color RGBColour::GetColour() const
{
    return sf::Color(r,g,b);
}

//Dominik Hampejs D00250604
//Add red value to the colour with a limit of 40 and 220
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

//Dominik Hampejs D00250604
//Add green value to the colour with a limit of 40 and 220
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

//Dominik Hampejs D00250604
//Add blue value to the colour with a limit of 40 and 220
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
