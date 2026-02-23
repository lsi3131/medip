#pragma once

// DLL export and import definitions
#ifdef MIPENGINE_EXPORTS
#define MIP_LIB __declspec(dllexport)
#else
//#define MIP_LIB __declspec(dllimport)
#define MIP_LIB
#endif

// Function type macros.
#define VARARGS     __cdecl					/* Functions with variable arguments */
//#define CDECL	    __cdecl					/* Standard C function */
#define STDCALL		__stdcall				/* Standard calling convention */

#define	MIP_INLINE	  __forceinline
#define MIP_NOINLINE  __declspec(noinline)	/* Force code to NOT be inline */

//#if defined WIN64 
typedef unsigned char muint8;
typedef unsigned short muint16;
typedef unsigned int muint32;
typedef unsigned long long muint64;

typedef char mint8;
typedef short mint16;
typedef int mint32;
typedef long long mint64;

typedef muint32   TEXTURE_ID;
typedef muint32   VERTEX_ID;
typedef muint32   INDEX_ID;
typedef muint32	 RESOURCE_ID;
typedef muint32	 DEFBUFFER_ID;
//#else


//#endif
#define MIP_STRING_MAX	(1024)
#define MIP_STRING_MAX2	(10240)


#define SAFE_RELEASE(comPointer) \
	if (comPointer) { (comPointer)->Release(); (comPointer)=NULL; }

#define SAFE_DELETE(a) { if( (a) != 0 ) delete (a); (a) = 0; }
#define SAFE_DELETES(a) { if( (a) != 0 ) delete [] (a); (a) = 0; }
#define SAFE_VTKDELETE(a) { if( (a) != 0 ) { (a)->Delete(); } }


#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
				{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
				}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

typedef void(*progUpdatefunc)(int value, void * data);
typedef bool(*progCheckfunc)();
typedef void(*renderHMDfunc)(muint32 nVREye, void * data);

#include <cmath>
#include <string>
#include <vector>

#include "config.h"