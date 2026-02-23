#include "stdafx.h"
#include "DicomListenerInfo.h"

using namespace fm;

DicomListenerInfo::DicomListenerInfo() :
	m_AETitle(L""),
	m_port(0),
	m_timeout(30)
{
}

DicomListenerInfo::DicomListenerInfo(std::wstring ae, int port, int timeout) :
	m_AETitle(ae),
	m_port(port),
	m_timeout(timeout)
{
}

bool DicomListenerInfo::IsEmpty()
{
	return m_AETitle.empty();
}

bool DicomListenerInfo::Init(std::wstring ae, int port, int timeout)
{
	m_AETitle = ae;
	m_port = port;
	m_timeout = timeout;

	return true;
}

std::wstring DicomListenerInfo::AETitle()
{
	return m_AETitle;
}

int DicomListenerInfo::Port()
{
	return m_port;
}

int DicomListenerInfo::Timeout()
{
	return m_timeout;
}

