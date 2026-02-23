#include "stdafx.h"
#include "SelfDump.h"
#include <strsafe.h>
#include <tchar.h>

// SelfDump를 위한 파라미터
static TCHAR			g_szDumpPreFixName		 [MAX_PATH]				= {0,};
static TCHAR			g_szDumpPath			 [MAX_PATH_SELFDUMP]	= {0,};
static DWORD			g_dwDumpCapacityBytes							= DEFAULT_DUMP_CAPACITY;
static MINIDUMP_TYPE	g_nMiniDumpType									= MiniDumpNormal;
static pfnCbOnCrash		g_pfnCallback									= NULL;
static LPVOID			g_pUserData										= NULL;

CSelfDump::~CSelfDump()
{

}

CSelfDump::CSelfDump(IN LPCTSTR lpszDumpPreFixName/*=NULL*/, IN MINIDUMP_TYPE nMiniDumpType/*=MiniDumpNormal*/, IN DWORD dwDumpCapacityBytes/*=DEFAULT_DUMP_CAPACITY*/, IN LPCTSTR lpszDumpPath/*=NULL*/, IN pfnCbOnCrash pfnCallback/*=NULL*/, IN LPVOID lpUserData/*=NULL*/)
{
	RegisterExceptionFilter(lpszDumpPreFixName, nMiniDumpType, dwDumpCapacityBytes, lpszDumpPath, pfnCallback, lpUserData);
}

