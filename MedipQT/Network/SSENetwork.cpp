#include "stdafx.h"
#include "SSENetwork.h"

#include "../System/LicenseManager.h"
#include "../System/ApplicationManager.h"
#include "../System/stringManager.h"
#include "windowManager.h"
#include "../MedipQT.h"

#include <WinInet.h>
#pragma comment (lib, "WinInet.lib")

const QString SSENetwork::SSE_DUPLICATED_LOGIN				= "sse_duplicated_login";
const QString SSENetwork::SSE_SUBSCRIBE						= "sse_subscribe";
const QString SSENetwork::SSE_RESUBSCRIBE					= "sse_resubscribe";
const QString SSENetwork::SSE_RE_CONNECTION					= "reConnection";
const QString SSENetwork::SSE_FIRST_CONNECTION				= "firstConnection";
const int SSENetwork::SSE_SUBSCRIBE_MAX_WAITTING_TIME		= 5000;
const int SSENetwork::SSE_SUBSCRIBE_CONNECTION_TIME_OUT		= 5000;
const int SSENetwork::SSE_RECONNECTION_TIMER_CALL_INTERVAL	= 60000;

SSENetwork::SSENetwork(QObject *parent): QObject(parent)
{
    // Initiate a new QNetworkAccessManager
    this->setQNAM(new QNetworkAccessManager(this));
	QNetworkConfigurationManager QNAMConfig;

#if 0
    this->QNAM()->setConfiguration(QNAMConfig.defaultConfiguration());
#else
	QNetworkConfiguration config = QNAMConfig.defaultConfiguration();
	config.setConnectTimeout(SSE_SUBSCRIBE_CONNECTION_TIME_OUT);
	this->QNAM()->setConfiguration(config);
#endif

    // Connect QNetworkAccessManager signals
    connect(this->QNAM(),
            SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this,
            SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    connect(this->QNAM(),
            SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)),
            this,
            SIGNAL(sslErrorsReceived(QNetworkReply *, QList<QSslError>)));
    connect(this->QNAM(),
            SIGNAL(finished(QNetworkReply *)),
            this,
            SLOT(streamFinished(QNetworkReply *)));

	connect(this,
			SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
			this,
			SLOT(networkAccessibleChangedSlot(QNetworkAccessManager::NetworkAccessibility)));


    // Init retries counter
    m_retries = 0;
	m_reply = nullptr;
	m_bDuplicatedLogin = false;
	m_bForceLogOutCase = false;

	// local ip address get
	m_strLocalIp = "";
	QString localhostname = QHostInfo::localHostName();
	QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
	foreach(const QHostAddress& address, hostList)
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) {
			m_strLocalIp = address.toString();
			break;
		}
	}

	m_ReconnectionTimer = new QTimer(this);
	connect(m_ReconnectionTimer, &QTimer::timeout, this, &SSENetwork::reSubscribeTimer);

}

SSENetwork::~SSENetwork()
{
	qInfo() << "SSENetwork destructor call";

	m_ReconnectionTimer->stop();
}

void SSENetwork::subscribe(QUrl &url, QString _accessToken, QString _connection)
{
	qInfo() << "subscribe call : " << url;

	QUrlQuery query;
	query.addQueryItem("accessToken", _accessToken);
	query.addQueryItem("accessIp", m_strLocalIp);
	query.addQueryItem("connection", _connection);
	url.setQuery(query);

    QNetworkRequest request = this->prepareRequest(url);

	// 최초 로딩시 NIC 사용 안함 상태로 해놓고 로그인 창이 뜨면 NIC 사용함으로 변경하고 5초정도 후에 로그인을 시도하면 SSE fail나는 경우 생김.
	// 원인은 물리적인 라인은 붙었는데 SSE Networkmanager 객체에서 not accessible로 잘못 인지하고 있는 상황이라 아래와 같은 코드 추가하여 수정 완료.
	if (this->QNAM()->networkAccessible() != QNetworkAccessManager::Accessible)
		this->QNAM()->setNetworkAccessible(QNetworkAccessManager::Accessible);

    m_reply = this->QNAM()->get(request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(streamReceived()));
}

