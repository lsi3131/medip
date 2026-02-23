#pragma once
#include "wtypes.h"
#include <string>

namespace mip
{
	void LogDebug(LPCSTR fmt_str, ...);

	std::string Format(LPCSTR fmt_str, ...);
	std::string IsLIBVersion(void);
};