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

#include "Window.hpp"
#include <glad.h>

SDL_Window *Window::Frame = nullptr;
SDL_GLContext Window::Context;
SDL_Event Window::Event;
Sint32 Window::Width, Window::Height;

bool Window::Open()
{
	SDL_DisplayMode displayMode;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GetCurrentDisplayMode(0, &displayMode);
	Width = (displayMode.w * 3) >> 2;
	Height = (displayMode.h * 3) >> 2;

	Frame = SDL_CreateWindow("Procedural Terrain", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(Frame == nullptr)
	{
		SDL_Quit();
		return false;
	}
	
	Context = SDL_GL_CreateContext(Frame);

	if(Context == nullptr)
	{
		SDL_DestroyWindow(Frame);
		SDL_Quit();

		return false;
	}

	SDL_GL_SetSwapInterval(1);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool Window::Update()
{
	SDL_GL_SwapWindow(Frame);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while(SDL_PollEvent(&Event) == SDL_TRUE)
		if(Event.type == SDL_QUIT)
			return false;

	return true;
}

void Window::Close()
{
	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Frame);
	SDL_Quit();
}