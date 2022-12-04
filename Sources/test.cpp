#include <ppm.hpp>

#define RED    0xFF0000
#define ORANGE 0xFF8800
#define YELLOW 0xFFFF00
#define GREEN  0x008800
#define BLUE   0x0000FF
#define PURPLE 0xAA00FF

int main()
{
	// Setting up a 2x1 image.
	ppm::Image img("Image.ppm", NULL, 512, 256);
	img.CompileImage();
	// Drawing an empty rectangle.
	img.DrawEmptyRect(RED, 4);
	// Adding a horizontal line to the center of the image.
	img.DrawHLine(0, 128, 512, ORANGE);
	// Adding a vertical line to the center of the image.
	img.DrawVLine(256, 0, 256, ORANGE);
	img.CompileImage();
	// Drawing a filled rectangle.
	img.DrawRect(YELLOW);
	img.CompileImage();
	// Drawing a Checkered Pattern on the full image.
	img.DrawCheckeredPattern(8, 4, GREEN, PURPLE);
	return 0;
}
