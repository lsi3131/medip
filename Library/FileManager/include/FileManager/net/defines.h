#pragma once

#include <qstring>
#include "filemanager/net/export.h"

namespace fm
{
	FM_NET_EXPORT extern const wchar_t MedicalIPInstanceName[];
	FM_NET_EXPORT extern const QString MedipcalIPLocalServerName;

	enum class EServerConnectStatus 
	{
		client_process_not_running = 0,
		client_process_in_running,
		client_process_connect_finished 
	};
	FM_NET_EXPORT inline QString GetText_EServerConnectStep(EServerConnectStatus value)
	{
		switch (value)
		{
		case EServerConnectStatus::client_process_not_running:
			return "client_process_not_running";
		case EServerConnectStatus::client_process_in_running:
			return "client_process_in_running";
		case EServerConnectStatus::client_process_connect_finished:
			return "client_process_connect_finished";
		default:
			return "invalid EServerConnectStep";
		}
	}

	enum class EServerCommand : char
	{
		update = 0x00,
		open,
		last,
	};

	FM_NET_EXPORT inline QString GetText_EServerCommand(EServerCommand cmd)
	{
		switch (cmd)
		{
		case EServerCommand::update:
			return "update";
		case EServerCommand::open:
			return "open";
		default:
			return "invalid command";
		}
	}

	enum class EClientCommand : char
	{
		update = 0x00,
		file_open,
		activate,
		last,
	};
	FM_NET_EXPORT inline QString GetText_EClientCommand(EClientCommand cmd)
	{
		switch (cmd)
		{
		case EClientCommand::update:
			return "update";
		case EClientCommand::file_open:
			return "file open";
		case EClientCommand::activate:
			return "activate";
		default:
			return "invalid command";
		}
	}

	class FM_NET_EXPORT SERVER_TO_CLIENT_INFO
	{
	public:
		EServerCommand Command;
		char FileExtensionFilter[512];

		SERVER_TO_CLIENT_INFO();
		SERVER_TO_CLIENT_INFO(const SERVER_TO_CLIENT_INFO& rhs);
		SERVER_TO_CLIENT_INFO& operator=(const SERVER_TO_CLIENT_INFO& rhs);
	};

	class FM_NET_EXPORT CLIENT_TO_SERVER_INFO
	{
	public:
		EClientCommand Command;
		char FilePath[512];
		char IsActivated;

		CLIENT_TO_SERVER_INFO();
		CLIENT_TO_SERVER_INFO(const CLIENT_TO_SERVER_INFO& rhs);
		CLIENT_TO_SERVER_INFO& operator=(const CLIENT_TO_SERVER_INFO& rhs);
	};
};
