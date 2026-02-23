#pragma once
#include <dcmtk/ofstd/ofstring.h>
#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT StringUtil
	{
	public:
		static OFString WideStringToOFString(std::wstring str);
		static std::string WideToMulitiByte(std::wstring str);
		static std::wstring OFStringToWideString(OFString str);
		static std::wstring MultiByteToWide(std::string str);
	};
}
