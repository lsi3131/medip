#pragma once

#include "filemanager/std/export.h"
#include <string>

namespace fm
{
#define DECLARE_ENUM_TEXT(EnumName, TableName, Count) \
	extern FM_CORE_EXPORT const std::pair<int, std::string> TableName[Count];\
	inline FM_CORE_EXPORT std::string EnumName##_Text(int v) { return Enum_Text(v, TableName, Count); };

	inline FM_STD_EXPORT std::string Enum_Text(int v, const std::pair<int, std::string>* table, int count)
	{
		for (int i = 0; i < count; ++i)
			if (table[i].first == v)
				return table[i].second;
		return "Invalid Type";
	}
};