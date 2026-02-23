#include "stdafx.h"
#include "DicomHostInfo.h"

using namespace fm;

DicomHostInfo DicomHostInfo::LocalHostInfo(
	std::wstring applicationEntity,
	int port,
	int timeout)
{
	DicomHostInfo hostInfo(
		applicationEntity,
		"localhost",
		"",
		port,
		0,
		C_MOVE,
		timeout
	);

	return hostInfo;
}

DicomHostInfo fm::DicomHostInfo::CreateQueryRetrieve(std::wstring AETitle, std::string IP, std::string transferSyntax, int port, int maxAssoc, EDcmProtocol protocol, int timeout)
{
	return DicomHostInfo(
		AETitle,
		IP,
		transferSyntax,
		port,
		maxAssoc,
		protocol,
		timeout
	);
}

DicomHostInfo fm::DicomHostInfo::CreateStore(std::wstring AETitle, std::string IP, std::string transferSyntax, int port, int maxAssoc, int timeout)
{
	return DicomHostInfo(
		AETitle,
		IP,
		transferSyntax,
		port,
		maxAssoc,
		timeout
	);
}

DicomHostInfo::DicomHostInfo() :
	m_port(0),
	m_maxAssoc(0),
	m_protocol(EDcmProtocol::C_MOVE)
{
}

DicomHostInfo::DicomHostInfo(std::wstring AETitle,
	std::string IP,
	std::string transferSyntax,
	int port,
	int maxAssoc,
	EDcmProtocol protocol,
	int timeout) :
	m_AETitle(AETitle),
	m_IP(IP),
	m_transferSyntax(transferSyntax),
	m_port(port),
	m_maxAssoc(maxAssoc),
	m_protocol(protocol),
	m_timeout(timeout)
{
}

fm::DicomHostInfo::DicomHostInfo(
	std::wstring AETitle, 
	std::string IP,
	std::string transferSyntax, 
	int port, 
	int maxAssoc, 
	int timeout) :
	m_AETitle(AETitle),
	m_IP(IP),
	m_transferSyntax(transferSyntax),
	m_port(port),
	m_maxAssoc(maxAssoc),
	m_protocol(EDcmProtocol::C_MOVE),
	m_timeout(timeout)
{
}

bool DicomHostInfo::IsEmpty()
{
	return m_AETitle.empty();
}

void fm::DicomHostInfo::Clear()
{
	m_AETitle.clear();
	m_IP.clear();
	m_transferSyntax.clear();
	m_port = 0;
	m_maxAssoc = 0;
	m_protocol = EDcmProtocol::C_MOVE;
	m_timeout = 0;
}

bool DicomHostInfo::Init(std::wstring AETitle,
	std::string IP,
	std::string transferSyntax,
	int port,
	int maxAssoc,
	EDcmProtocol protocol,
	int timeout)
{
	m_AETitle = AETitle;
	m_IP = IP;
	m_transferSyntax = transferSyntax;
	m_port = port;
	m_maxAssoc = maxAssoc;
	m_protocol = protocol;
	m_timeout = timeout;

	return true;
}

std::wstring DicomHostInfo::AETitle()
{
	return m_AETitle;
}

std::string DicomHostInfo::IP()
{
	return m_IP;
}

std::string DicomHostInfo::TransferSyntax()
{
	return m_transferSyntax;
}

int DicomHostInfo::Port()
{
	return m_port;
}

int DicomHostInfo::MaxAssoc()
{
	return m_maxAssoc;
}

EDcmProtocol DicomHostInfo::Protocol()
{
	return m_protocol;
}

int DicomHostInfo::Timeout()
{
	return m_timeout;
}

