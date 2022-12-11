#include "ppm.hpp"

#define WHITE  0xFFFFFF
#define RED    0xFF0000
#define ORANGE 0xFF8800
#define YELLOW 0xFFFF00
#define GREEN  0x008800
#define BLUE   0x0000FF
#define PURPLE 0xAA00FF
#define BLACK  NULL

int main()
{
	ppm::Image image("Image.ppm", BLACK, 400, 300);
	image.DrawBezier({ {image.GetWidth(), 0}, {0, image.GetHeight()}, {0, 0},}, PURPLE);
	return 0;
}
