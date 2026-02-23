#include "stdafx.h"
#include "StringUtil.h"
#include <Windows.h>

#define MAX_BUFFER_SIZE 262144
namespace fm
{
	OFString StringUtil::WideStringToOFString(std::wstring wstr)
	{
		return OFString(WideToMulitiByte(wstr).c_str());
	}

	std::string StringUtil::WideToMulitiByte(std::wstring wstr)
	{
		int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		char text[MAX_BUFFER_SIZE] = "";
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, text, len, NULL, NULL);

		std::string result(text);
		return result;
	}

	std::wstring StringUtil::OFStringToWideString(OFString str)
	{
		return MultiByteToWide(str.c_str());
	}

	std::wstring StringUtil::MultiByteToWide(std::string str)
	{
		int len = MultiByteToWideChar(CP_ACP, NULL, str.c_str(), str.length(), NULL, NULL);
		wchar_t wtext[MAX_BUFFER_SIZE] = L"";
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), wtext, len);

		std::wstring result = std::wstring(wtext);
		return result;
	}

}
