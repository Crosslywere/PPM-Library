#pragma once

#ifndef PPM_HPP
#define PPM_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <array>

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
Color operator/(const Color& c, uint8_t s)
{
	return Color(c.r / s, c.g / s, c.b / s, c.a / s);
}
// Stringifying the RGB values of Color c
std::string StrColor(const Color& c)
{
	return std::to_string((int)c.r) + " " + std::to_string((int)c.g) + " " + std::to_string((int)c.b);
}

template<typename T>
struct Vector2
{
	T x, y;
	Vector2(T x = 0, T y = 0)
		: x{ x }, y{ y }
	{
	}
	template<typename Ty>
	Vector2(const Vector2<Ty> v)
		: x{ v.x }, y{ v.y }
	{
	}
	template<typename Ty>
	Vector2 operator+(const Vector2<Ty>& v)
	{
		return Vector2{ x + v.y, y + v.y };
	}
	Vector2 operator*(double s)
	{
		return Vector2{ T(x * s), T(y * s) };
	}
	template<typename Ty>
	void operator+=(const Vector2<Ty>& v)
	{
		x += v.x;
		y += v.y;
	}
};

typedef Vector2<int>      Vector2i;
typedef Vector2<uint32_t> Vector2u;
typedef Vector2<float>    Vector2f;
typedef Vector2<double>   Vector2d;

uint32_t binomialCoefficent(uint32_t n, uint32_t k)
{
	if (k > n)
		return 0;
	if (k == 0 || n == k)
		return 1;

	return binomialCoefficent(n - 1, k - 1) + binomialCoefficent(n - 1, k);
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
	inline void SetPixel(Vector2u p, const Color& c)
	{
		SetPixel(p.x, p.y, c);
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
	inline void DrawRect(Vector2u p1, Vector2u p2, const Color& c)
	{
		DrawRect(p1.x, p1.y, p2.x, p2.y, c);
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
	inline void DrawCheckeredPattern(Vector2u p1, Vector2u p2, uint32_t xCount, uint32_t yCount, const Color& c1, const Color& c2)
	{
		DrawCheckeredPattern(p1.x, p1.y, p2.x, p2.y, xCount, yCount, c1, c2);
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
	inline void DrawEmptyRect(Vector2u p1, Vector2u p2, const Color& c, uint32_t thickness = 1)
	{
		DrawEmptyRect(p1.x, p1.y, p2.x, p2.y, c, thickness);
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
	inline void DrawBezier(std::vector<Vector2u> controlPoints, const Color& c, uint32_t thickness = 1)
	{
		for (double t = 0; t <= 1.0; t += 0.00001)
		{
			Vector2u p;
			int n = controlPoints.size() - 1;
			for (int i = 0; i <= n; i++)
			{
				double bernsteinCoeff = binomialCoefficent(n, i) * std::pow(1 - t, n - 1) * std::pow(t, i);
				p += controlPoints[i] * bernsteinCoeff;
			}
			SetPixel(p, c);
		}
	}
	inline void DrawBezier(const Color& c, uint32_t thickness = 1)
	{
		DrawBezier({ { 0, nHeight }, { 0, 0 }, { nWidth, 0 }, { nWidth, nHeight } }, c, thickness);
	}
};

}

#endif // PPM_HPP