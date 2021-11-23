#include "./core/graphics/shader.h"
#include "./core/file_utility.h"

#include <GL/glew.h>
#include <iostream>

namespace CRT
{
	Shader::Shader(const std::string& _vertexFile, const std::string& _fragmentFile)
	{
		int success;
		char infoLog[512];

		uint32_t v = glCreateShader(GL_VERTEX_SHADER);
		std::string vtexts = ReadTextFile(_vertexFile);
		const char* vtext = vtexts.c_str();
		glShaderSource(v, 1, &vtext, 0);
		glCompileShader(v);
		glGetProgramiv(v, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(v, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		uint32_t f = glCreateShader(GL_FRAGMENT_SHADER);
		std::string ftexts = ReadTextFile(_fragmentFile);
		const char* ftext = ftexts.c_str();
		glShaderSource(f, 1, &ftext, 0);
		glCompileShader(f);

		glGetProgramiv(f, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(f, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		m_ProgramID = glCreateProgram();
		glAttachShader(m_ProgramID, v);
		glAttachShader(m_ProgramID, f);
		glBindAttribLocation(m_ProgramID, 0, "pos");
		glBindAttribLocation(m_ProgramID, 1, "tuv");
		glLinkProgram(m_ProgramID);

		glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

	//	glDeleteShader(v);
	//	glDeleteShader(f);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ProgramID);
	}

	void Shader::Bind()
	{
		glUseProgram(m_ProgramID);
	}

	void Shader::SetInputTexture(const uint32_t _parameterID, uint32_t _textureID, const char* _name)
	{
		glActiveTexture(GL_TEXTURE0 + _parameterID);
		glBindTexture(GL_TEXTURE_2D, _textureID);
		glUniform1i(glGetUniformLocation(m_ProgramID, _name), _parameterID);
	}
}