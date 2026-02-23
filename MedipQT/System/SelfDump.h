#pragma once

#ifndef SELFDUMP_H
#define SELFDUMP_H

#include <Windows.h>
#include <dbghelp.h>

//////////////////////////////////////////////////////////////////////////
//
// #include "selfdump.h"
//
// winmain()
// {
//		...
//		// 대략 초기에...
//		CSelfDump::RegisterExceptionFilter();
//		..
// }
//
// 혹은
//
// CApp theApp;
// ...
// CSelfDump g_cSelfDump();	// <- 전역 변수로 실행
// ..
//
// ==> 만약, TestApp.exe에서 Crash가 발생하면, 
//     (파라미터를 default 값으로 위 코드와 같이 지정했을때),
//
//		* TestApp.exe Path에 Temp 폴더가 있는 경우, 미니 덤프인 TestApp_yyyymmddhhmmssmmm.dmp가 생성된다.
//		* TestApp.exe Path에 Temp 폴더가 없는 경우, 덤프를 남기지 않는다.
//		* TestApp_*.dmp의 Total 파일 크기가 1MB를 넘어가면, 가장 옛날껏을 지워서 덤프를 만들어 크기를 유지한다.
//		* TestApp.exe Path의 Temp 폴더에 CrashDmp.log 파일에 오류 사항들을 기록한다.
//		* XP/Vista인 경우, 기존의 디버거로도 연결이 된다. (drwtsn32, Problem Reports and Solutions 지원)
//
// (알림) minidump 생성이므로, !analyze -v 정도로 사용될 수 있다.
// (알림) TestApp.exe가 User계정에서 실행될 수 있으므로, Temp 폴더는 everyone 777 권한이 있는것이 좋을 것이다.
// (알림) CSelfDump 함수군은 Thread Safe 보장이 없다. 따라서, .exe 실행 초기화때 되도록 단 한번만 호출하도록 한다.
// (알림) dbghelp.dll을 사용하는데, 실행 .exe의 module path 다음 system32 path 순서로 LoadLibrary를 시도한다.
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Parameter :
//
//	CSelfDump::CSelfDump(...) / CSelfDump::RegisterExectionFilter(...)
//	의 Argument를 설명한다.
//
//	* lpszDumpPreFixName : 덤프파일의 이름을 지정한다. 지정되지 않는 경우, 실행 .exe의 파일명이 된다.
//		ex) "abc"로 지정하는 경우, abc_yyyymmddhhmmssmmm.dmp 파일명으로 강제 지정된다.
//
//	* nMiniDumpType : 덤프파일의 타입 지정. 기본값은 MiniDumpNormal이다.
//		**) 타입은 http://msdn.microsoft.com/en-us/library/ms680519(VS.85).aspx 에서 참고하시오.
//
//	* dwDumpCapacityBytes : PreFixName으로 만들어진 덤프파일들이 유지될 파일 크기. 기본값은 1048576(1MB)이다.
//		**) 덤프를 만들기 전에 크기를 확인하여, 넘어가는 경우 가장 옛날것을 차례로 지워서 크기를 일정하게 유지한다.
//
//	* lpszDumpPath : 덤프파일을 생성할 Path. 기본값은 .exe의 Path 하부의 Temp 폴더이다.
//		ex) 기본값인 경우, C:\ttt\abc.exe는 C:\ttt\temp\abc_~~~~.dmp로 저장된다)
//
//	* pfnCallback / pfnUserData : 덤프파일을 만들기 전에 호출받을 수 있는 callback을 등록. 기본값은 NULL
//		**) callback 호출받아 UI를 띄워 줄 수 있다. FALSE를 리턴하면 덤프를 남기지 않는다.
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Tool :
//
//	* CSelfDump::SetMiniDumpTypeWhenExist(...)
//		**) nMiniDumpType을 레지스트리 값으로 세팅한다. (레지스트리 값이 있는경우만 해당)
//		ex) CSelfDump::RegisterExectionFilter(..., MiniDumpScanMemory, ...);
//			CSelfDump::SetMiniDumpTypeWhenExist(TEXT("Sofware\....\Tmp", TEXT("minidumptype"));
//			인 경우, 만약 레지스트리에 값이 있다면, MiniDumpScanMemory값에서 레지스트리 값으로 변경된다.
//			만약, 해당 값이 (DWORD)-1인 경우에는, 덤프를 남기지 않도록 예외 처리되었다.
//
//////////////////////////////////////////////////////////////////////////

