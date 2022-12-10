#pragma once

#ifndef PPM_HPP
#define PPM_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

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
private:
	// For handling complex lerping
	struct v2 { uint32_t x, y; };
	v2 lerp(v2 p1, v2 p2, double t)
	{
		return v2{ uint32_t(p1.x + t * (p2.x - p1.x)), uint32_t(p1.y + t * (p2.y - p1.y)) };
	}

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
	const uint32_t GetWidth() const
	{
		return nWidth;
	}
	const uint32_t GetHeight() const
	{
		return nHeight;
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
	inline void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const Color& c, uint32_t thickness = 1)
	{
		int dx = x2 - x1;
		int dy = y2 - y1;
		auto dist = std::sqrt((dx * dx) + (dy * dy));
		for (int offset = 0; offset <= thickness; offset++)
		{
			for (double t = 0; t <= dist; t++)
			{
				int x = int(x1 + offset + ((t / dist) * dx));
				int y = int(y1 + ((t / dist) * dy));
				SetPixel(x, y, c);
			}
		}
	}
	inline void DrawLine(const Color& c, uint32_t thickness = 1)
	{
		DrawLine(0, 0, nWidth, nHeight, c, thickness);
	}
/*	
	inline void DrawCubicBezier(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3, uint32_t x4, uint32_t y4, const Color& c, uint32_t thickness = 1)
	{
		auto dista = std::sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
		auto distb = std::sqrt(((x3 - x2) * (x3 - x2)) + ((y3 - y2) * (y3 - y2)));
		auto distc = std::sqrt(((x4 - x3) * (x4 - x3)) + ((y4 - y3) * (y4 - y3)));
		auto tdist = dista + distb + distc;
		std::cout << "Dist A: " << dista << "\nDist B: " << distb << "\nDist C: " << distc << "\nDist Total: " << tdist << std::endl;
		v2 p0{ x1, y1 };
		v2 p1{ x2, y2 };
		v2 p2{ x3, y3 };
		v2 p3{ x3, y3 };
		for (double t = 0; t <= dista; t++)
		{
			std::cout << "Percentage : " << (t / dista) * 100 << std::endl;
			v2 u = lerp(p0, p1, t / dista);
			v2 v = lerp(p1, p2, t / dista);
			v2 w = lerp(p2, p3, t / dista);
			v2 x = lerp(u, v, t / dista);
			v2 y = lerp(v, w, t / dista);
			v2 z = lerp(x, y, t / dista);
			SetPixel(z.x, z.y, c);
		}
		
	}
	inline void DrawCubicBezier(const Color& c, uint32_t thickness = 1)
	{
		DrawCubicBezier(0, nHeight, 0, 0, nWidth, 0, nWidth, nHeight, c, thickness);
	}
*/
	inline void DrawQuadBezier(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3, const Color& c, uint32_t thickness = 1)
	{
		auto dist1 = std::sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
		auto dist2 = std::sqrt(((x3 - x2) * (x3 - x2)) + ((y3 - y2) * (y3 - y2)));
		double tdist = std::sqrt(((x3 - x1) * (x3 - x1)) + ((y3 - y1) * (y3 - y1)));
		for (double t = 0; t <= tdist; t++)
		{
			double percent = t / tdist;
			v2 a = lerp({ x1, y1 }, { x2, y2 }, percent);
			v2 b = lerp({ x2, y2 }, { x3, y3 }, percent);
			double ndist = std::sqrt(((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)));
			v2 p = lerp(a, b, percent);
			std::cout << percent * ((dist1 + dist2) / ndist) << std::endl;
			SetPixel(p.x, p.y, c);
		}
	}
	inline void DrawQuadBezier(const Color& c, uint32_t thickness = 1)
	{
		DrawQuadBezier(0, nHeight, nWidth / 2, 0, nWidth, nHeight, c, thickness);
	}
};

}

#endif // PPM_HPP