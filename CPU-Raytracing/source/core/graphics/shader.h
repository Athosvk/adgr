#pragma once
#include <string>

namespace CRT
{
	class Shader
	{
	public:
		Shader(const std::string& _vertexFile, const std::string& _fragmentFile);
		~Shader();

		void Bind();

		void SetInputTexture(const uint32_t _parameterID, uint32_t _textureID, const char* _name);

	private:
		uint32_t m_ProgramID;

	};
}