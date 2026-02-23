#pragma once

#ifndef ACTION_FTPWORK_H
#define ACTION_FTPWORK_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>

#include "Network/FTPProtocolOutput.h"
#include "Network/Network.h"

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

class ActionFTPWork : public QUndoCommand
{
public:
	ActionFTPWork(VOLUME_DATA* volumData, mask _m, QUndoCommand* parent = 0);
	virtual ~ActionFTPWork();

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand* command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	mask					m_mask;
	VOLUME_DATA* m_pVolumData;
};

class WorkFTPUpload : public QObject, public nsFTP::CFTPClient::ITransferNotification
{
	Q_OBJECT

public:
	enum TOriginEnum { orBegin = SEEK_SET, orEnd = SEEK_END, orCurrent = SEEK_CUR };

public:
	WorkFTPUpload(VOLUME_DATA* pVolumeData, QString& strLatest);
	virtual ~WorkFTPUpload() { Close(); }

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

signals:
	void progress(int);
	void finished();

private:
	FILE* m_pFile;
	tstring m_strFileName;
	UINT m_current;
	UINT m_uSize;
	QString m_strLatest;

	nsFTP::CFTPClient* m_pClient;

	VOLUME_DATA* m_pVolumeData;
};
#endif
