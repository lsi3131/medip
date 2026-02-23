#pragma once

#include "filemanager/export.h"
#include <string>

namespace fm
{
	class FM_CORE_EXPORT DicomListenerInfo
	{
	public:
		DicomListenerInfo();
		DicomListenerInfo(std::wstring AETitle, int port, int timeout);

		bool IsEmpty();
		bool Init(std::wstring AETitle, int port, int timeout);

		std::wstring AETitle();
		int Port();
		int Timeout();
	private:
		std::wstring m_AETitle;
		int m_port;
		int m_timeout;
	};
};
