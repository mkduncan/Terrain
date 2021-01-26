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
#include "Noise.hpp"
#include "Terrain.hpp"

std::vector<Mesh> Terrain::Nodes;

void Terrain::Generate()
{
	Noise::Reseed();
	Nodes.resize(9);

	Nodes[0].Generate(-1.5f, -1.5f);
	Nodes[1].Generate(-1.5f, -0.5f);
	Nodes[2].Generate(-1.5f, 0.5f);
	Nodes[3].Generate(-0.5f, -1.5f);
	Nodes[4].Generate(-0.5f, -0.5f);
	Nodes[5].Generate(-0.5f, 0.5f);
	Nodes[6].Generate(0.5f, -1.5f);
	Nodes[7].Generate(0.5f, -0.5f);
	Nodes[8].Generate(0.5f, 0.5f);
}

void Terrain::Update()
{
	bool overlap[8] = { false, false, false, false, false, false, false, false };
	Frustum::Extract();

	for(size_t index = 0; index < Nodes.size(); ++index)
		if(Frustum::Test(Nodes[index].Bounds))
			Nodes[index].Render();

	if(!Nodes[4].Bounds.Test(Camera::Position))
	{
		if(Nodes[0].Bounds.Test(Camera::Position))
		{
			overlap[1] = true;
			overlap[3] = true;
		}

		else if(Nodes[1].Bounds.Test(Camera::Position))
			overlap[1] = true;
		
		else if(Nodes[2].Bounds.Test(Camera::Position))
		{
			overlap[1] = true;
			overlap[5] = true;
		}

		else if(Nodes[3].Bounds.Test(Camera::Position))
			overlap[3] = true;

		else if(Nodes[5].Bounds.Test(Camera::Position))
			overlap[5] = true;

		else if(Nodes[6].Bounds.Test(Camera::Position))
		{
			overlap[3] = true;
			overlap[7] = true;
		}

		else if(Nodes[7].Bounds.Test(Camera::Position))
			overlap[7] = true;

		else if(Nodes[8].Bounds.Test(Camera::Position))
		{
			overlap[5] = true;
			overlap[7] = true;
		}

		if(overlap[1])
		{
			Nodes[6].Destroy();
			Nodes[6] = Nodes[3];
			Nodes[7].Destroy();
			Nodes[7] = Nodes[4];
			Nodes[8].Destroy();
			Nodes[8] = Nodes[5];
			Nodes[3] = Nodes[0];
			Nodes[4] = Nodes[1];
			Nodes[5] = Nodes[2];
			Nodes[0].Generate(Nodes[0].Offset[0] - 1.0f, Nodes[0].Offset[1]);
			Nodes[1].Generate(Nodes[1].Offset[0] - 1.0f, Nodes[1].Offset[1]);
			Nodes[2].Generate(Nodes[2].Offset[0] - 1.0f, Nodes[2].Offset[1]);
		}

		if(overlap[3])
		{
			Nodes[2].Destroy();
			Nodes[2] = Nodes[1];
			Nodes[1] = Nodes[0];
			Nodes[0].Generate(Nodes[0].Offset[0], Nodes[0].Offset[1] - 1.0f);
			Nodes[5].Destroy();
			Nodes[5] = Nodes[4];
			Nodes[4] = Nodes[3];
			Nodes[3].Generate(Nodes[3].Offset[0], Nodes[3].Offset[1] - 1.0f);
			Nodes[8].Destroy();
			Nodes[8] = Nodes[7];
			Nodes[7] = Nodes[6];
			Nodes[6].Generate(Nodes[6].Offset[0], Nodes[6].Offset[1] - 1.0f);
		}

		if(overlap[5])
		{
			Nodes[0].Destroy();
			Nodes[0] = Nodes[1];
			Nodes[1] = Nodes[2];
			Nodes[2].Generate(Nodes[2].Offset[0], Nodes[2].Offset[1] + 1.0f);
			Nodes[3].Destroy();
			Nodes[3] = Nodes[4];
			Nodes[4] = Nodes[5];
			Nodes[5].Generate(Nodes[5].Offset[0], Nodes[5].Offset[1] + 1.0f);
			Nodes[6].Destroy();
			Nodes[6] = Nodes[7];
			Nodes[7] = Nodes[8];
			Nodes[8].Generate(Nodes[8].Offset[0], Nodes[8].Offset[1] + 1.0f);
		}

		if(overlap[7])
		{
			Nodes[0].Destroy();
			Nodes[0] = Nodes[3];
			Nodes[3] = Nodes[6];
			Nodes[6].Generate(Nodes[6].Offset[0] + 1.0f, Nodes[6].Offset[1]);
			Nodes[1].Destroy();
			Nodes[1] = Nodes[4];
			Nodes[4] = Nodes[7];
			Nodes[7].Generate(Nodes[7].Offset[0] + 1.0f, Nodes[7].Offset[1]);
			Nodes[2].Destroy();
			Nodes[2] = Nodes[5];
			Nodes[5] = Nodes[8];
			Nodes[8].Generate(Nodes[8].Offset[0] + 1.0f, Nodes[8].Offset[1]);
		}
	}

	Camera::Position.y = Noise::Generate(Camera::Position.x, Camera::Position.z) + 0.667f;
}

void Terrain::Destroy()
{
	for(size_t index = 0; index < Nodes.size(); ++index)
		Nodes[index].Destroy();
}