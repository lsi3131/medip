#include "stdafx.h"
#include "defines.h"
#include <qmetatype>

using namespace fm;

const wchar_t fm::MedicalIPInstanceName[] = L"MedicalIPFileManager";
const QString fm::MedipcalIPLocalServerName = "MedicalIPLocalServer";

//==========================================================
//				Qt Signal/Slot을 위한 MetaType 등록
//==========================================================
struct Register
{
	Register()
	{
		qRegisterMetaType<SERVER_TO_CLIENT_INFO>("SERVER_TO_CLIENT_INFO");
		qRegisterMetaType<CLIENT_TO_SERVER_INFO>("CLIENT_TO_SERVER_INFO");
	}
};
Register __register__;

//==========================================================
//				ServerCommunicationInfo
//==========================================================
SERVER_TO_CLIENT_INFO::SERVER_TO_CLIENT_INFO()
{
	Command = EServerCommand::update;
	memset(FileExtensionFilter, 0, sizeof(FileExtensionFilter));
}

SERVER_TO_CLIENT_INFO::SERVER_TO_CLIENT_INFO(const SERVER_TO_CLIENT_INFO& rhs)
{
	this->Command = rhs.Command;
	memcpy(this->FileExtensionFilter, rhs.FileExtensionFilter, sizeof(FileExtensionFilter));
}

SERVER_TO_CLIENT_INFO& SERVER_TO_CLIENT_INFO::operator=(const SERVER_TO_CLIENT_INFO& rhs)
{
	this->Command = rhs.Command;
	memcpy(this->FileExtensionFilter, rhs.FileExtensionFilter, sizeof(FileExtensionFilter));
	return *this;
}

//==========================================================
//				ClientCommunicationInfo
//==========================================================

CLIENT_TO_SERVER_INFO::CLIENT_TO_SERVER_INFO()
{
	Command = EClientCommand::update;
	memset(FilePath, 0, sizeof(FilePath));
	IsActivated = false;
}

CLIENT_TO_SERVER_INFO::CLIENT_TO_SERVER_INFO(const CLIENT_TO_SERVER_INFO & rhs)
{
	this->Command = rhs.Command;
	memcpy(this->FilePath, rhs.FilePath, sizeof(FilePath));
	IsActivated = rhs.IsActivated;
}

CLIENT_TO_SERVER_INFO& CLIENT_TO_SERVER_INFO::operator=(const CLIENT_TO_SERVER_INFO & rhs)
{
	this->Command = rhs.Command;
	memcpy(this->FilePath, rhs.FilePath, sizeof(FilePath));
	IsActivated = rhs.IsActivated;
	return *this;
}
