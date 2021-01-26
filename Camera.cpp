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
#include "Shader.hpp"
#include "Window.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const float moveSpeed = 10.0f;

glm::mat4 Camera::World;
glm::vec3 Camera::Position(0.0f, 1.0f, 0.0f), Camera::Direction(0.0f, 0.0f, -1.0f), Camera::Right(1.0f, 0.0f, 0.0f), Camera::Up(0.0f, 1.0f, 0.0f);
uint32_t Camera::renderMode = 0, Camera::modePause = 500;
int32_t Camera::mouseX[2], Camera::mouseY[2];
float Camera::Yaw = glm::radians(180.0f), Camera::Pitch = 0.0f, Camera::Delta = 0.0f, Camera::Time = 0.0f;

void Camera::Update()
{
	static bool locateCursor = true;

	if(locateCursor)
	{
		SDL_GetMouseState(&mouseX[1], &mouseY[1]);
		locateCursor = false;
	}

	glm::mat4 Projection, View;
	const Uint8 *keyState = SDL_GetKeyboardState(nullptr);

	Delta = static_cast<float>(SDL_GetTicks()) - Time;
	Time = static_cast<float>(SDL_GetTicks());
	mouseX[0] = mouseX[1];
	mouseY[0] = mouseY[1];

	if(SDL_GetMouseState(&mouseX[1], &mouseY[1]) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		Yaw -= static_cast<float>(mouseX[1] - mouseX[0]) * Delta * 0.00025f;
		Pitch -= static_cast<float>(mouseY[1] - mouseY[0]) * Delta * 0.00025f;
	}

	Direction = glm::vec3(glm::sin(Yaw) * glm::cos(Pitch), glm::sin(Pitch), glm::cos(Yaw) * glm::cos(Pitch));
	Right = glm::vec3(-glm::cos(Yaw), 0.0f, glm::sin(Yaw));
	Up = glm::cross(Right, Direction);

	glm::normalize(Direction);
	glm::normalize(Right);
	glm::normalize(Up);

	if(keyState[SDL_SCANCODE_W] != 0 || keyState[SDL_SCANCODE_UP] != 0)
		Position += Direction * Delta * 0.001f * moveSpeed;

	if(keyState[SDL_SCANCODE_A] != 0 || keyState[SDL_SCANCODE_LEFT] != 0)
		Position -= Right * Delta * 0.001f * moveSpeed;

	if(keyState[SDL_SCANCODE_S] != 0 || keyState[SDL_SCANCODE_DOWN] != 0)
		Position -= Direction * Delta * 0.001f * moveSpeed;

	if(keyState[SDL_SCANCODE_D] != 0 || keyState[SDL_SCANCODE_RIGHT] != 0)
		Position += Right * Delta * 0.001f * moveSpeed;

	if(renderMode == 0 && keyState[SDL_SCANCODE_SPACE] != 0 && modePause <= SDL_GetTicks())
	{
		renderMode = 1;
		modePause = SDL_GetTicks() + 500;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	else if(renderMode == 1 && keyState[SDL_SCANCODE_SPACE] != 0 && modePause <= SDL_GetTicks())
	{
		renderMode = 2;
		modePause = SDL_GetTicks() + 500;
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	else if(renderMode == 2 && keyState[SDL_SCANCODE_SPACE] != 0 && modePause <= SDL_GetTicks())
	{
		renderMode = 0;
		modePause = SDL_GetTicks() + 500;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	Projection = glm::perspective(glm::radians(60.0f), static_cast<float>(Window::Width) / static_cast<float>(Window::Height), 0.01f, 32.0f);
	View = glm::lookAt(Position, Position + Direction, Up);
	World = Projection * View;

	glUniformMatrix4fv(glGetUniformLocation(Shader::shaderProgram, "mvpmatrix"), 1, GL_FALSE, &World[0][0]);
}