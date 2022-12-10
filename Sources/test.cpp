#include <ppm.hpp>

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
	image.DrawLine(WHITE, 3);
	image.CompileImage();
	image.DrawLine(image.GetWidth(), image.GetHeight() / 2, image.GetWidth() / 2, image.GetHeight(), WHITE, 3);
	image.CompileImage();
	return 0;
}
