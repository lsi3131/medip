#include "stdafx.h"
#include "VisualPrintConfig.h"

VisualPrintConfig::VisualPrintConfig()
{
	/* Default °ª */
	m_url = QString("medicalip.synology.me");
	m_port = 2999;
}

void VisualPrintConfig::setUrl(QString url)
{
	m_url = url;
}

void VisualPrintConfig::setPort(int port)
{
	m_port = port;
}

QString VisualPrintConfig::getUrl()
{
	return m_url;
}

int VisualPrintConfig::getPort()
{
	return m_port;
}

