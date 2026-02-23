#pragma once

#include <string>
#include "Omniverse/OmniverseHeader.h"

class mipUsdUtil
{
public:
	static bool CopyProperties(pxr::UsdPrim* pDst, const pxr::UsdPrim& src);
	static bool IsCompatiblePrimName(const std::wstring& wname);

private:
	static bool hasNoneAsciiChar(const std::wstring& wstr);
	static bool hasSpecialChar(const std::wstring& wstr);
	static bool isNotCompatibleSpecialChar(wchar_t c);
	static bool isStartWithNumber(const std::wstring& wstr);

};
