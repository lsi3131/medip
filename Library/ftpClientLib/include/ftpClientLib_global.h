#ifndef FTPCLIENTLIB_GLOBAL_H
#define FTPCLIENTLIB_GLOBAL_H

#define BUILD_STATIC_FTPCLIENTLIB

#ifndef BUILD_STATIC_FTPCLIENTLIB
#if defined(FTPCLIENTLIB_EXPORTS)
#define FTPCLIENTLIB_EXPORT __declspec(dllexport)
#else
#define FTPCLIENTLIB_EXPORT __declspec(dllimport)
#endif
#else
#define FTPCLIENTLIB_EXPORT
#endif

#endif // TESTLIB_GLOBAL_H