void SSENetwork::subscribe_test(QUrl &url, QString _userId, QString _serviceId, QString _connection)
{
	qInfo() << "subscribe test call : " << url;

	QUrlQuery query;
	query.addQueryItem("userId", _userId);
	query.addQueryItem("serviceId", _serviceId);
	query.addQueryItem("accessIp", m_strLocalIp);
	query.addQueryItem("connection", _connection);
	url.setQuery(query);

	QNetworkRequest request = this->prepareRequest(url);

	// 최초 로딩시 NIC 사용 안함 상태로 해놓고 로그인 창이 뜨면 NIC 사용함으로 변경하고 5초정도 후에 로그인을 시도하면 SSE fail나는 경우 생김.
	// 원인은 물리적인 라인은 붙었는데 SSE Networkmanager 객체에서 not accessible로 잘못 인지하고 있는 상황이라 아래와 같은 코드 추가하여 수정 완료.
	if (this->QNAM()->networkAccessible() != QNetworkAccessManager::Accessible)
		this->QNAM()->setNetworkAccessible(QNetworkAccessManager::Accessible);

	m_reply = this->QNAM()->get(request);
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(streamReceived()));
}

#if 0
void SSENetwork::unsubscribe(QUrl &url, QString _accessToken)
{
	qInfo() << "unsubscribe call";

	QUrlQuery query;
	query.addQueryItem("accessToken", _accessToken);
	url.setQuery(query.query());

	QNetworkRequest request(url);
	nAccessMng_unsubscribe.get(request);
}
#else
qint16 SSENetwork::unsubscribe(QUrl &url, QString _accessToken)
//qint16 SSENetwork::unsubscribe(QUrl &url, QString _userId, QString _serviceId)
{
	qInfo() << "unsubscribe call";

	QUrlQuery query;
	query.addQueryItem("accessToken", _accessToken);
//	query.addQueryItem("userId", _userId);
//	query.addQueryItem("serviceId", _serviceId);
	url.setQuery(query.query());
	QNetworkRequest request(url);

	QTimer timer;
	timer.setSingleShot(true);

	QEventLoop loop;
	QNetworkAccessManager nAccessMng;
	QNetworkReply *reply = nAccessMng.get(request);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	timer.start(1000);
	loop.exec();

	if (!timer.isActive())
	{
		// network waitting time timeout
		disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		reply->abort();
		qInfo() << "unsubscribe() network timeout";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network timeout");
		return eURRTNetworkTimerOut;
	}

	disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	timer.stop();

	/* Check if we need to redirect */
	QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirect.isEmpty())
	{
		qInfo() << "unsubscribe() : need to redirect";
		return eURRTNeedToRedirect;
	}

	/* There was a network error */
	QNetworkReply::NetworkError err;
	if ((err = reply->error()) != QNetworkReply::NoError)
	{
		qInfo() << "unsubscribe() errorstring : " << reply->errorString() << ", error number : " << reply->error();
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network error");
		return eURRTNetworkError;
	}

	/* Try to create a JSON document from downloaded data */
	QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

	/* JSON is invalid */
	if (document.isNull())
	{
		qInfo() << "unsubscribe(): json document is null";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "json document is null");
		return eURRTJsonInvalid;
	}

	bool bSuccess = false;
	if (document.object().value("success") != QJsonValue::Undefined)
		bSuccess = document.object().value("success").toBool();

	return bSuccess ? eURRTSuccess : -1;

}
#endif

//qint16 SSENetwork::addNotificationHistory(QUrl &url, QString _accessToken, QString _notificationContents)
qint16 SSENetwork::addNotificationHistory(QUrl &url, QString _userId, QString _serviceId, QString _notificationContents)
{
	qInfo() << "addNotificationHistory call";

	QUrlQuery query;
//	query.addQueryItem("accessToken", _accessToken);
	query.addQueryItem("userId", _userId);
	query.addQueryItem("serviceId", _serviceId);
	url.setQuery(query.query());

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QJsonObject json;
	json.insert("notificationContents", _notificationContents);
	json.insert("accessIp", m_strLocalIp);

	QTimer timer;
	timer.setSingleShot(true);

	QEventLoop loop;
	QNetworkAccessManager nAccessMng;
	QNetworkReply *reply = nAccessMng.post(request, QJsonDocument(json).toJson());
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	timer.start(1000);
	loop.exec();

	if (!timer.isActive())
	{
		// network waitting time timeout
		disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		reply->abort();
		qInfo() << "addNotificationHistory() network timeout";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network timeout");
		return eURRTNetworkTimerOut;
	}

	disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	timer.stop();

	/* Check if we need to redirect */
	QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirect.isEmpty())
	{
		qInfo() << "addNotificationHistory() : need to redirect";
		return eURRTNeedToRedirect;
	}

	/* There was a network error */
	QNetworkReply::NetworkError err;
	if ((err = reply->error()) != QNetworkReply::NoError)
	{
		qInfo() << "addNotificationHistory() errorstring : " << reply->errorString() << ", error number : " << reply->error();
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "network error");
		return eURRTNetworkError;
	}

	/* Try to create a JSON document from downloaded data */
	QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

	/* JSON is invalid */
	if (document.isNull())
	{
		qInfo() << "addNotificationHistory(): json document is null";
		//	QMessageBox::warning(nullptr, "chkFunctionUsableCount function", "json document is null");
		return eURRTJsonInvalid;
	}

	bool bSuccess = false;
	if (document.object().value("success") != QJsonValue::Undefined)
		bSuccess = document.object().value("success").toBool();

	return bSuccess ? eURRTSuccess : -1;
}