LONG CSelfDump::CbTopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG							lRtnValue					= EXCEPTION_CONTINUE_SEARCH;
	HANDLE							hFile						= INVALID_HANDLE_VALUE;
	BOOL							bRtnValue					= FALSE;
	TCHAR							szPath[MAX_PATH_SELFDUMP]	= {0,};
	HMODULE							hDll						= NULL;
	LPFN_MinuDumpWriteDump			pfn							= NULL;
	SYSTEMTIME						stTime						= {0,};
	_MINIDUMP_EXCEPTION_INFORMATION	stExceptInfo				= {0,};

	// Dump Path가 존재하지 않는다면, Pass
	if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(g_szDumpPath))
	{
		// 로그도 남기지 않도록 한다.
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// Callback 등록 되어 있으면 우선 호출한다.
	if (NULL != g_pfnCallback)
	{
		if (FALSE == (*g_pfnCallback)(g_pUserData))
		{
			WriteLogFile(g_szDumpPath, TEXT(""), &stTime, g_szDumpPreFixName, FALSE);
			lRtnValue = EXCEPTION_CONTINUE_SEARCH;
			goto FINAL;
		}
	}

	// 만약, MiniDumpType이 -1 이라면,... 종료
	if ((MINIDUMP_TYPE)-1 == g_nMiniDumpType)
	{
		WriteLogFile(g_szDumpPath, TEXT(""), &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// DbgHelp.dll을 로드한다.
	hDll = CSelfDump::LoadLibrary_DbgHelp();
	if (NULL == hDll)
	{
		WriteLogFile(g_szDumpPath, TEXT(""), &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// DbgHelp.dll::MinuDumpWriteDump(...)를 구한다.
	pfn = (LPFN_MinuDumpWriteDump)::GetProcAddress(hDll, "MiniDumpWriteDump");
	if (NULL == pfn)
	{
		WriteLogFile(g_szDumpPath, TEXT(""), &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// Dump 파일 이름을 구한다.
	if (FALSE == CSelfDump::GetDumpFileFullPathName(g_szDumpPath, 
													g_szDumpPreFixName, 
													szPath, 
													MAX_PATH_SELFDUMP, 
													&stTime))
	{
		::GetLocalTime(&stTime);
		WriteLogFile(g_szDumpPath, TEXT(""), &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// Dump 파일을 Create 한다.
	hFile = ::CreateFile(szPath, 
						 GENERIC_WRITE, 
						 0, 
						 NULL, 
						 CREATE_ALWAYS, 
						 FILE_ATTRIBUTE_NORMAL, 
						 NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		WriteLogFile(g_szDumpPath, szPath, &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// 덤프 생성을 위한 변수 초기화
	stExceptInfo.ThreadId			= ::GetCurrentThreadId();
	stExceptInfo.ExceptionPointers	= pExceptionInfo;
	stExceptInfo.ClientPointers		= NULL;

	// Dump File 들을 manage하여 파일등을 삭제한다.
	if (FALSE == CSelfDump::ManageCapacity(g_szDumpPath, 
										   g_szDumpPreFixName, 
										   g_dwDumpCapacityBytes))
	{
		WriteLogFile(g_szDumpPath, szPath, &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
		goto FINAL;
	}

	// Dump 생성
	bRtnValue = (*pfn)(::GetCurrentProcess(),
					   ::GetCurrentProcessId(), 
					   hFile, 
					   MiniDumpNormal, 
					   &stExceptInfo, 
					   NULL, 
					   NULL);
	if (TRUE == bRtnValue)
	{
		WriteLogFile(g_szDumpPath, szPath, &stTime, g_szDumpPreFixName, TRUE);

		// EXCEPTION_EXECUTE_HANDLER 으로 하면,
		// drwtsn32.exe와 같은 디버거를 통한 덤프가 남지 않는다.
		// lRtnValue = EXCEPTION_EXECUTE_HANDLER;
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
	}
	else
	{
		WriteLogFile(g_szDumpPath, szPath, &stTime, g_szDumpPreFixName, FALSE);
		lRtnValue = EXCEPTION_CONTINUE_SEARCH;
	}

FINAL:

	if (INVALID_HANDLE_VALUE != hFile)
	{
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
		pfn   = NULL;
	}

	if (NULL != hDll)
	{
		::FreeLibrary(hDll);
		hDll = NULL;
		pfn  = NULL;
	}

	return lRtnValue;
}

VOID CSelfDump::SetMiniDumpTypeWhenExist(IN LPCTSTR lpszRegKeyPath, IN LPCTSTR lpszValueName)
{
	HKEY	hKey		= NULL;
	LONG	lRtnValue	= 0;
	DWORD	dwType		= 0;
	DWORD	dwCbSize	= 0;
	DWORD	dwValue		= 0;

	if ((NULL == lpszRegKeyPath) || (NULL == lpszValueName))
	{
		goto FINAL;
	}

	lRtnValue = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							   lpszRegKeyPath, 
							   0, 
							   KEY_READ, 
							   &hKey);
	if (ERROR_SUCCESS != lRtnValue)
	{
		goto FINAL;
	}

	dwType   = REG_DWORD;
	dwCbSize = sizeof(dwValue);
	lRtnValue = ::RegQueryValueEx(hKey, 
								  lpszValueName, 
								  NULL, 
								  &dwType, 
								  (LPBYTE)&dwValue, 
								  &dwCbSize);
	if (ERROR_SUCCESS != lRtnValue)
	{
		goto FINAL;
	}

	g_nMiniDumpType = (MINIDUMP_TYPE)dwValue;

FINAL:
	if (NULL != hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}
}

VOID CSelfDump::WriteLogFile(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpFileFullPathName, IN PSYSTEMTIME pstTimeOccur, IN LPCTSTR lpszDumpPrefixName, IN BOOL bWriteDumpSuccess)
{
	TCHAR szLogPath[MAX_PATH_SELFDUMP]	= {0,};
	TCHAR szSection[MAX_PATH]			= {0,};
	TCHAR szTmp	   [MAX_PATH]			= {0,};

	if ((NULL == lpszPath) || (NULL == lpszDumpFileFullPathName) || (NULL == pstTimeOccur) || (NULL == lpszDumpPrefixName))
	{
		return;
	}

	StringCchPrintf(szLogPath, MAX_PATH_SELFDUMP, TEXT("%s\\CrashDmp.log"), lpszPath);
	StringCchPrintf(szSection, MAX_PATH, TEXT("%.4d%.2d%.2d_%.2d%.2d%.2d%.3d"), pstTimeOccur->wYear, pstTimeOccur->wMonth, pstTimeOccur->wDay, pstTimeOccur->wHour, pstTimeOccur->wMinute, pstTimeOccur->wSecond, pstTimeOccur->wMilliseconds);
	
	::WritePrivateProfileString(szSection, TEXT("Prefix"), lpszDumpPrefixName, szLogPath);
	::WritePrivateProfileString(szSection, TEXT("DumpPath"), lpszDumpFileFullPathName, szLogPath);
	
	StringCchPrintf(szTmp, MAX_PATH, TEXT("%d"), ::GetCurrentProcessId());
	::WritePrivateProfileString(szSection, TEXT("ProcessId"), szTmp, szLogPath);

	::GetModuleFileName(NULL, szTmp, MAX_PATH);
	::WritePrivateProfileString(szSection, TEXT("CrashModulePath"), szTmp, szLogPath);

	if (TRUE == bWriteDumpSuccess)
	{
		::WritePrivateProfileString(szSection, TEXT("SuccessWriteDump"), TEXT("yes"), szLogPath);
	}
	else
	{
		::WritePrivateProfileString(szSection, TEXT("SuccessWriteDump"), TEXT("no"), szLogPath);
	}
}

BOOL CSelfDump::RegisterExceptionFilter(IN LPCTSTR lpszDumpPreFixName/*=NULL*/, IN MINIDUMP_TYPE nMiniDumpType/*=MiniDumpNormal*/, IN DWORD dwDumpCapacityBytes/*=DEFAULT_DUMP_CAPACITY*/, IN LPCTSTR lpszDumpPath/*=NULL*/, IN pfnCbOnCrash pfnCallback/*=NULL*/, IN LPVOID lpUserData/*=NULL*/)
{
	BOOL	bRtnValue	= TRUE;
	DWORD	dwRtnValue	= 0;

	// 파라미터들를 복사한다.
	if (NULL != lpszDumpPreFixName)
	{
		StringCchCopy(g_szDumpPreFixName, MAX_PATH, lpszDumpPreFixName);
	}
	else
	{
		GetDefaultDumpPreFixName(g_szDumpPreFixName, MAX_PATH);
	}

	if (DEFAULT_DUMP_CAPACITY != dwDumpCapacityBytes)
	{
		g_dwDumpCapacityBytes = dwDumpCapacityBytes;
	}

	if (NULL != lpszDumpPath)
	{
		StringCchCopy(g_szDumpPath, MAX_PATH_SELFDUMP, lpszDumpPath);
	}
	else
	{
		// NULL --> Default Path ( <.exe의 ModulePath>\Temp )
		dwRtnValue = CSelfDump::GetDefaultModuleTempPath(g_szDumpPath, MAX_PATH_SELFDUMP);
		if (ERROR_SUCCESS != dwRtnValue)
		{
			bRtnValue = FALSE;
			goto FINAL;
		}
	}

	g_nMiniDumpType = nMiniDumpType;
	g_pfnCallback	= pfnCallback;
	g_pUserData		= lpUserData;

	// Exception Filter를 등록한다.
	::SetUnhandledExceptionFilter(CSelfDump::CbTopLevelExceptionFilter);

	bRtnValue = TRUE;

FINAL:
	return bRtnValue;
}

BOOL CSelfDump::GetDumpFileFullPathName(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpPreFixName, OUT LPTSTR lpszDumpFileFullPathName, IN DWORD dwCchDumpFileFullPathName, OUT PSYSTEMTIME pstTime)
{
	BOOL bRtnValue	= TRUE;

	if ((NULL == lpszPath) || 
		(NULL == lpszDumpPreFixName) || 
		(NULL == lpszDumpFileFullPathName) ||
		(NULL == pstTime))
	{
		bRtnValue = FALSE;
		goto FINAL;
	}

	::GetLocalTime(pstTime);
	StringCchPrintf(lpszDumpFileFullPathName, 
					dwCchDumpFileFullPathName, 
					TEXT("%s\\%s_%.4d%.2d%.2d%.2d%.2d%.2d.dmp"), 
					lpszPath, 
					lpszDumpPreFixName, 
					pstTime->wYear, 
					pstTime->wMonth, 
					pstTime->wDay, 
					pstTime->wHour, 
					pstTime->wMinute, 
					pstTime->wSecond);
	bRtnValue = TRUE;
FINAL:
	return bRtnValue;
}

DWORD CSelfDump::GetFilesSize(IN LPCTSTR lpszFindFileName, OUT LPTSTR lpszEarlyFileName, IN DWORD dwCchEarlyFileName)
{
	HANDLE			hFind			= INVALID_HANDLE_VALUE;
	DWORD			dwTotalBytes	= 0;
	WIN32_FIND_DATA	FindFileData	= {0,};

	dwTotalBytes = 0;

	if ((NULL == lpszFindFileName) || (NULL == lpszEarlyFileName))
	{
		goto FINAL;
	}

	ZeroMemory(lpszEarlyFileName, sizeof(TCHAR) * dwCchEarlyFileName);

	// Enum File 한다.
	hFind = ::FindFirstFile(lpszFindFileName, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		dwTotalBytes = 0;
		goto FINAL;
	}

	if (TEXT('.') != FindFileData.cFileName[0])
	{
		dwTotalBytes += FindFileData.nFileSizeLow;
		StringCchCopy(lpszEarlyFileName, dwCchEarlyFileName, FindFileData.cFileName);
	}

	while (FindNextFile(hFind, &FindFileData) != 0)
	{
		if (TEXT('.') == FindFileData.cFileName[0])
		{
			continue;
		}

		// 파일 크기를 계속 더해 나간다.
		// (설마 nFileHighSize는 0이겠지?)
		dwTotalBytes += FindFileData.nFileSizeLow;

		if (TEXT('\0') == lpszEarlyFileName[0])
		{
			StringCchCopy(lpszEarlyFileName, dwCchEarlyFileName, FindFileData.cFileName);
			continue;
		}

		if (0 < _tcscmp(lpszEarlyFileName, FindFileData.cFileName))
		{
			StringCchCopy(lpszEarlyFileName, dwCchEarlyFileName, FindFileData.cFileName);
		}
	}

FINAL:

	if (INVALID_HANDLE_VALUE != hFind)
	{
		::FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return dwTotalBytes;
}

BOOL CSelfDump::ManageCapacity(IN LPCTSTR lpszPath, IN LPCTSTR lpszDumpPreFixName, IN DWORD dwDumpCapacityBytes)
{
	INT		i									= 0;
	BOOL	bRtnValue							= FALSE;
	DWORD	dwCbFileSize						= 0;
	TCHAR	szFileName[MAX_PATH_SELFDUMP]		= {0,};
	TCHAR	szDelFileName[MAX_PATH_SELFDUMP]	= {0,};
	TCHAR	szEarlyFileName[MAX_PATH_SELFDUMP]	= {0,};

	if ((NULL == lpszPath) || (NULL == lpszDumpPreFixName))
	{
		bRtnValue = FALSE;
		goto FINAL;
	}

	// Path를 결정한다.
 	StringCchPrintf(szFileName, 
					MAX_PATH_SELFDUMP, 
					TEXT("%s\\%s_*.dmp"), 
					lpszPath, 
					lpszDumpPreFixName);

	// 한꺼번에 32개의 덤프파일을 삭제할 수 있다.
	for (i=0; i<MAX_DEL_TRY_MANAGE; i++)
	{
		dwCbFileSize = CSelfDump::GetFilesSize(szFileName, 
											   szEarlyFileName, 
											   MAX_PATH_SELFDUMP);

		if (dwCbFileSize < g_dwDumpCapacityBytes)
		{
			break;
		}

		// 삭제한다.
		StringCchPrintf(szDelFileName, MAX_PATH_SELFDUMP, TEXT("%s\\%s"), g_szDumpPath, szEarlyFileName);
		::DeleteFile(szDelFileName);
	}

	bRtnValue = TRUE;
	
FINAL:

	return bRtnValue;
}

HMODULE CSelfDump::LoadLibrary_DbgHelp(VOID)
{
	HMODULE	hDLL							= NULL;
	TCHAR	szDbgHelp[MAX_PATH_SELFDUMP]	= {0,};
	LPTSTR	lpszFound						= NULL;

	if (0 != ::GetModuleFileName(NULL, szDbgHelp, MAX_PATH_SELFDUMP))
	{
		// .exe의 Path에 DbgHelp.dll이 있다면, 이것을 LoadLibrary 한다.
		lpszFound = _tcsrchr(szDbgHelp, TEXT('\\'));
		if ((NULL != lpszFound) && (szDbgHelp < lpszFound))
		{
			*lpszFound = TEXT('\0');
			StringCchCat(szDbgHelp, MAX_PATH_SELFDUMP, TEXT("\\DbgHelp.dll"));

			hDLL = ::LoadLibrary(szDbgHelp);
			if (NULL != hDLL)
			{
				// success!
				goto FINAL;
			}
		}
	}

	// System32 폴더로 부터 dbghelp.dll을 LoadLibrary 한다.
	hDLL = CSelfDump::LoadLibraryFromSystem(TEXT("DbgHelp.dll"));

FINAL:
	return hDLL;
}

HMODULE CSelfDump::LoadLibraryFromSystem(IN LPCTSTR lpFileName)
{
	if( NULL == lpFileName ) {
		return NULL;
	}

	if( 0 == lpFileName[0] ) {
		return NULL;
	}

	const int BUF_SIZ = 4096;

	LPTSTR lpszSysDir = NULL;
	UINT nNeedSize = GetSystemDirectory( NULL, 0 );
	lpszSysDir = (LPTSTR)malloc( (nNeedSize + 1) * sizeof(TCHAR) );
	if( NULL == lpszSysDir ) {
		return NULL;
	}

	if( 0 == GetSystemDirectory( lpszSysDir, nNeedSize + 1 ) ) {
		free( lpszSysDir );
		lpszSysDir = NULL;
		return NULL;
	}

	TCHAR szFullPath[BUF_SIZ] = {0,};
	_tcsncpy( szFullPath, lpszSysDir, BUF_SIZ );
	_tcsncat( szFullPath, _T("\\"), BUF_SIZ );
	_tcsncat( szFullPath, lpFileName, BUF_SIZ );

	HMODULE hModule = LoadLibraryEx( szFullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

	free( lpszSysDir );
	lpszSysDir = NULL;

	if( hModule ) {
		return hModule;
	}
	
	return NULL;
}

DWORD CSelfDump::GetDefaultModuleTempPath(OUT LPTSTR lpszPath, IN DWORD dwCchPath)
{
	DWORD	dwRtnValue	= ERROR_SUCCESS;
	LPTSTR	lpszFound	= NULL;

	if (NULL == lpszPath)
	{
		dwRtnValue = ERROR_INVALID_PARAMETER;
		goto FINAL;
	}

	// Clean Up
	ZeroMemory(lpszPath, sizeof(TCHAR) * dwCchPath);

	if (0 == ::GetModuleFileName(NULL, lpszPath, dwCchPath))
	{
		// Module Handle 구하는데 실패
		dwRtnValue = ::GetLastError();
		goto FINAL;
	}

	lpszFound = _tcsrchr(lpszPath, TEXT('\\'));
	if (NULL == lpszFound)
	{
		// Path 끝에 \가 없음?
		dwRtnValue = ERROR_PATH_NOT_FOUND;
		goto FINAL;
	}

	if (lpszFound <= lpszPath)
	{
		// Path 끝에 \가 없음?
		dwRtnValue = ERROR_PATH_NOT_FOUND;
		goto FINAL;
	}

	// Null 문자로 만들어줌
	*lpszFound = TEXT('\0');

	// Temp 폴더를 추가해줌
	StringCchCat(lpszPath, dwCchPath, TEXT("\\dump"));

	// 여기까지 왔다면, 성공
	dwRtnValue = ERROR_SUCCESS;

FINAL:
	return dwRtnValue;
}

DWORD CSelfDump::GetDefaultDumpPreFixName(OUT LPTSTR lpszDefaultDumpPreFixName, IN DWORD dwCchDefaultDumpPreFixName)
{
	DWORD	dwRtnValue						= ERROR_SUCCESS;
	TCHAR	szModulePath[MAX_PATH_SELFDUMP]	= {0,};
	LPTSTR	lpszFound						= NULL;
	LPTSTR	lpszIter						= NULL;
	BOOL	bFound							= FALSE;
	INT		i								= 0;

	// 실행 .exe의 파일이름 (확장자 제외)를 dumpfile의 prefix로 결정한다.
	if (NULL == lpszDefaultDumpPreFixName)
	{
		dwRtnValue = ERROR_INVALID_PARAMETER;
		goto FINAL;
	}

	ZeroMemory(lpszDefaultDumpPreFixName, sizeof(TCHAR)*dwCchDefaultDumpPreFixName);

	if (0 == ::GetModuleFileName(NULL, szModulePath, MAX_PATH_SELFDUMP))
	{
		dwRtnValue = ::GetLastError();
		StringCchPrintf(lpszDefaultDumpPreFixName, dwCchDefaultDumpPreFixName, TEXT("UnKnown"));
		goto FINAL;
	}

	lpszFound = _tcsrchr(szModulePath, TEXT('\\'));
	if ((NULL == lpszFound) || (lpszFound <= szModulePath) || (TEXT('\0') == *(lpszFound+1)))
	{
		dwRtnValue = ERROR_PATH_NOT_FOUND;
		StringCchPrintf(lpszDefaultDumpPreFixName, dwCchDefaultDumpPreFixName, TEXT("UnKnown"));
		goto FINAL;
	}

	// 확장자를 절단한다.
	bFound = FALSE;
	for (i=0; i<MAX_PATH; i++)
	{
		if (TEXT('\0') == *(lpszFound+i))
		{
			break;
		}

		if (TEXT('.') == *(lpszFound+i))
		{
			bFound = TRUE;
			*(lpszFound+i) = TEXT('\0');
			break;
		}
	}

	if (FALSE == bFound)
	{
		dwRtnValue = ERROR_PATH_NOT_FOUND;
		StringCchPrintf(lpszDefaultDumpPreFixName, dwCchDefaultDumpPreFixName, TEXT("UnKnown"));
		goto FINAL;
	}

	dwRtnValue = ERROR_SUCCESS;
	StringCchCopy(lpszDefaultDumpPreFixName, dwCchDefaultDumpPreFixName, lpszFound + 1);

FINAL:
	return dwRtnValue;
}