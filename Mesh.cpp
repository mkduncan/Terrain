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

#include "Mesh.hpp"
#include "Noise.hpp"
#include <glm.hpp>
#include <vector>

static const GLuint Width = 256, Depth = 256;
static const float Scale = 32.0f;

static GLuint Reindex(const GLuint row, const GLuint column, const GLuint rowLength)
{
	return row * rowLength + column;
}

void Mesh::Generate(const GLfloat offsetX, const GLfloat offsetZ)
{
	std::vector<glm::vec3> positionBatch, colorBatch, positionData, colorData;
	GLint index = 0;
	bool initializeBox = true;

	vertexCount = Width * Depth;
	positionBatch.resize(vertexCount);
	colorBatch.resize(vertexCount);
	vertexCount *= 6;
	positionData.resize(vertexCount);
	colorData.resize(vertexCount);

	for(GLuint indexR = 0; indexR < Width; ++indexR)
		for(GLuint indexC = 0; indexC < Depth; ++indexC)
		{
			positionBatch[index].x = (static_cast<float>(indexR) / static_cast<float>(Width - 1) + offsetX) * Scale;
			positionBatch[index].z = (static_cast<float>(indexC) / static_cast<float>(Depth - 1) + offsetZ) * Scale;
			positionBatch[index].y = Noise::Generate(positionBatch[index].x, positionBatch[index].z);
			colorBatch[index] = glm::vec3((positionBatch[index].y + 1.0f) * 0.5f);
			Bounds.Expand(positionBatch[index++], initializeBox);
		}

	Bounds.Minimum.y -= 2.0f;
	Bounds.Maximum.y += 2.0f;
	Offset[0] = offsetX;
	Offset[1] = offsetZ;
	index = 0;
	
	for(GLuint indexR = 0; indexR < Width - 1; ++indexR)
		for(GLuint indexC = 0; indexC < Depth - 1; ++indexC)
		{
			positionData[index] = positionBatch[Reindex(indexR, indexC, Width)];
			colorData[index++] = colorBatch[Reindex(indexR, indexC, Width)];
			positionData[index] = positionBatch[Reindex(indexR + 1, indexC, Width)];
			colorData[index++] = colorBatch[Reindex(indexR + 1, indexC, Width)];
			positionData[index] = positionBatch[Reindex(indexR + 1, indexC + 1, Width)];
			colorData[index++] = colorBatch[Reindex(indexR + 1, indexC + 1, Width)];
			positionData[index] = positionBatch[Reindex(indexR + 1, indexC + 1, Width)];
			colorData[index++] = colorBatch[Reindex(indexR + 1, indexC + 1, Width)];
			positionData[index] = positionBatch[Reindex(indexR, indexC + 1, Width)];
			colorData[index++] = colorBatch[Reindex(indexR, indexC + 1, Width)];
			positionData[index] = positionBatch[Reindex(indexR, indexC, Width)];
			colorData[index++] = colorBatch[Reindex(indexR, indexC, Width)];
		}

	positionBatch.clear();
	colorBatch.clear();
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	glGenBuffers(2, vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), &positionData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), &colorData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	positionData.clear();
	colorData.clear();
}

void Mesh::Render()
{
	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Mesh::Destroy()
{
	glDeleteBuffers(2, vboId);
	glDeleteVertexArrays(1, &vaoId);
}