void SSENetwork::streamFinished(QNetworkReply *reply)
{
	// 아래에서 diconnect한 finish시그널이 recived 되면 다시 열리기 때문에 
	// received 되기 전에 보내진 fail난 호출로 인해 finish를 다시 탈 가능성이 존재.
	QNetworkReply::NetworkError errType = reply->error();
	qInfo() << "streamFinished call!!!, errType : " << errType;

	// retry count가 0이라는 것은 재연결 Period가 아니라는 것이고 그경우만 타이머 가동.
	// retry count가 1이상이면 재연결이 진행중인 상태라고 판단.
	// timeout 시간이 테스트 환경과 다를 경우 이상 상황이 발생할 수 있으므로 대비한 방어 코드.
	if (!m_retries)
	{
		// 최초 스트림이 피니쉬 되고(nic사용안함 or 랜포트 해제시 reply->abort호출로) 
		// 재연결 시도로 인해 커넥션 타임아웃이 지나면 또 호출될 수 있기 때문에 disconnect 처리하고
		// received 되면 다시 connect함.
		disconnect(this->QNAM(),
			SIGNAL(finished(QNetworkReply *)),
			this,
			SLOT(streamFinished(QNetworkReply *)));

		reSubscribeTimer();

		m_ReconnectionTimer->start(SSE_RECONNECTION_TIMER_CALL_INTERVAL);
	}
}

void SSENetwork::streamReceived()
{
	qInfo() << "Received event from stream";
//  qInfo() << QString(m_reply->readAll()).simplified().replace("data: ", "");
//	QString receivedMessage1 = QString(m_reply->readAll());
//	QString receivedMessage2 = receivedMessage1.simplified().replace("data: ", "");
//	qInfo() << receivedMessage2;
//  qInfo() << "-----------------------------------------------------";
//	QString streamReceivedData = QString(m_reply->readAll()).simplified().replace("data: ", "");
//	QString streamReceivedData = m_reply->readAll();
//	QString streamReceivedData2 = streamReceivedData.simplified();
//	QString streamReceivedData3 = streamReceivedData2.replace("data:", "");

	/* Try to create a JSON document from downloaded data */
	QByteArray streamReceivedData = m_reply->readAll().simplified().replace("data:", "");
	QJsonDocument document = QJsonDocument::fromJson(streamReceivedData);
	
	/* JSON is invalid */
	if (!document.isNull())
	{
		QString userId = "";
		QString serviceId = "";
		QString notificationContents = "";
		if (document.object().value("userId") != QJsonValue::Undefined)
			userId = document.object().value("userId").toString();
		if (document.object().value("serviceId") != QJsonValue::Undefined)
			serviceId = document.object().value("serviceId").toString();
		if (document.object().value("notificationContents") != QJsonValue::Undefined)
			notificationContents = document.object().value("notificationContents").toString();

		qInfo() << "message : " << notificationContents << ", userId : " << userId << ", serviceId : " << serviceId;
		
		// 중복 로그인인 경우 처리.
			// 현재 다른 PC에서 로그인한 사람의 아이디와 서비스 아이디가 같은 기존 사용자가 연결되어 있으므로 SSE로 전송됨.
			// 서버에서 전달된 중복 로그인 확인 메시지까지 같으면 중복 로그인하여 사용중이라고 판단.
		if (!notificationContents.isEmpty() && !notificationContents.compare(SSE_DUPLICATED_LOGIN, Qt::CaseInsensitive))
		{
			//대, 소문자를 구분하는 상황 발생. Case Insensitive로 변경하여 배포
			if (!userId.isEmpty() && !userId.compare(LICENSE_DATA->getAuthenticatedID(), Qt::CaseInsensitive))
			{
				if (!serviceId.isEmpty() && !serviceId.compare(LICENSE_DATA->getServiceID(), Qt::CaseInsensitive))
				{
					// 마지막 접속 IP정보 Parsing.
					QString lastLoginIp = "";
					if (document.object().value("lastLoginIp") != QJsonValue::Undefined)
						lastLoginIp = document.object().value("lastLoginIp").toString();

					m_bDuplicatedLogin = true;

					// 중복 로그인 메시지를 정상적으로 받았으면 히스토리 저장 api 호출.
					LICENSE_DATA->sseAddNotificationHistory();

					QMessageBox::warning(
						WIN_MANAGER->mainWindow, 
						STRING_MANAGER->getString(STR_WARN),
						STRING_MANAGER->getStringFormat(ERR_DE_0008, { lastLoginIp }),
						QMessageBox::Ok);

					// 로그 아웃.
					m_bForceLogOutCase = true;		// close시 팝업 제거위해 셋팅.
					APP_MNG->AppLogOut();
				}
			}
		}
		// login - chklicense - subscribe - subscribe 완료 메시지가 오면
		else if (!notificationContents.isEmpty() && !notificationContents.compare(SSE_SUBSCRIBE, Qt::CaseSensitive))
		{
			// 서버로부터 subscribe가 완료됐다는 메시지까지 받아야 최종 로그인 성공을 판단.
			// 못받으면 로그아웃 되어야한다.
			emit SSESubscibeComplete();
		}

		m_retries = 0;
		m_ReconnectionTimer->stop();

		disconnect(this->QNAM(),
			SIGNAL(finished(QNetworkReply *)),
			this,
			SLOT(streamFinished(QNetworkReply *)));

		connect(this->QNAM(),
			SIGNAL(finished(QNetworkReply *)),
			this,
			SLOT(streamFinished(QNetworkReply *)));
	}

}

