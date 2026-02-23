#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include "define.h"
#include "MedipType.h"

class network : public QObject
{
	// static function
public:
	static const bool GetMacAddress(std::string & mac_address);
	static const bool GetMacAddressList(QStringList & addr_list);
	static const bool GetNTPDateTime(QDateTime &DateTime, QString Server, int Socket);
	static const bool SendMysqlQuary(QString str_ip, QString str_time, QString str_log, QString str_comment);
	static const bool GetServerTime(QDateTime &DateTime);
	static const QString GetipAddress();

	static const int chkFunctionUsableCount(QString url, eMEDIP_FUNCTION_LEVEL functionLevelId);		// chkFunctionUsableCount api
	static const int chkFunctionUsableCount(QString url, eMEDIP_FUNCTION_LEVEL functionLevelId, sFuncUsableCountResult &outData);
	static const bool getNoticeList(QString strUrl, QList<sNoticeListInfo> &resultVec);						// getNoticeList api.
};
#endif