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

#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <string>

GLuint Shader::vertexShader, Shader::fragmentShader, Shader::shaderProgram;

bool Shader::Load()
{
	std::ifstream vertexFile, fragmentFile;
	std::stringstream vertexStream, fragmentStream;
	std::string vertexData, fragmentData;
	GLint statusCode;

	vertexFile.open("terrain.vs");

	if(!vertexFile)
		return false;

	fragmentFile.open("terrain.fs");

	if(!fragmentFile)
	{
		vertexFile.close();
		return false;
	}

	vertexStream << vertexFile.rdbuf();
	fragmentStream << fragmentFile.rdbuf();
	vertexFile.close();
	fragmentFile.close();
	vertexData = vertexStream.str();
	fragmentData = fragmentStream.str();

	const GLchar *vertexBuffer = vertexData.c_str();
	const GLchar *fragmentBuffer = fragmentData.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexBuffer, 0);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &statusCode);

	if(statusCode == GL_FALSE)
	{
		glDeleteShader(vertexShader);
		return false;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentBuffer, 0);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &statusCode);

	if(statusCode == GL_FALSE)
	{
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		return false;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindAttribLocation(shaderProgram, 0, "in_Position");
	glBindAttribLocation(shaderProgram, 1, "in_Color");
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &statusCode);

	if(statusCode == GL_FALSE)
	{
		Unload();
		return false;
	}

	glUseProgram(shaderProgram);
	return true;
}

void Shader::Unload()
{
	glDetachShader(shaderProgram, fragmentShader);
	glDetachShader(shaderProgram, vertexShader);
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}