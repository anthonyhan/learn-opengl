#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>

class Shader
{
	enum ECompileType
	{
		ECTYPE_VERTEX,
		ECTYPE_FRAGMENT,
		ECTYPE_GEOMETRY,
		ECTYPE_PROGRAM
	};
public:

	unsigned int ID;

	// constructor
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const char* geometryPath = NULL)
	{
		// 1. read vertex/fragment code from file
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

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

			if (geometryPath)
			{
				std::ifstream gShaderFile;
				gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				gShaderFile.open(geometryPath);

				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();

				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}

		}
		catch (std::ifstream::failure e)
		{
			printf("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n");;
			return;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		// vertex shader
		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, ECompileType::ECTYPE_VERTEX);

		// fragment shader
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, ECompileType::ECTYPE_FRAGMENT);

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);

		if (geometryPath)
		{
			const char* gShaderCode = geometryCode.c_str();
			unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, ECTYPE_GEOMETRY);

			glAttachShader(ID, geometry);
		}

		
		glLinkProgram(ID);
		checkCompileErrors(ID, ECompileType::ECTYPE_PROGRAM);

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
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value, size_t count = 1) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value, size_t count = 1) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value, size_t count = 1) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), count, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string &name, const glm::mat2 &mat, size_t count = 1) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat, size_t count = 1) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat, size_t count = 1) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
    }


protected:

	void checkCompileErrors(unsigned int shader, ECompileType type)
	{
		int success;
		char infoLog[1024];

		if (type != ECompileType::ECTYPE_PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				printf("ERROR::SHADER::%s::COMPILATION_FAILED\n%s\n", (type == ECompileType::ECTYPE_VERTEX ? "VERTEX" : "FRAGMENT"), infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
			}
		}
	}
};

#endif