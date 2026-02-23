#pragma once

#ifndef VISUALPRINT_CONFIG_H
#define VISUALPRINT_CONFIG_H

#include <qstring>

class VisualPrintConfig
{
public:
	VisualPrintConfig();

public:
	void setUrl(QString url);
	void setPort(int port);

	QString getUrl();
	int getPort();

private:
	QString m_url;
	int m_port;
};
#endif
