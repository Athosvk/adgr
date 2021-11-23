#pragma once
#include <string>
#include <fstream>

namespace CRT
{
	static std::string ReadTextFile(const std::string& _filepath)
	{
		//
		std::string result = "";
		std::string line;

		// Read all lines
		std::ifstream file(_filepath);
		if (file.is_open())
		{
			while (std::getline(file, line))
				result += line + "\n";

			file.close();
			file.clear();
			return result;
		}

		// Error
		printf("[ERROR] Unable to read file:%s\n", _filepath.c_str());
		return "";
	}
}