#include "stdafx.h"
#include "ActionFtpWork.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "qfile.h"
#include "DataContext.h"

ActionFTPWork::ActionFTPWork(VOLUME_DATA* volumData, mask _m, QUndoCommand* parent)
	: QUndoCommand(parent),
	m_pVolumData(volumData)
{
	static int s_id = ACT_ID_IMAGE_DILATION;
}

ActionFTPWork::~ActionFTPWork()
{

}

bool ActionFTPWork::mergeWith(const QUndoCommand* command)
{
	return false;
}

void ActionFTPWork::undo()
{

}

void ActionFTPWork::redo()
{

}

bool WorkFTPUpload::Open(const tstring& strFileName, const tstring& strMode)
{
	m_strFileName = strFileName;
#if _MSC_VER >= 1500
	bool r = fopen_s(&m_pFile, nsHelper::CCnv::ConvertToString(strFileName).c_str(),
		nsHelper::CCnv::ConvertToString(strMode).c_str()) == 0;

	if (r)
	{
		const long lCurPos = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_END);
		const long lEndPos = ftell(m_pFile);
		fseek(m_pFile, lEndPos, SEEK_SET);

		m_uSize = lEndPos;
	}

	return r;
#else
	m_pFile = fopen(CCnv::ConvertToString(strFileName).c_str(),
		CCnv::ConvertToString(strMode).c_str());
	return m_pFile != NULL;
#endif
}

bool WorkFTPUpload::Close()
{
	FILE* pFile = m_pFile;
	m_pFile = NULL;
	return pFile && fclose(pFile) == 0;
}

bool WorkFTPUpload::Seek(long lOffset, TOriginEnum enOrigin)
{
	return m_pFile && fseek(m_pFile, lOffset, enOrigin) == 0;
}

long WorkFTPUpload::Tell()
{
	if (!m_pFile)
		return -1L;
	return ftell(m_pFile);
}

size_t WorkFTPUpload::Write(const void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fwrite(pBuffer, itemSize, itemCount, m_pFile);
}

size_t WorkFTPUpload::Read(void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fread(pBuffer, itemSize, itemCount, m_pFile);
}

tstring WorkFTPUpload::GetLocalStreamName() const
{
	return m_strFileName;
}

UINT WorkFTPUpload::GetLocalStreamSize() const
{
	if (!m_pFile)
		return 0;

	return m_uSize;
}

void WorkFTPUpload::SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream)
{
	Seek(dwOffsetFromBeginOfStream, WorkFTPUpload::orBegin);
}

void WorkFTPUpload::OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes)
{
	Write(&(*vBuffer.begin()), sizeof(nsFTP::TByteVector::value_type), lReceivedBytes);
}

void WorkFTPUpload::OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend)
{
	m_current += bufferSize;
	bytesToSend = Read(pszBuffer, sizeof(char), bufferSize);

	//abort 호출
	if (m_pVolumeData->threadStop)
	{
		if (NULL != m_pClient) m_pClient->Abort();
	}

	setProgressValue(m_current / float(m_uSize) * 100);
}

void WorkFTPUpload::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}



WorkFTPUpload::WorkFTPUpload(VOLUME_DATA* pVolumeData, QString& strLatest) :
	m_pVolumeData(pVolumeData)
{
	m_strLatest = strLatest;
	m_pFile = NULL;
	m_current = 0;
	m_pClient = NULL;
	setProgressValue(0, true);
}

void WorkFTPUpload::threadRun()
{
	std::string mac_address;
	bool bCancel;
	while ((bCancel = network::GetMacAddress(mac_address)) == true)
	{
		//QMessageBox::warning(this, "address", mac_address.c_str());
		nsFTP::CFTPProtocolOutput protocolOutput;
		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));

		// jhc [2021.03.30] - ftp 로그인시 간헐적으로 안되는 현상으로 retry count 3회 적용.
		int loginRetryCount = FTP_FUNCTION_RETRY_COUNT;
		while (loginRetryCount)
		{
			bCancel = ftpClient.Login(logonInfo);
			loginRetryCount--;
			if (bCancel)
				break;
		}

		if (!bCancel)
		{
			break;
		}

		tstring wmac_address;
		wmac_address.assign(mac_address.begin(), mac_address.end());
		ftpClient.MakeDirectory(wmac_address);
		ftpClient.ChangeWorkingDirectory(wmac_address);

		QDateTime time = QDateTime::currentDateTime();
		QString savename = QString("%1.mip").arg(time.toString(Qt::ISODateWithMs));
		savename.remove(':');

		if (!Open(m_strLatest.toStdWString(), _T("rb")))
		{
			emit finished();
			return;
		}
		m_pClient = &ftpClient;
		bool r = ftpClient.UploadFile(*this, savename.toStdWString());

		bCancel = ftpClient.Logout();

		break;
	}

	if (bCancel)
	{
		m_pVolumeData->threadStop = true;
	}

	emit finished();
}