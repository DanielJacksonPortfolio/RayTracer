// StringTools.h

// Provides several helpful functions to manipulate strings

#pragma once

#include <string>
#include <algorithm>
#include <vector>

namespace StringTools
{
	static std::wstring StandardToWide(std::string str)
	{
		return std::wstring(str.begin(), str.end());
	}
	static std::string WideToStandard(std::wstring wstr)
	{
		return std::string(wstr.begin(), wstr.end());
	}

	static std::string GetDirectoryFromPath(const std::string& filepath)
	{
		size_t offset1 = filepath.find_last_of('\\');
		size_t offset2 = filepath.find_last_of('/');
		if (offset1 == std::string::npos && offset2 == std::string::npos) //No Directory
			return "";
		if (offset1 == std::string::npos) //Directory (\\)
			return filepath.substr(0, offset2 + 1);
		if (offset2 == std::string::npos) //Directory (/)
			return filepath.substr(0, offset1 + 1);

		return filepath.substr(0, (offset1 > offset2 ? offset1 : offset2) + 1); //Directory (Last slash)
	}

	static std::string GetFileFromPath(const std::string& filepath)
	{
		return filepath.substr(filepath.find_last_of("/\\") + 1);
	}

	static std::string RemoveExtension(const std::string& filepath)
	{
		std::string::size_type const p(filepath.find_last_of('.'));
		return filepath.substr(0, p);
	}

	static std::string GetFileExtension(const std::string& filename)
	{
		size_t offset = filename.find_last_of('.');
		if (offset == std::string::npos) //No extension
			return{ };

		return std::string(filename.substr(offset + 1)); //Extension
	}

	static void SplitString(const std::string& string, std::vector<std::string>& container, char delim = ' ')
	{
		std::size_t current, previous = 0;
		current = string.find(delim);
		while (current != std::string::npos) {
			container.push_back(string.substr(previous, current - previous));
			previous = current + 1;
			current = string.find(delim, previous);
		}
		container.push_back(string.substr(previous, current - previous));
	}

	static std::string ChangeFileExtension(std::string filePath, std::string newExtension)
	{
		int numChars = static_cast<int>(filePath.size());
		for (int i = numChars - 1; i >= 0; --i)
		{
			if (filePath[i] == '.')
			{
				return filePath.substr(0, static_cast<size_t>(i) + 1) + newExtension;
			}
		}
		return "";
	}

	static int FindPositionOfSubStr(std::string substring, std::string data)
	{
		size_t pos = data.find(substring);
		return pos == std::string::npos ? -1 : (int)pos;
	}
};
