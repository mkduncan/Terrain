/*********************************************************************************
** MIT License
**
** Copyright(c) 2018 duncanmk
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files(the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions :
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*********************************************************************************/

#include "Noise.hpp"
#include <chrono>
#include <random>

uint8_t Noise::Permutations[256], Noise::Octaves;
float Noise::Amplitude, Noise::Frequency, Noise::Lacunarity, Noise::Persistence, Noise::Scale, Noise::deltaX, Noise::deltaY, Noise::Fade;

static int64_t fastFloor(const float x)
{
	return x >= 0.0f ? static_cast<int64_t>(x) : static_cast<int64_t>(x) - 1;
}

static uint8_t getHashCode(const int64_t index)
{
	return Noise::Permutations[index & 255];
}

static float getGradient(const int64_t hash, const float x, const float y)
{
	const int64_t h = hash & 0x3F;
	const float u = h < 4 ? x : y, v = h < 4 ? y : x;

	return (((h & 1) ? (-u) : u) + ((h & 2) ? (-2.0f * v) : (2.0f * v)));
}

/*
	Modified version of 2D simplex noise generation
	from: https://github.com/SRombauts/SimplexNoise
*/
static float getSimplexNoise(const float x, const float y)
{
	static const float F2 = 0.3660254037844386f, G2 = 0.2113248654051871f, I3 = 0.5773502691896257f;
	float s, xs, ys, t, X0, Y0, x0, y0, x1, y1, x2, y2, t0, t1, t2, n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;
	int64_t i, j, i1, j1, gi0, gi1, gi2;
	
	s = (x + y) * F2;
	xs = x + s;
	ys = y + s;
	i = fastFloor(xs);
	j = fastFloor(ys);
	t = static_cast<float>(i + j) * G2;
	X0 = i - t;
	Y0 = j - t;
	x0 = x - X0;
	y0 = y - Y0;
	i1 = x0 > y0 ? 1 : 0;
	j1 = x0 > y0 ? 0 : 1;
	x1 = x0 - i1 + G2;
	y1 = y0 - j1 + G2;
	x2 = x0 - I3;
	y2 = y0 - I3;
	gi0 = getHashCode(i + getHashCode(j));
	gi1 = getHashCode(i + i1 + getHashCode(j + j1));
	gi2 = getHashCode(i + 1 + getHashCode(j + 1));
	t0 = 0.5f - (x0 * x0) - (y0 * y0);
	t1 = 0.5f - (x1 * x1) - (y1 * y1);
	t2 = 0.5f - (x2 * x2) - (y2 * y2);

	if(t0 >= 0.0f)
	{
		t0 *= t0;
		n0 = t0 * t0 * getGradient(gi0, x0, y0);
	}

	if(t1 >= 0.0f)
	{
		t1 *= t1;
		n1 = t1 * t1 * getGradient(gi1, x1, y1);
	}

	if(t2 >= 0.0f)
	{
		t2 *= t2;
		n2 = t2 * t2 * getGradient(gi2, x2, y2);
	}

	return (n0 + n1 + n2) * 47.63f;
}

void Noise::Reseed()
{
	std::mt19937_64 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	uint64_t swapIndex[2];
	uint8_t swapValue;

	for(int32_t index = 0; index < 256; ++index)
		Noise::Permutations[index] = (uint8_t)index;

	for(int32_t index = 0; index < 1024; ++index)
	{
		swapIndex[0] = generator() & 255;
		swapIndex[1] = generator() & 255;

		swapValue = Noise::Permutations[swapIndex[0]];
		Noise::Permutations[swapIndex[0]] = Noise::Permutations[swapIndex[1]];
		Noise::Permutations[swapIndex[1]] = swapValue;
	}

	Noise::Amplitude = 1.0f;
	Noise::Frequency = 1.0f;
	Noise::Lacunarity = 2.0f;
	Noise::Persistence = 0.5f;
	Noise::Scale = 0.1f;
	Noise::deltaX = 0.0f;
	Noise::deltaY = 0.0f;
	Noise::Fade = 1.0f;
	Noise::Octaves = 4;
}

/*
	Modified version of 2D fractional Brownian motion
	from: https://github.com/SRombauts/SimplexNoise
*/
float Noise::Generate(const float x, const float y)
{
	float sigma = 0.0f, divisor = 0.0f, frequency = Noise::Frequency, amplitude = Noise::Amplitude;

	for(uint8_t octave = 0; octave < Noise::Octaves; ++octave)
	{
		sigma += amplitude * getSimplexNoise((x + Noise::deltaX) * Noise::Scale * frequency, (y + Noise::deltaY) * Noise::Scale * frequency);
		divisor += amplitude;
		frequency *= Noise::Lacunarity;
		amplitude *= Noise::Persistence;
	}

	return sigma / (Noise::Fade * divisor);
}