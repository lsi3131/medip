#pragma once

#ifndef MINIDUMP_HELPER_H
#define MINIDUMP_HELPER_H

#include <Windows.h>
#include <string>

class MinidumpHelp
{
private:
	std::string format_arg_list(const char *fmt, va_list args);
	std::string format_string(const char *fmt, ...);
	std::wstring s2ws(const std::string& s);
	std::wstring getFileVersion(const std::wstring& filePath);

	static LONG WINAPI my_top_level_filter(__in PEXCEPTION_POINTERS pExceptionPointer);

public:
	std::wstring get_dump_filename();
	MinidumpHelp(void);
	~MinidumpHelp(void);
	
	void install_self_mini_dump();

};
#endif