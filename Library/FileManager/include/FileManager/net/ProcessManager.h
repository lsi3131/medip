#pragma once

/*
Purpose : 
- Windows Process정보를 조회 및 Kill할 수 있는 Utility 클래스

Author: 
- 이상일
*/

#include "filemanager/net/export.h"
#include <vector>
#include <string>

namespace fm
{
	struct ProcessInfo
	{
		std::wstring PName;
		int PID;
	};

	class FM_NET_EXPORT ProcessManager
	{
	public:
		ProcessManager();
		~ProcessManager();

	public:
		static bool StartProcess(std::wstring processPath, ProcessInfo* pProcessInfo = nullptr);
		static bool SetWindowToTopByPID(int pid);
		static bool GetAllProcessList(std::vector<ProcessInfo>& procesInfos);
		static bool FindByName(std::vector<ProcessInfo>& processInfos, std::wstring processName);
		static bool FindByNameAndPID(ProcessInfo& processInfo, std::wstring processName, int pid);
		static bool WaitUntilWindowExists(std::wstring processName, int pid, int elapsedTime_ms = -1);
		static bool IsProcessExist(std::wstring processName);
		static bool KillByName(std::wstring processName, std::vector<ProcessInfo>* pKilledProcessList = nullptr);

	};
};

