#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:

	unsigned int ID;

	// constructor
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		// 1. read vertex/fragment code from file
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
			return;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		// vertex shader
		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkShaderCompileStatus(vertex, true);

		// fragment shader
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkShaderCompileStatus(fragment, false);

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkShaderLinkStatus(ID);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}

	void use()
	{
		glUseProgram(ID);
	}

	// uniform utils

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

protected:

	void checkShaderCompileStatus(unsigned int shader, bool isVertexShader)
	{
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::" << (isVertexShader ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	void checkShaderLinkStatus(unsigned int program)
	{
		int success;
		char infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}
	}

};

#endif