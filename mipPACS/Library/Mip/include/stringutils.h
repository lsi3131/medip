#pragma once
#include <string>
#include <vector>

namespace mip
{
	bool dirExists(const std::string& dirpath);
	bool dirExists(const std::wstring& dirpath);

	std::vector<std::string> listFiles(const std::string& directory, const bool sort);
	std::vector<std::wstring> listFiles(const std::wstring& directory, const bool sort);

	size_t removeTrailingCharacters(std::string& str, const char trailer);
	size_t removeTrailingCharacters(std::wstring& str, const char trailer);

	std::vector<std::string> readDirectory(const std::string& directory, const bool sort, const bool recursiveSearch);
	std::vector<std::wstring> readDirectory(const std::wstring& directory, const bool sort, const bool recursiveSearch);
	
	std::string replaceAllCharacters(const std::string& name, const char oldChar, const char newChar);
	std::wstring replaceAllCharacters(const std::wstring& name, const char oldChar, const char newChar);

	std::string trim(std::string str, const std::string& charlist =  " \t\n\r\0\x0B" );
	std::wstring trim(std::wstring str, const std::wstring& charlist = L" \t\n\r\0\x0B");
};