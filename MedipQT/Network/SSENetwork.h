#ifndef SSENETWORK_H
#define SSENETWORK_H

#define ACCEPT_HEADER "text/event-stream"
#define USER_AGENT "MEDIP SSE Cleint"
#define MAX_RETRIES 15

class SSENetwork : public QObject
{
    Q_OBJECT

public:
	explicit SSENetwork(QObject *parent = nullptr);
	virtual ~SSENetwork();

	QNetworkAccessManager *QNAM() const				{ return m_QNAM; }
	void setQNAM(QNetworkAccessManager *value)		{ m_QNAM = value; }

	bool IsDuplicatedLogin()						{ return m_bDuplicatedLogin; }

	bool isForceLogOutCase()						{ return m_bForceLogOutCase; }
	void setForceLogOutCase(bool _bForceLogOutCase)	{ m_bForceLogOutCase = _bForceLogOutCase; }

signals:
    QList<QSslError> sslErrorsReceived(QNetworkReply *reply, QList<QSslError> sslError);
    QNetworkAccessManager::NetworkAccessibility networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility state);

	void SSESubscibeComplete();

public slots:
	// 로그인 -> 라이센스 체크 -> SSE subscribe(로그인이 정상적으로 완료된 Client라는 것을 SSE Server에 등록하는 과정) -> 로딩 완료.
	void subscribe(QUrl &url, QString _accessToken, QString _connection);
	void subscribe_test(QUrl &url, QString _userId, QString _serviceId, QString _connection);

	// SSE Client가 종료되었다는 것을 SSE Server에 알려줌으로써 중복 로그인 검사 대상 Client에서 제외된다.
#if 0
	void unsubscribe(QUrl &url, QString _accessToken);
#else
	qint16 unsubscribe(QUrl &url, QString _accessToken);
//	qint16 unsubscribe(QUrl &url, QString _userId, QString _serviceId);
#endif

	// SSE Client가 서버로부터 Notification 메시지를 받았다는 히스토리를 기록.
//	qint16 addNotificationHistory(QUrl &url, QString _accessToken, QString _notificationContents);
	qint16 addNotificationHistory(QUrl &url, QString _userId, QString _serviceId, QString _notificationContents);

	void networkAccessibleChangedSlot(QNetworkAccessManager::NetworkAccessibility state);

	void reSubscribeTimer();

private slots:
    void streamFinished(QNetworkReply *reply);
    void streamReceived();

public:
	bool IsInternetOn();

private:
    qint16 m_retries;
    QNetworkReply *m_reply;
    QNetworkAccessManager *m_QNAM;

	QString m_strLocalIp;

	QTimer *m_ReconnectionTimer;

	bool m_bDuplicatedLogin;	// 중복 로그인 알림 메시지 received.
	bool m_bForceLogOutCase;	// 중복 로그인 발생시와 SSE 15분 타임아웃 동안 서버에 못붙은 경우 강제 로그아웃 케이스.

	QNetworkRequest prepareRequest(const QUrl &url);

	// 상수 정의
public:
	static const QString				SSE_DUPLICATED_LOGIN;
	static const QString				SSE_SUBSCRIBE;
	static const QString				SSE_RESUBSCRIBE;
	static const QString				SSE_RE_CONNECTION;
	static const QString				SSE_FIRST_CONNECTION;

	static const int					SSE_SUBSCRIBE_MAX_WAITTING_TIME;
	static const int					SSE_SUBSCRIBE_CONNECTION_TIME_OUT;
	static const int					SSE_RECONNECTION_TIMER_CALL_INTERVAL;

};

#endif // SSENETWORK_H
