#include "StdAfx.h"
#include "MinidumpHelp.h"
#include <time.h>
#include <DbgHelp.h>

#include <QUUid>

#include "System/stringManager.h"
#include "System/LicenseManager.h"
#include "Actions/ActionManager.h"
#include "ApplicationManager.h"


MinidumpHelp::MinidumpHelp(void)
{
}

MinidumpHelp::~MinidumpHelp(void)
{
}

std::string MinidumpHelp::format_arg_list(const char *fmt, va_list args)
{
	if (!fmt) return "";
	int   result = -1, length = 256;
	char *buffer = 0;
	while (result == -1)
	{
		if (buffer) delete [] buffer;
		buffer = new char [length + 1];
		memset(buffer, 0, length + 1);
		result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args);
		length *= 2;
	}
	std::string s(buffer);
	delete [] buffer;
	return s;
}

std::string MinidumpHelp::format_string(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	std::string s = format_arg_list(fmt, args);
	va_end(args);

	return s;
}

std::wstring MinidumpHelp::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::wstring MinidumpHelp::getFileVersion(const std::wstring& filePath)
{
	// 버전정보를 담을 버퍼
	char* buffer = nullptr;
	std::wstring versionInfo;

	DWORD infoSize = 0;

	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구한다.
	infoSize = GetFileVersionInfoSize(filePath.c_str(), 0);
	if (infoSize == 0) return false;

	// 버퍼할당
	buffer = new char[infoSize];
	if (buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if (GetFileVersionInfo(filePath.c_str(), 0, infoSize, buffer) != 0)
		{
			VS_FIXEDFILEINFO* pFineInfo = nullptr;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져온다.
			if (VerQueryValue(buffer, (LPCWSTR)"\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
			{
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				// 파일버전 출력
				versionInfo = std::to_wstring(majorVer) + L"." + std::to_wstring(minorVer) + L"." +
					std::to_wstring(buildNum) + L"." + std::to_wstring(revisionNum);
			}
		}
		delete[] buffer;
	}

	return versionInfo;
}

std::wstring MinidumpHelp::get_dump_filename()
{
	time_t rawtime;
	struct tm timeinfo;

	std::string date_string;
	std::wstring date_wstring;
	std::wstring version_wstring;

	std::wstring module_path;
	std::wstring dump_filepath;

	static WCHAR ModulePath[1024];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	date_string = format_string("%d-%02d-%02d_%02d-%02d-%02d", 
		timeinfo.tm_year + 1900,
		timeinfo.tm_mon + 1,
		timeinfo.tm_mday,
		timeinfo.tm_hour,
		timeinfo.tm_min,
		timeinfo.tm_sec);
	date_wstring = s2ws(date_string);

	if (::GetModuleFileNameW(0, ModulePath, sizeof(ModulePath) /sizeof(WCHAR)) == 0)
	{
		return std::wstring();
	}

	//module_path = ModulePath;
	//dump_filename.assign(module_path, 0, module_path.rfind(L"\\") + 1);
	module_path = STRING_MANAGER->m_strAppDataLocalPath.toStdWString();
	//dump_filename.assign(module_path, 0, module_path.rfind(L"\\") + 1);
	dump_filepath = module_path + L"/dump/";

	QDir dir;
	QString folderPath = QString::fromStdWString(dump_filepath);
	
	if (!dir.exists(folderPath))
	{
		dir.mkdir(folderPath);
	}

	// uuid 생성
	std::wstring uuid_wstring = QUuid::createUuid().toString().toStdWString();

	// PDB 파일 체크
	std::wstring pdb_path = ModulePath;
	std::wstring pdb_filename;

	pdb_filename.assign(pdb_path, 0, pdb_path.rfind(L"\\") + 1);

	QString file_q_name = "MEDIP.pdb";
	
	QString pdb_q_path = QString::fromStdWString(pdb_filename) + file_q_name;

		if (QFile::exists(pdb_q_path))
	{
		QString copy_path = QString::fromStdWString(dump_filepath) + QString::fromStdWString(date_wstring) + QString("_")+ file_q_name;

		::CopyFileW(pdb_q_path.toStdWString().c_str(), copy_path.toStdWString().c_str(), false);
	}

	// version 정보 획득
	module_path = ModulePath;
	version_wstring = getFileVersion(module_path);

	if (version_wstring.size() > 0)
	{
		version_wstring = L"[" + version_wstring + L"]";
		dump_filepath = dump_filepath + version_wstring + L"_";
	}

	dump_filepath = dump_filepath + date_wstring + L"_" + uuid_wstring + L".dmp";

	return dump_filepath;
}

LONG MinidumpHelp::my_top_level_filter(__in PEXCEPTION_POINTERS pExceptionPointer)
{
	MinidumpHelp mini;

	APP_MNG->SaveOpendFile();

	MINIDUMP_EXCEPTION_INFORMATION MinidumpExceptionInformation;
	std::wstring dump_filename;

	MinidumpExceptionInformation.ThreadId = ::GetCurrentThreadId();
	MinidumpExceptionInformation.ExceptionPointers = pExceptionPointer;
	MinidumpExceptionInformation.ClientPointers = FALSE;

	dump_filename = mini.get_dump_filename();
	if (dump_filename.empty() == true)
	{
		::TerminateProcess(::GetCurrentProcess(), 0);
	}

	HANDLE hDumpFile = ::CreateFileW(dump_filename.c_str(),
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&MinidumpExceptionInformation,
		NULL,
		NULL);
	::TerminateProcess(::GetCurrentProcess(), 0);

	return 0;
}

void MinidumpHelp::install_self_mini_dump()
{
	SetUnhandledExceptionFilter(my_top_level_filter);
}

