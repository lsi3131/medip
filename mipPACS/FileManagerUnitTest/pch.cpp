//
// pch.cpp
//

#include "pch.h"

#ifdef _DEBUG
#pragma comment(lib, "FileManagerAppCored.lib")
#pragma comment(lib, "FileManagerNetd.lib")
#pragma comment(lib, "FileManagerStdd.lib")
#pragma comment(lib, "gtest.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib,"qpdf.lib")
//#pragma comment(lib, "gtest_main.lib")
#else
#pragma comment(lib, "FileManagerAppCore.lib")
#pragma comment(lib, "FileManagerNet.lib")
#pragma comment(lib, "FileManagerStd.lib")
#pragma comment(lib, "gtest.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib,"qpdf.lib")
//#pragma comment(lib, "gtest_main.lib")
#endif
