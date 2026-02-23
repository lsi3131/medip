#pragma once

#include <QtCore/qglobal.h>

#define BUILD_STATIC

#ifndef BUILD_STATIC
# if defined(UPDATERLIB_LIB)
#define UPDATERLIB_EXPORT Q_DECL_EXPORT
# else
#define UPDATERLIB_EXPORT Q_DECL_IMPORT
# endif
#else
#define UPDATERLIB_EXPORT
#endif
