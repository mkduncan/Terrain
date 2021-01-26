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

#ifndef Geometry_H
#define Geometry_H

#include <glm.hpp>
#include <vector>

class AlignedBox
{
	public:
		glm::vec3 Minimum, Maximum;

		void Expand(const glm::vec3&, bool&);
		bool Test(const glm::vec3&) const;
		const glm::vec3 getCenter() const;
		const glm::vec3 getExtent() const;
};

class Plane
{
	public:
		glm::vec4 Values;

		void Normalize(const glm::vec4&);
		bool Test(const glm::vec3&) const;
};

class Frustum
{
	public:
		static Plane Planes[6];

		static void Extract();
		static bool Test(const AlignedBox&);
};

#endif