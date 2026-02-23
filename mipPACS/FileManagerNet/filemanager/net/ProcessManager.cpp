#include "stdafx.h"
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <iostream>
#include <qthread>
#include <qdebug>
#include <qstring>
#include "Util.h"
#include "ProcessManager.h"

using namespace std;
using namespace fm;

//===============================================================
//			Utility
//===============================================================
BOOL CALLBACK SetWindowToTopFromProcess_Utility(HWND hwnd, LPARAM lParam)
{
	DWORD       win_pid = 0;
	DWORD       pid = (DWORD)lParam;
	char        buf[512];

	GetWindowThreadProcessId(hwnd, &win_pid);
	memset(&buf, 0, sizeof(buf));
	GetWindowTextA(hwnd, buf, sizeof(buf));

	if (strcmp(buf, "TestGUIProject") == 0)
	{
		qInfo() << "found";
	}
	//qInfo() << "win pid : " << win_pid << "(name= " << buf << "), pid : " << pid;
	if (pid == 0 || win_pid == 0)
	{
		return TRUE;
	}
	if (pid == win_pid)
	{
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		BringWindowToTop(hwnd);

		memset(&buf, 0, sizeof(buf));
		GetWindowTextA(hwnd, buf, sizeof(buf));
		qInfo() << "window text : " << buf;
		//		printf("%s\n", buf);

		return TRUE;
	}
	return TRUE;
}

//===============================================================
//			ProcessManager
//===============================================================
ProcessManager::ProcessManager()
{
}


ProcessManager::~ProcessManager()
{
}

bool ProcessManager::StartProcess(std::wstring processPath, ProcessInfo* pProcessInfo)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	bool result = CreateProcessW(
		NULL,
		(WCHAR*)processPath.c_str(),
		NULL, NULL,
		FALSE, 0,
		NULL, NULL,
		&si, &pi
	) == TRUE;

	if (result)
	{
		if (pProcessInfo)
		{
			pProcessInfo->PID = pi.dwProcessId;
			pProcessInfo->PName = FilePathUtil::ExtractFileName(QString::fromStdWString(processPath)).toStdWString();
		}
	}
	return result;
}

bool fm::ProcessManager::SetWindowToTopByPID(int pid)
{
	return EnumWindows(SetWindowToTopFromProcess_Utility, (LPARAM)pid) == TRUE;
}

bool ProcessManager::GetAllProcessList(std::vector<ProcessInfo>& procesInfos)
{
	DWORD processes[1024];
	DWORD cbNeeded = 0;
	DWORD processesCount = 0;
	if (EnumProcesses(processes, sizeof(processes), &cbNeeded) == FALSE)
	{
		return false;
	}

	processesCount = cbNeeded / sizeof(DWORD);

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hProcessSnapShot == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	procesInfos.clear();
	BOOL first = Process32FirstW(hProcessSnapShot, &pe32);
	do
	{
		ProcessInfo processInfo;
		processInfo.PName = pe32.szExeFile;
		processInfo.PID = pe32.th32ProcessID;
		procesInfos.push_back(processInfo);
	} while (Process32NextW(hProcessSnapShot, &pe32));

	return true;
}

bool ProcessManager::FindByName(std::vector<ProcessInfo>& findProcessInfos, std::wstring processName)
{
	std::vector<ProcessInfo> processInfos;
	if (GetAllProcessList(processInfos) == false)
	{
		return false;
	}

	findProcessInfos.clear();
	for (auto& pinfo : processInfos)
	{
		QString lhs = QString::fromStdWString(pinfo.PName);
		QString rhs = QString::fromStdWString(processName);
		if (lhs.compare(rhs, Qt::CaseInsensitive) == 0)
		{
			findProcessInfos.push_back(pinfo);
		}
	}

	return findProcessInfos.size() > 0;
}

bool ProcessManager::FindByNameAndPID(ProcessInfo& processInfo, std::wstring processName, int pid)
{
	std::vector<ProcessInfo> findProcessInfos;
	if (FindByName(findProcessInfos, processName) == false)
	{
		return false;
	}

	for (auto& pinfo : findProcessInfos)
	{
		if (pinfo.PID == pid)
		{
			processInfo = pinfo;
			return true;
		}
	}

	return false;
}

bool fm::ProcessManager::WaitUntilWindowExists(std::wstring processName, int pid, int elapsedTime_ms)
{
	ProcessInfo processInfo;
	while (FindByNameAndPID(processInfo, processName, pid) == false)
	{
		/* Window에 Process 생성까지 대기 */
		QThread::msleep(100);
	}
	return true;
}

bool ProcessManager::IsProcessExist(std::wstring processName)
{
	std::vector<ProcessInfo> findProcessInfos;
	return FindByName(findProcessInfos, processName);
}

bool ProcessManager::KillByName(wstring processName, std::vector<ProcessInfo>* pKilledProcessList)
{
	std::vector<ProcessInfo> findProcessInfos;
	if (FindByName(findProcessInfos, processName) == false)
	{
		return false;
	}

	std::vector<ProcessInfo> killedProcessList;
	for (auto& pinfo : findProcessInfos)
	{
		HANDLE hProcess = OpenProcess(
			PROCESS_TERMINATE,
			0,
			pinfo.PID
		);
		if (hProcess == NULL)
		{
			return false;
		}
		TerminateProcess(hProcess, 9);
		CloseHandle(hProcess);
		killedProcessList.push_back(pinfo);
	}

	if (pKilledProcessList)
	{
		*pKilledProcessList = killedProcessList;
	}

	/* process가 전부 종료 될 때까지 대기*/
	while (FindByName(findProcessInfos, processName))
	{
		QThread::msleep(300);
	}

	return true;
}
