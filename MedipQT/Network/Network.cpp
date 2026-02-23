#include "stdafx.h"
#include "NetWork.h"

#include <IPHlpApi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#include "QtNetwork\qhostinfo.h"
#include "QtNetwork\qudpsocket.h"

#include "LicenseManager.h"


	const bool network::GetMacAddress(std::string & mac_address)
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		UINT i;

		struct tm newtime;
		char buffer[50];
		errno_t error;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			//printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return false;
		}

		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL)
			{
				//printf("Error allocating memory needed to call GetAdaptersinfo\n");
				return false;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
		{
			pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				if (0)
				{
					printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
					printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
					printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
					printf("\tAdapter Addr: \t");

					for (i = 0; i < pAdapter->AddressLength; i++)
					{
						if (i == (pAdapter->AddressLength - 1))
							printf("%.2X\n", (int)pAdapter->Address[i]);
						else
							printf("%.2X-", (int)pAdapter->Address[i]);
					}
				}

				if (pAdapter->Type == MIB_IF_TYPE_ETHERNET)
				{
					snprintf(buffer, 50, "%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X",
						pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
					mac_address = buffer;
					break;
				}

				if (0)
				{
					printf("\tIndex: \t%d\n", pAdapter->Index);
					printf("\tType: \t");
					switch (pAdapter->Type)
					{
					case MIB_IF_TYPE_OTHER:
						printf("Other\n");
						break;
					case MIB_IF_TYPE_ETHERNET:
						printf("Ethernet\n");
						break;
					case MIB_IF_TYPE_TOKENRING:
						printf("Token Ring\n");
						break;
					case MIB_IF_TYPE_FDDI:
						printf("FDDI\n");
						break;
					case MIB_IF_TYPE_PPP:
						printf("PPP\n");
						break;
					case MIB_IF_TYPE_LOOPBACK:
						printf("Lookback\n");
						break;
					case MIB_IF_TYPE_SLIP:
						printf("Slip\n");
						break;
					default:
						printf("Unknown type %ld\n", pAdapter->Type);
						break;
					}
				}

				if (0)
				{
					printf("\tIP Address: \t%s\n",
						pAdapter->IpAddressList.IpAddress.String);
					printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

					printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
					printf("\t***\n");

					if (pAdapter->DhcpEnabled)
					{
						printf("\tDHCP Enabled: Yes\n");
						printf("\t  DHCP Server: \t%s\n",
							pAdapter->DhcpServer.IpAddress.String);

						printf("\t  Lease Obtained: ");
						/* Display local time */
						error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
						if (error)
							printf("Invalid Argument to _localtime32_s\n");
						else {
							// Convert to an ASCII representation 
							error = asctime_s(buffer, 32, &newtime);
							if (error)
								printf("Invalid Argument to asctime_s\n");
							else
								/* asctime_s returns the string terminated by \n\0 */
								printf("%s", buffer);
						}

						printf("\t  Lease Expires:  ");
						error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
						if (error)
							printf("Invalid Argument to _localtime32_s\n");
						else {
							// Convert to an ASCII representation 
							error = asctime_s(buffer, 32, &newtime);
							if (error)
								printf("Invalid Argument to asctime_s\n");
							else
								/* asctime_s returns the string terminated by \n\0 */
								printf("%s", buffer);
						}
					}
					else
						printf("\tDHCP Enabled: No\n");

					if (pAdapter->HaveWins)
					{
						printf("\tHave Wins: Yes\n");
						printf("\t  Primary Wins Server:    %s\n",
							pAdapter->PrimaryWinsServer.IpAddress.String);
						printf("\t  Secondary Wins Server:  %s\n",
							pAdapter->SecondaryWinsServer.IpAddress.String);
					}
					else
						printf("\tHave Wins: No\n");

				}
				pAdapter = pAdapter->Next;
			}
		}
		else
		{
			printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
			return false;
		}

		if (pAdapterInfo)
			FREE(pAdapterInfo);

		return mac_address.size() > 0 ? true : false;
	}

	const bool network::GetMacAddressList(QStringList & addr_list)
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		UINT i;
		int index = 0;
		struct tm newtime;
		char buffer[50];
		errno_t error;
		std::string mac_addr;
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			//printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return false;
		}

		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL)
			{
				//printf("Error allocating memory needed to call GetAdaptersinfo\n");
				return false;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
		{
			pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				if (0)
				{
					printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
					printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
					printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
					printf("\tAdapter Addr: \t");

					for (i = 0; i < pAdapter->AddressLength; i++)
					{
						if (i == (pAdapter->AddressLength - 1))
							printf("%.2X\n", (int)pAdapter->Address[i]);
						else
							printf("%.2X-", (int)pAdapter->Address[i]);
					}
				}

				if (pAdapter->Type == MIB_IF_TYPE_ETHERNET || pAdapter->Type == IF_TYPE_IEEE80211)
				{
					snprintf(buffer, 50, "%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X",
						pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
					mac_addr = buffer;
					addr_list << QString(mac_addr.c_str());
					
				//	break;
				}

				if (0)
				{
					printf("\tIndex: \t%d\n", pAdapter->Index);
					printf("\tType: \t");
					switch (pAdapter->Type)
					{
					case MIB_IF_TYPE_OTHER:
						printf("Other\n");
						break;
					case MIB_IF_TYPE_ETHERNET:
						printf("Ethernet\n");
						break;
					case MIB_IF_TYPE_TOKENRING:
						printf("Token Ring\n");
						break;
					case MIB_IF_TYPE_FDDI:
						printf("FDDI\n");
						break;
					case MIB_IF_TYPE_PPP:
						printf("PPP\n");
						break;
					case MIB_IF_TYPE_LOOPBACK:
						printf("Lookback\n");
						break;
					case MIB_IF_TYPE_SLIP:
						printf("Slip\n");
						break;
					default:
						printf("Unknown type %ld\n", pAdapter->Type);
						break;
					}
				}

				if (0)
				{
					printf("\tIP Address: \t%s\n",
						pAdapter->IpAddressList.IpAddress.String);
					printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

					printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
					printf("\t***\n");

					if (pAdapter->DhcpEnabled)
					{
						printf("\tDHCP Enabled: Yes\n");
						printf("\t  DHCP Server: \t%s\n",
							pAdapter->DhcpServer.IpAddress.String);

						printf("\t  Lease Obtained: ");
						/* Display local time */
						error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
						if (error)
							printf("Invalid Argument to _localtime32_s\n");
						else {
							// Convert to an ASCII representation 
							error = asctime_s(buffer, 32, &newtime);
							if (error)
								printf("Invalid Argument to asctime_s\n");
							else
								/* asctime_s returns the string terminated by \n\0 */
								printf("%s", buffer);
						}

						printf("\t  Lease Expires:  ");
						error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
						if (error)
							printf("Invalid Argument to _localtime32_s\n");
						else {
							// Convert to an ASCII representation 
							error = asctime_s(buffer, 32, &newtime);
							if (error)
								printf("Invalid Argument to asctime_s\n");
							else
								/* asctime_s returns the string terminated by \n\0 */
								printf("%s", buffer);
						}
					}
					else
						printf("\tDHCP Enabled: No\n");

					if (pAdapter->HaveWins)
					{
						printf("\tHave Wins: Yes\n");
						printf("\t  Primary Wins Server:    %s\n",
							pAdapter->PrimaryWinsServer.IpAddress.String);
						printf("\t  Secondary Wins Server:  %s\n",
							pAdapter->SecondaryWinsServer.IpAddress.String);
					}
					else
						printf("\tHave Wins: No\n");

				}
				pAdapter = pAdapter->Next;
			}
		}
		else
		{
			printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
			return false;
		}

		if (pAdapterInfo)
			FREE(pAdapterInfo);

		return addr_list.size() > 0 ? true : false;
	}

	const bool network::GetNTPDateTime(QDateTime &DateTime, QString Server, int Socket) {
		bool Result = false;
		QHostInfo HostInfo = QHostInfo::fromName(Server);
		if (HostInfo.addresses().length() > 0) {
			QUdpSocket *UdpSocket = new QUdpSocket(); {
				UdpSocket->connectToHost(QHostAddress(HostInfo.addresses().at(0)), Socket);
				char Message[48] = { 010, 0, 0, 0, 0, 0, 0, 0, 0 };
				if (UdpSocket->writeDatagram(Message, sizeof(Message), QHostAddress(HostInfo.addresses().at(0)), Socket) == sizeof(Message)) {
					if (UdpSocket->waitForReadyRead()) {
						while (UdpSocket->hasPendingDatagrams()) {
							QByteArray QBABufferIn = UdpSocket->readAll();
							if (QBABufferIn.size() == 48) {
								int count = 40;
								unsigned long DateTimeIn = uchar(QBABufferIn.at(count)) + (uchar(QBABufferIn.at(count + 1)) << 8) + (uchar(QBABufferIn.at(count + 2)) << 16) + (uchar(QBABufferIn.at(count + 3)) << 24);
								long tmit = ntohl((time_t)DateTimeIn);
								tmit -= 2208988800U;
								DateTime = QDateTime::fromTime_t(tmit);
								Result = true;
							}
						}
					}
				}
			} {
				delete UdpSocket;
			}
		}
		return Result;



	}



	const bool network::SendMysqlQuary(QString str_ip, QString str_time, QString str_log, QString str_comment)
	{
		return true;

// 		QStringList Name = QApplication::libraryPaths();		
// 	
// 	//	QMessageBox::warning(0, QObject::tr("Error"), Name);
// 		QSqlDatabase db;
// 		db = QSqlDatabase::addDatabase("QMYSQL");
// 		db.setHostName("147.47.229.147"); // ip = QString
// 		db.setPort(3307); // port = int
// 		db.setUserName("dhpark"); // id = QString
// 		db.setPassword("medicalip809"); // pass = QString
// 
// 		db.setDatabaseName("medip_covid19");
// 
// 		qDebug() << L"드라이버목록 " << QSqlDatabase::drivers();
// 
// 		if (!db.open())
// 			 
// 		{
// 
// 			QMessageBox::warning(0, QObject::tr("Error"), db.lastError().text());
// 			//QMessageBox::warning(0, QObject::tr("Error"), QString::number(db.lastError().number()));
// 
// 			return false;
// 
// 		}
// 
// 		qDebug() << "Opened!" << endl;
// 		
// 		
// 		QSqlQuery q(db);
// 		
// 		q.prepare("insert into log_info values (:ip, :time, :log, :content)");
// 		q.bindValue(":ip", str_ip);
//  		q.bindValue(":time", str_time);
//  		q.bindValue(":log", str_log);
//  		q.bindValue(":content", str_comment);
// 
// 		
// 		if (q.exec()) {
// 
// 			qDebug() << "Inserted!" << endl;
// 			db.close();
// 			return true;
// 
// 		}
// 		else {
// 
// 			
// 			qDebug() << q.lastError().text() << endl;
// 			db.close();
// 			return false;
// 		}
		
	}


	const bool network::GetServerTime(QDateTime &DateTime)
	{

		//QUrl url = "http://147.47.229.147:9000/now";
		QUrl url = "http://medicalip.synology.me:9000/now";
		QNetworkAccessManager manager;
		QNetworkRequest request(url);
		QNetworkReply *reply(manager.get(request));
		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		
		loop.exec();

		QString time_str =  reply->readAll();		
		
		QString time_format = "yyyy-MM-dd HH:mm:ss";
			
		DateTime = QDateTime::fromString(time_str, time_format);

		if (time_str.length() < 19)
			return false;

		return true;
	}

	const QString network::GetipAddress()
	{
		QList<QHostAddress> addrlist = QNetworkInterface::allAddresses();
		QHostAddress addr;
		foreach(addr, addrlist) 
		{

			if (0 >= addr.toIPv4Address()) continue;
			else {
				if (addr.toString().contains("127.0.0.1")) continue;
				else { qDebug() << addr.toString();  break; }
			}
			
		} //for()
		
		return addr.toString();
		
	}

	const int network::chkFunctionUsableCount(QString url, eMEDIP_FUNCTION_LEVEL functionLevelId)
	{
		QString accessToken = tr("");
		accessToken = LICENSE_DATA->getLastLoginResultInfo().strAccessToken;

		QUrl _url(url);
		QUrlQuery query;
		query.addQueryItem("accessToken", accessToken);
		_url.setQuery(query.query());
		QNetworkRequest request(_url);

		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonObject json;

		QString strFunctionLevelId = QString::number(functionLevelId).rightJustified(8, '0');
		json.insert("functionLevelId", strFunctionLevelId);

		QTimer timer;
		timer.setSingleShot(true);

		QEventLoop loop;
		QNetworkAccessManager nAccessMng;
		QNetworkReply *reply = nAccessMng.post(request, QJsonDocument(json).toJson());
		connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		timer.start(MAX_SERVER_WAITTING_TIME);
		loop.exec();

		if (!timer.isActive())
		{
			// network waitting time timeout
			disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			reply->abort();
			qInfo() << "chkFunctionUsableCount() network timeout";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network timeout");
			return -1;
		}

		disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		timer.stop();

		/* Check if we need to redirect */
		QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (!redirect.isEmpty())
		{
			qInfo() << "chkFunctionUsableCount() : need to redirect";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "need to redirect");
			return -1;
		}

		/* There was a network error */
		QNetworkReply::NetworkError err;
		if ((err = reply->error()) != QNetworkReply::NoError)
		{
			qInfo() << "chkFunctionUsableCount() errorstring : " << reply->errorString() << ", error number : " << reply->error();
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network error");
			return -1;
		}

		/* Try to create a JSON document from downloaded data */
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

		/* JSON is invalid */
		if (document.isNull())
		{
			qInfo() << "chkFunctionUsableCount(): json document is null";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "json document is null");
			return -1;
		}

		bool bSuccess = false;
		int nAvailableCount = 0;
		if (document.object().value("success") != QJsonValue::Undefined)
			bSuccess = document.object().value("success").toBool();
		if (bSuccess)
		{
			QJsonObject data = document.object().value("data").toObject();
			nAvailableCount = data.value("functionUsableCount").toInt();
			if (nAvailableCount < 0)
				nAvailableCount = 0;
		}

		return nAvailableCount;

	}

	const int network::chkFunctionUsableCount(QString url, eMEDIP_FUNCTION_LEVEL functionLevelId, sFuncUsableCountResult &outData)
	{
		QString accessToken = tr("");
		accessToken = LICENSE_DATA->getLastLoginResultInfo().strAccessToken;

		QUrl _url(url);
		QUrlQuery query;
		query.addQueryItem("accessToken", accessToken);
		_url.setQuery(query.query());
		QNetworkRequest request(_url);

		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonObject json;

		QString strFunctionLevelId = QString::number(functionLevelId).rightJustified(8, '0');
		json.insert("functionLevelId", strFunctionLevelId);

		QTimer timer;
		timer.setSingleShot(true);

		QEventLoop loop;
		QNetworkAccessManager nAccessMng;
		QNetworkReply *reply = nAccessMng.post(request, QJsonDocument(json).toJson());
		connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		timer.start(MAX_SERVER_WAITTING_TIME);
		loop.exec();

		if (!timer.isActive())
		{
			// network waitting time timeout
			disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			reply->abort();
			qInfo() << "chkFunctionUsableCount() network timeout";
			//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network timeout");
			return -1;
		}

		disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		timer.stop();

		/* Check if we need to redirect */
		QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (!redirect.isEmpty())
		{
			qInfo() << "chkFunctionUsableCount() : need to redirect";
			//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "need to redirect");
			return -1;
		}

		/* There was a network error */
		QNetworkReply::NetworkError err;
		if ((err = reply->error()) != QNetworkReply::NoError)
		{
			qInfo() << "chkFunctionUsableCount() errorstring : " << reply->errorString() << ", error number : " << reply->error();
			//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network error");
			return -1;
		}

		/* Try to create a JSON document from downloaded data */
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

		/* JSON is invalid */
		if (document.isNull())
		{
			qInfo() << "chkFunctionUsableCount(): json document is null";
			//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "json document is null");
			return -1;
		}

		bool bSuccess = false;
		int nAvailableCount = 0;
		QString strUsableCountState = "";
		if (document.object().value("success") != QJsonValue::Undefined)
			bSuccess = document.object().value("success").toBool();
		if (bSuccess)
		{
			QJsonObject data = document.object().value("data").toObject();
			nAvailableCount = data.value("functionUsableCount").toInt();
			if (nAvailableCount < 0)
				nAvailableCount = 0;
			strUsableCountState = data.value("functionUsableCountState").toString();
		}
		outData.nFuncUsableCount = nAvailableCount;
		outData.strFuncUsableCountState = strUsableCountState;
		return nAvailableCount;

	}

	// get Notice List
	const bool network::getNoticeList(QString strUrl, QList<sNoticeListInfo> &resultVec)
	{
		QUrl url(strUrl);
	//	QUrlQuery query;
	//	url.setQuery(query.query());
		QNetworkRequest request(url);

		QTimer timer;
		timer.setSingleShot(true);

		QEventLoop loop;
		QNetworkAccessManager netMng;

		QNetworkReply *reply = netMng.get(request);

		connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		timer.start(MAX_SERVER_WAITTING_TIME);
		loop.exec();

		if (!timer.isActive())
		{
			// network waitting time timeout
			disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			reply->abort();
			qInfo() << "getNoticeList() network timeout : ";
			return eURRTNetworkTimerOut;
		}

		disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		timer.stop();

		/* Check if we need to redirect */
		QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (!redirect.isEmpty())
		{
			qInfo() << "getNoticeList() : redirect is not empty";
			return eURRTNeedToRedirect;
		}

		/* There was a network error */
		QNetworkReply::NetworkError err;
		if ((err = reply->error()) != QNetworkReply::NoError)
		{
			qInfo() << "getNoticeList() errorstring : " << reply->errorString() << ", error number : " << reply->error();
			return eURRTNetworkError;
		}

		/* Try to create a JSON document from downloaded data */
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

		/* JSON is invalid */
		if (document.isNull())
		{
			qInfo() << "getNoticeList(): json document is null";
			return eURRTJsonInvalid;
		}

		bool bSuccess = false;
		bool bResult = false;
		QString resultUrl = tr("");
		if (document.object().value("success") != QJsonValue::Undefined)
			bSuccess = document.object().value("success").toBool();
		if (bSuccess)
		{
			QJsonArray data = document.object().value("data").toArray();
			foreach(const QJsonValue & value, data)
			{
				QJsonObject obj = value.toObject();
				sNoticeListInfo jsonData;
				jsonData.noticeSeq = obj.value("noticeSeq").toInt();
				jsonData.noticeType = obj.value("noticeType").toInt();
				jsonData.noticeSubject = obj.value("noticeSubject").toString();
				jsonData.noticeContent = obj.value("noticeContent").toString();
				jsonData.noticeStartDate = obj.value("strDttm").toString();
				jsonData.noticeEndDate = obj.value("endDttm").toString();
				resultVec.push_back(jsonData);
				bResult = true;
			}
		}

		return bResult;
	}
