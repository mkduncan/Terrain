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

#include "Camera.hpp"
#include "Geometry.hpp"
#include "glm/gtc/matrix_access.hpp"

Plane Frustum::Planes[6];

void AlignedBox::Expand(const glm::vec3 &point, bool &initialize)
{
	if(initialize)
	{
		Minimum = point;
		Maximum = point;
		initialize = false;
	}

	else
	{
		Minimum = glm::vec3(glm::min(Minimum.x, point.x), glm::min(Minimum.y, point.y), glm::min(Minimum.z, point.z));
		Maximum = glm::vec3(glm::max(Maximum.x, point.x), glm::max(Maximum.y, point.y), glm::max(Maximum.z, point.z));
	}
}

bool AlignedBox::Test(const glm::vec3 &point) const
{
	return Minimum.x <= point.x && Minimum.y <= point.y && Minimum.z <= point.z && Maximum.x >= point.x && Maximum.y >= point.y && Maximum.z >= point.z;
}

const glm::vec3 AlignedBox::getCenter() const
{
	return (Minimum + Maximum) * 0.5f;
}

const glm::vec3 AlignedBox::getExtent() const
{
	return (Maximum - Minimum) * 0.5f;
}

void Plane::Normalize(const glm::vec4 &values)
{
	Values = values * glm::inversesqrt(values.x * values.x + values.y * values.y + values.z * values.z);
}

bool Plane::Test(const glm::vec3 &point) const
{
	return point.x * Values.x + point.y * Values.y + point.z * Values.z + Values.w > 0.0f;
}

void Frustum::Extract()
{
	const glm::vec4 rowX = glm::row(Camera::World, 0);
	const glm::vec4 rowY = glm::row(Camera::World, 1);
	const glm::vec4 rowZ = glm::row(Camera::World, 2);
	const glm::vec4 rowW = glm::row(Camera::World, 3);

	Planes[0].Normalize(rowW + rowX);
	Planes[1].Normalize(rowW - rowX);
	Planes[2].Normalize(rowW + rowY);
	Planes[3].Normalize(rowW - rowY);
	Planes[4].Normalize(rowW + rowZ);
	Planes[5].Normalize(rowW - rowZ);
}

bool Frustum::Test(const AlignedBox &box)
{
	bool validTest;
	const glm::vec3 center = box.getCenter(), extent = box.getExtent(), corners[8] =
	{
		glm::vec3(center.x - extent.x, center.y - extent.y, center.z - extent.z),
		glm::vec3(center.x + extent.x, center.y - extent.y, center.z - extent.z),
		glm::vec3(center.x - extent.x, center.y + extent.y, center.z - extent.z),
		glm::vec3(center.x + extent.x, center.y + extent.y, center.z - extent.z),
		glm::vec3(center.x - extent.x, center.y - extent.y, center.z + extent.z),
		glm::vec3(center.x + extent.x, center.y - extent.y, center.z + extent.z),
		glm::vec3(center.x - extent.x, center.y + extent.y, center.z + extent.z),
		glm::vec3(center.x + extent.x, center.y + extent.y, center.z + extent.z)
	};

	for(int32_t planeIndex = 0; planeIndex < 6; ++planeIndex)
	{
		validTest = false;

		for(int32_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
			if(Planes[planeIndex].Test(corners[cornerIndex]))
			{
				validTest = true;
				break;
			}

		if(!validTest)
			return false;
	}

	return true;
}