// dbghelp.dll::MinuDumpWriteDump(...)의 prototype
typedef BOOL (WINAPI *LPFN_MinuDumpWriteDump)	(HANDLE								hProcess, 
												 DWORD								dwPid, 
												 HANDLE								hFile, 
												 MINIDUMP_TYPE						DumpType,
												 PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam,
												 PMINIDUMP_USER_STREAM_INFORMATION	UserStreamParam,
												 PMINIDUMP_CALLBACK_INFORMATION		CallbackParam);

// Crash 발생때 호출되는 Callback의 Prototype
// FALSE를 리턴하면, 덤프를 남기지 않는다.
typedef BOOL (*pfnCbOnCrash)	(IN LPVOID lpUserData);

// Capacity에 맞춰 파일을 삭제하고 write dump 하므로, Total 용량은 최대 (1MB + 마지막 Dump File 크기) 가 될수 있다.
#define DEFAULT_DUMP_CAPACITY	1048576	// 1MB

// 덤프 파일 manage를 위해, 한꺼번에 삭제될 파일 개수
#define MAX_DEL_TRY_MANAGE		32

#define MAX_PATH_SELFDUMP		1024

class CSelfDump
{
	public:
		CSelfDump(IN LPCTSTR lpszDumpPreFixName = NULL, IN MINIDUMP_TYPE nMiniDumpType = MiniDumpNormal, IN DWORD dwDumpCapacityBytes = DEFAULT_DUMP_CAPACITY, IN LPCTSTR lpszDumpPath = NULL, IN pfnCbOnCrash pfnCallback = NULL, IN LPVOID lpUserData = NULL);
		virtual ~CSelfDump();

	public:
		static BOOL RegisterExceptionFilter		(IN LPCTSTR lpszDumpPreFixName = NULL, IN MINIDUMP_TYPE nMiniDumpType = MiniDumpNormal, IN DWORD dwDumpCapacityBytes = DEFAULT_DUMP_CAPACITY, IN LPCTSTR lpszDumpPath = NULL, IN pfnCbOnCrash pfnCallback = NULL, IN LPVOID lpUserData = NULL);
		static VOID SetMiniDumpTypeWhenExist	(IN LPCTSTR lpszRegKeyPath, IN LPCTSTR lpszValueName);

	private:
		static LONG WINAPI CbTopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
		static VOID		WriteLogFile			(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpFileFullPathName, IN PSYSTEMTIME pstTimeOccur, IN LPCTSTR lpszDumpPrefixName, IN BOOL bWriteDumpSuccess);

	// Utility
	private:
		static DWORD	GetDefaultDumpPreFixName(OUT LPTSTR lpszDefaultDumpPreFixName, IN DWORD dwCchDefaultDumpPreFixName);
		static DWORD	GetDefaultModuleTempPath(OUT LPTSTR lpszPath, IN DWORD dwCchPath);
		static HMODULE	LoadLibrary_DbgHelp		(VOID);
		static HMODULE	LoadLibraryFromSystem	(IN LPCTSTR lpFileName);
		static BOOL		ManageCapacity			(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpPreFixName, IN DWORD dwDumpCapacityBytes);
		static BOOL		GetDumpFileFullPathName	(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpPreFixName, OUT LPTSTR lpszDumpFileFullPathName, IN DWORD dwCchDumpFileFullPathName, OUT PSYSTEMTIME pstTime);
		static DWORD	GetFilesSize			(IN LPCTSTR lpszFindFileName, OUT LPTSTR lpszEarlyFileName, IN DWORD dwCchEarlyFileName);
};
#endif