QNetworkRequest SSENetwork::prepareRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("Accept"), QByteArray(ACCEPT_HEADER));
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached
    return request;
}

void SSENetwork::networkAccessibleChangedSlot(QNetworkAccessManager::NetworkAccessibility state)
{
	qInfo() << "changedState : " << state;

	// 재시도 카운트가 0이라는 것은 재 연결 시도를 하지 않고 있는 상태.
	// 재시도 카운트가 0 이상이라는 것은 이미 인터넷 끊김 등을 감지하여 재연결 Period로 진행되고 있기 때문에 커넥션을 초기화하고 재연결 시도를 다시할 필요는 없다.
	// access -> not access로 변경시에만 동작.
	if (state == QNetworkAccessManager::NotAccessible && !m_retries)
	{
		// NotAccessible 되는 경우가 포트 해제 말고 더 존재하여 
		// 인터넷이 연결이 끊긴 경우를 추가로 체크하여 포트 해제를 판단. 
		if (m_reply && !IsInternetOn())
		{
			qInfo() << "port release";
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = nullptr;
		}
	}
}

// 랜 포트 해제/NIC 사용 안함등 클라이언트 인터넷이 끊긴 경우에 1분에 한번씩 호출되는 타이머 함수.
// Connection timeout 동안 호출되어 연결이 되거나 타임아웃 종료되면 로그 아웃.
void SSENetwork::reSubscribeTimer()
{
	qInfo() << "Reconnecting..." << ", retryCount : " << m_retries;
	if (m_retries < MAX_RETRIES) {
		m_retries++;
		// 인터넷 연결된 경우만 서버로 subscribe_test를 날린다.
		if (IsInternetOn())
		{
			QUrl url(LICENSE_DATA->getUrl(eLSPTSSESubscribe_test));
			//	QUrl url("http://192.168.1.105:8080/sse/subscribe_test");
			this->subscribe_test(url, LICENSE_DATA->getAuthenticatedID(), LICENSE_DATA->getServiceID(), SSE_RE_CONNECTION);
		}
	}
	else {
		qCritical() << "Unable to reconnect, max retries reached";

		QMessageBox::warning(
			WIN_MANAGER->mainWindow,
			STRING_MANAGER->getString(STR_WARN),
			STRING_MANAGER->getString(ERR_DE_0009),
			QMessageBox::Ok);

		// 지정된 횟수만큼 재연결 시도 실패시 시스템 유효성이 깨지므로 로그 아웃된다.
		m_bForceLogOutCase = true;		// close시 팝업 제거위해 셋팅.
		APP_MNG->AppLogOut();
	}
}

bool SSENetwork::IsInternetOn()
{
	// 인터넷 연결 상태 확인 후 
	DWORD dwFlag;
	TCHAR szName[256];
	BOOL bInternet = ::InternetGetConnectedStateEx(&dwFlag, szName, 256, 0);
	qInfo() << "IsInternetOn : " << bInternet;
	return bInternet ? true : false;
}
