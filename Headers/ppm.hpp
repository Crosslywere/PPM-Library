#pragma once

#ifndef PPM_HPP
#define PPM_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace ppm
{
// A union that defines a color as either one large 32bit number or 4 8bit numbers
union Color
{
	uint32_t argb;
	struct { uint8_t b, g, r, a; };
	Color(uint32_t argb = 0)
		: argb{ argb }
	{
	}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r{ r }, g{ g }, b{ b }, a{ a }
	{
	}
};
// Stringifying the RGB values of Color c
std::string StrColor(const Color& c)
{
	return std::to_string((int)c.r) + " " + std::to_string((int)c.g) + " " + std::to_string((int)c.b);
}
// The Image class
class Image
{
private:
	std::string sName;
	uint32_t nWidth, nHeight;
	uint16_t nColorDepth;
	std::vector<Color> vBufferedImage;
	std::string sHeader;
public:
	Image(const std::string& name, const Color& base, uint32_t width, uint32_t height, uint16_t colorDepth = 255u)
		: sName{ name }, nWidth { width	}, nHeight{ height }
	{
		sHeader += "P3\n";
		sHeader += std::to_string(nWidth) + " " + std::to_string(nHeight) + "\n";
		sHeader += std::to_string(colorDepth) + "\n";
		for (int y = 0; y < nHeight; y++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				vBufferedImage.push_back(base);
			}
		}
	}
	// Destructor outputs final image
	~Image()
	{
		std::ofstream fs(sName);
		if (!fs.is_open())
		{
			std::cerr << "Failed to open " << sName << " for output.";
			return;
		}
		fs << sHeader;
		for (int y = 0; y < nHeight; y++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				fs << StrColor(vBufferedImage[x + y * nWidth]) << " ";
			}
			fs << "\n";
		}
	}
	inline void CompileImage() const
	{
		std::ofstream fs(sName);
		if (!fs.is_open())
		{
			std::cerr << "Failed to open " << sName << " for output.";
			return;
		}
		fs << sHeader;
		for (int y = 0; y < nHeight; y++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				fs << StrColor(vBufferedImage[x + y * nWidth]) << " ";
			}
			fs << "\n";
		}
	}
	inline void SetPixel(uint32_t x, uint32_t y, const Color& c)
	{
		if (x >= nWidth || y >= nHeight)
			return;

		vBufferedImage.at(x + y * nWidth) = c;
	}
	inline void DrawRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const Color& c)
	{
		for (int y = y1; y < y2; y++)
		{
			for (int x = x1; x < x2; x++)
			{
				SetPixel(x, y, c);
			}
		}
	}
	inline void DrawRect(const Color& c)
	{
		DrawRect(0, 0, nWidth, nHeight, c);
	}
	inline void DrawCheckeredPattern(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t xCount, uint32_t yCount, const Color& c1, const Color& c2)
	{
		int yInc = (y2 - y1) / yCount;
		int xInc = (x2 - x1) / xCount;
		bool c = true;
		for (int y = y1 + yInc; y < y2 + yInc; y += yInc)
		{
			c = !c;
			for (int x = x1 + xInc; x < x2 + xInc; x += xInc)
			{
				c = !c;
				DrawRect(x - xInc, y - yInc, x, y, (c ? c1 : c2));
			}
		}
	}
	inline void DrawCheckeredPattern(uint32_t xCount, uint32_t yCount, const Color& c1, const Color& c2)
	{
		DrawCheckeredPattern(0, 0, nWidth, nHeight, xCount,yCount, c1, c2);
	}
	inline void DrawHLine(uint32_t x1, uint32_t y1, uint32_t x2, const Color& c, uint32_t thickness = 1)
	{
		for (int y = y1; y < y1 + thickness; y++)
			for (int x = x1; x < x2; x++)
				SetPixel(x, y, c);
	}
	inline void DrawVLine(uint32_t x1, uint32_t y1, uint32_t y2, const Color& c, uint32_t thickness = 1)
	{
		for (int y = y1; y < y2; y++)
			for (int x = x1; x < x1 + thickness; x++)
				SetPixel(x, y, c);
	}
	inline void DrawEmptyRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const Color& c, uint32_t thickness = 1)
	{
		DrawHLine(x1, y1, x2 - thickness, c, thickness);
		DrawHLine(x1, y2 - thickness, x2, c, thickness);
		DrawVLine(x1, y1, y2 - thickness, c, thickness);
		DrawVLine(x2 - thickness, y1, y2, c, thickness);
	}
	inline void DrawEmptyRect(const Color& c, uint32_t thickness = 1)
	{
		DrawEmptyRect(0, 0, nWidth, nHeight, c, thickness);
	}
};

}

#endif // PPM_HPP