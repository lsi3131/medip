//
// pch.h
//

#pragma once

#include "gtest/gtest.h"

#include <QObject>
#include <QVector>
#include <QMap>
#include <QDir>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTextStream>

#include <Windows.h>

#define NAS_UNITTEST_ROOT_PATH (std::wstring(L"\\\\it_team\\IT_share\\MEDIP\\30_개발데이터\\DICOM\\test\\unittest\\FileManager\\"))
#define NAS_USER_NAME (L"silee_tester")
#define NAS_USER_PASSWORD (L"p#j6WAn.")

inline std::wstring GetNASDirPath(const std::wstring& dirpath)
{
	return NAS_UNITTEST_ROOT_PATH + dirpath + L"\\";
}

inline bool ConnectToNAS()
{
	NETRESOURCE nr = {};
	memset(&nr, 0, sizeof(nr));
	nr.dwType = RESOURCETYPE_DISK;
	wchar_t remoteName[] = L"\\\\it_team";
	nr.lpRemoteName = remoteName;
	wchar_t username[] = NAS_USER_NAME;
	wchar_t password[] = NAS_USER_PASSWORD;

	int ret = WNetUseConnectionW(NULL, &nr, password, username, 0, NULL, NULL, NULL);
	//Q_ASSERT(ret == NO_ERROR);
	return ret == NO_ERROR;
}

inline QString GetUnitTestDataDirectory(QString subDirPath)
{
	return QDir::currentPath() + "/UnitTest/" + subDirPath;
}
