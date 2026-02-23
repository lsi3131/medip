#pragma once

#ifndef ACTION_VISUALPRINTWORK_H
#define ACTION_VISUALPRINTWORK_H

#include <QUndoCommand>

#include "Network/FTPProtocolOutput.h"
#include "Network/Network.h"
#include "System/Config/VisualPrintConfig.h"

#include "DataContext.h"

//class ActionVisualPrintWork : public QUndoCommand
//{
//public:
//	ActionVisualPrintWork(VOLUME_DATA * volumData, mask _m, QUndoCommand *parent = 0);
//	virtual ~ActionVisualPrintWork();
//
//	void undo() override;
//	void redo() override;
//	bool mergeWith(const QUndoCommand *command) override;
//	int id() const override { return m_id; }
//	mask getMask() const { return m_mask; }
//private:
//	int						m_id;
//	mask					m_mask;
//	VOLUME_DATA *			m_volumData;
//};

class FTPUpload : public QObject, public nsFTP::CFTPClient::ITransferNotification
{
	Q_OBJECT

public:
	FILE* m_pFile;
	tstring m_strFileName;
	UINT m_current;
	UINT m_uSize;
	QString m_strLatest;	

	nsFTP::CFTPClient * m_pClient;

	DataContext*		m_pDataContext;

	enum TOriginEnum { orBegin = SEEK_SET, orEnd = SEEK_END, orCurrent = SEEK_CUR };
public:
	FTPUpload(DataContext* pDataContext, QString& strLatest);
	virtual ~FTPUpload() { Close(); }

	bool Open(const tstring& strFileName, const tstring& strMode);
	bool Close();
	bool Seek(long lOffset, TOriginEnum enOrigin);
	long Tell();
	size_t Write(const void* pBuffer, size_t itemSize, size_t itemCount);
	size_t Read(void* pBuffer, size_t itemSize, size_t itemCount);
	virtual tstring GetLocalStreamName() const override;
	virtual UINT GetLocalStreamSize() const override;
	virtual void SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream) override;
	virtual void OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes) override;
	virtual void OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend) override;

	void setProgressValue(int value, bool init = false);
	public slots:
	void threadRun();
	//	void threadRun(QString &IP, QString &ID, QString &PW);

signals:
	void progress(int);
	void finished();
};

class FTPUploadROI : public QObject, public nsFTP::CFTPClient::ITransferNotification
{
	Q_OBJECT

public:
	FILE* m_pFile;
	tstring m_strFileName;
	UINT m_current;
	UINT m_uSize;
	QString m_strLatest;

	nsFTP::CFTPClient * m_pClient;

	DataContext* m_pDataContext;

	enum TOriginEnum { orBegin = SEEK_SET, orEnd = SEEK_END, orCurrent = SEEK_CUR };
public:
	FTPUploadROI(DataContext* pDataContext, QString& strLatest);
	virtual ~FTPUploadROI() { Close(); }

	bool Open(const tstring& strFileName, const tstring& strMode);
	bool Close();
	bool Seek(long lOffset, TOriginEnum enOrigin);
	long Tell();
	size_t Write(const void* pBuffer, size_t itemSize, size_t itemCount);
	size_t Read(void* pBuffer, size_t itemSize, size_t itemCount);
	virtual tstring GetLocalStreamName() const override;
	virtual UINT GetLocalStreamSize() const override;
	virtual void SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream) override;
	virtual void OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes) override;
	virtual void OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend) override;

	void setProgressValue(int value, bool init = false);
	public slots:
	void threadRun();
	//	void threadRun(QString &IP, QString &ID, QString &PW);

signals:
	void progress(int);
	void finished();
};

class WebLink : public QObject//, public nsFTP::CFTPClient::ITransferNotification
{
	Q_OBJECT

public:
	FILE* m_pFile;
	tstring m_strFileName;
	UINT m_current;
	UINT m_uSize;
	QString m_strLatest;
	VisualPrintConfig* m_pVisualPrintConfig;

	DataContext* m_pDataContext;

	//	nsFTP::CFTPClient * m_pClient;

	enum TOriginEnum { orBegin = SEEK_SET, orEnd = SEEK_END, orCurrent = SEEK_CUR };
public:
	WebLink(DataContext* pDataContext, VisualPrintConfig* pConfig);
	virtual ~WebLink() { Close(); }

	bool Open(const tstring& strFileName, const tstring& strMode);
	bool Close();
	bool Seek(long lOffset, TOriginEnum enOrigin);
	long Tell();
	size_t Write(const void* pBuffer, size_t itemSize, size_t itemCount);
	size_t Read(void* pBuffer, size_t itemSize, size_t itemCount);
	//	virtual tstring GetLocalStreamName() const override;
	/*virtual UINT GetLocalStreamSize() const override;
	virtual void SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream) override;
	virtual void OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes) override;
	virtual void OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend) override;*/

	void setProgressValue(int value, bool init = false);
	public slots:
	void threadRun();
	//	void threadRun(QString &IP, QString &ID, QString &PW);

signals:
	void progress(int);
	void finished();
};

class VisualPrintDlgWorker : public QObject//, public nsFTP::CFTPClient::ITransferNotification
{
	Q_OBJECT

public:
	FILE* m_pFile;
	tstring m_strFileName;
	UINT m_current;
	UINT m_uSize;
	QString m_strLatest;

	DataContext* m_pDataContext;

	//	nsFTP::CFTPClient * m_pClient;

	enum TOriginEnum { orBegin = SEEK_SET, orEnd = SEEK_END, orCurrent = SEEK_CUR };
public:
	VisualPrintDlgWorker(DataContext* pDataContext);
	virtual ~VisualPrintDlgWorker() { Close(); }

	bool Open(const tstring& strFileName, const tstring& strMode);
	bool Close();
	bool Seek(long lOffset, TOriginEnum enOrigin);
	long Tell();
	size_t Write(const void* pBuffer, size_t itemSize, size_t itemCount);
	size_t Read(void* pBuffer, size_t itemSize, size_t itemCount);
	//	virtual tstring GetLocalStreamName() const override;
	/*virtual UINT GetLocalStreamSize() const override;
	virtual void SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream) override;
	virtual void OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes) override;
	virtual void OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend) override;*/

	void setProgressValue(int value, bool init = false);
	public slots:
	void threadRun();
	//	void threadRun(QString &IP, QString &ID, QString &PW);

signals:
	void progress(int);
	void finished();
};
#endif
