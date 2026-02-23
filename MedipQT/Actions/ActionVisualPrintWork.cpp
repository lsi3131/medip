#include "stdafx.h"
#include "ActionVisualPrintWork.h"
#include "Windows/windowManager.h"
#include "System/LicenseManager.h"
#include "System/stringManager.h"
#include "System/Config/ConfigManager.h"
#include "DataContext.h"

/*
	21.04.22 이상일 대리
	- ID, Passward 값 하드코딩 
	- 테스트 및 배포 시 해당 변수 수정할 것 
*/
std::wstring LOGIN_ID = L"admin_visual_printing";
std::wstring LOGIN_PASSWD = L"5zJu4L";

FTPUpload::FTPUpload(DataContext* pDataContext, QString &strLatest)
{
	m_pDataContext = pDataContext;
	m_strLatest = strLatest;	
	m_pFile = NULL;
	m_current = 0;
	m_pClient = NULL;
	setProgressValue(0, true);
}

bool FTPUpload::Open(const tstring& strFileName, const tstring& strMode)
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

bool FTPUpload::Close()
{
	FILE* pFile = m_pFile;
	m_pFile = NULL;
	return pFile && fclose(pFile) == 0;
}

bool FTPUpload::Seek(long lOffset, TOriginEnum enOrigin)
{
	return m_pFile && fseek(m_pFile, lOffset, enOrigin) == 0;
}

long FTPUpload::Tell()
{
	if (!m_pFile)
		return -1L;
	return ftell(m_pFile);
}

size_t FTPUpload::Write(const void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fwrite(pBuffer, itemSize, itemCount, m_pFile);
}

size_t FTPUpload::Read(void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fread(pBuffer, itemSize, itemCount, m_pFile);
}

tstring FTPUpload::GetLocalStreamName() const
{
	return m_strFileName;
}

UINT FTPUpload::GetLocalStreamSize() const
{
	if (!m_pFile)
		return 0;

	return m_uSize;
}

void FTPUpload::SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream)
{
	Seek(dwOffsetFromBeginOfStream, FTPUpload::orBegin);
}

void FTPUpload::OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes)
{
	Write(&(*vBuffer.begin()), sizeof(nsFTP::TByteVector::value_type), lReceivedBytes);
}

void FTPUpload::OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend)
{
	m_current += bufferSize;
	bytesToSend = Read(pszBuffer, sizeof(char), bufferSize);

	//abort 호출
	if (m_pDataContext->volume_data.threadStop)
	{
		if (NULL != m_pClient) m_pClient->Abort();
	}

	setProgressValue(m_current / float(m_uSize) * 100);
}

void FTPUpload::setProgressValue(int value, bool init)
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

void FTPUpload::threadRun()
{
//	201006 허건대리
//  기존소스코드 주석처리
//	std::string mac_address;
//	bool bCancel;
//	while ((bCancel = network::GetMacAddress(mac_address)) == true)
//	{
//		//QMessageBox::warning(this, "address", mac_address.c_str());
//		nsFTP::CFTPProtocolOutput protocolOutput;
//		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
//		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));
////		nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin"), tstring(L"medIcalIp808!"));
//		nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
//		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), 32769, tstring(L"lyi"), tstring(L"030801ab"));
//		bCancel = ftpClient.Login(logonInfo);
//		if (!bCancel)
//		{
//			QMessageBox::warning(NULL, "FTP Upload", "FTP Login Fail!");
//			break;
//		}
//		ftpClient.SetResumeMode(false); // File 덮어쓰기 옵션
//
//		/*tstring wmac_address;
//		QDateTime time = QDateTime::currentDateTime();
//		QString time_format = "yyyy_MM_dd_HH_mm_ss";
//		QString qstrIDName = "SRF/stl/public/data/" + LICENSE_DATA->getAuthenticatedID() +"/" + time.toString(time_format);
//		wmac_address = qstrIDName.toStdWString().c_str();
//		int nReturn;
//		nReturn = ftpClient.MakeDirectory(wmac_address);
//		nReturn = ftpClient.ChangeWorkingDirectory(wmac_address);*/
//
//		int nReturn;		
//		nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath);
//		nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);
//		nReturn = ftpClient.ChangeWorkingDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);
//
////		QDateTime time = QDateTime::currentDateTime();		
//		QString savename = m_strLatest.section("\\", -1);
//
//		if (!Open(m_strLatest.toStdWString(), _T("rb")))
//		{
//			emit finished();
//			return;
//		}
//		m_pClient = &ftpClient;
//		bool r = ftpClient.UploadFile(*this, savename.toStdWString());
//
//		bCancel = ftpClient.Logout();
//
//		break;
//	}
//	if (bCancel)
//	{
//		m_pDataContext->volume_data.threadStop = true;
//	}
//
//	emit finished();

	//	201006 허건대리
	//  수정 소스코드 
	int n_list = WIN_MANAGER->roiUploadList.size();

	bool bCancel;

	m_pDataContext->volume_data.threadResult = 1;

	for (int i = 0; i < n_list; ++i)
	{
		QString str_list = WIN_MANAGER->roiUploadList[i];

		std::string mac_address;
		//while ((bCancel = network::GetMacAddress(mac_address)) == true)
		{
			//QMessageBox::warning(this, "address", mac_address.c_str());
			nsFTP::CFTPProtocolOutput protocolOutput;
			nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
			//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));
			//		nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin"), tstring(L"medIcalIp808!"));

			//nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
			//nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.synology.me"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
			//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), 32769, tstring(L"lyi"), tstring(L"030801ab"));

			nsFTP::CLogonInfo logonInfo(
				tstring(CONFIG_MANAGER->getConfig_VisualPrint()->getUrl().toStdWString()),
				nsFTP::DEFAULT_FTP_PORT,
				LOGIN_ID, LOGIN_PASSWD);
	
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
				// 210330 허 건 과장
				// 오류발생으로 인한 주석처리
				//QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0001)).exec();

				m_pDataContext->volume_data.threadResult = -3;
				break;
			}
			else
			{
				ftpClient.SetResumeMode(false); // File 덮어쓰기 옵션

												/*tstring wmac_address;
												QDateTime time = QDateTime::currentDateTime();
												QString time_format = "yyyy_MM_dd_HH_mm_ss";
												QString qstrIDName = "SRF/stl/public/data/" + LICENSE_DATA->getAuthenticatedID() +"/" + time.toString(time_format);
												wmac_address = qstrIDName.toStdWString().c_str();
												int nReturn;
												nReturn = ftpClient.MakeDirectory(wmac_address);
												nReturn = ftpClient.ChangeWorkingDirectory(wmac_address);*/

				int nReturn;
				nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath);
				nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);
				nReturn = ftpClient.ChangeWorkingDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);

				//		QDateTime time = QDateTime::currentDateTime();		
				QString savename = str_list.section("\\", -1);

				if (!Open(str_list.toStdWString(), _T("rb")))
				{
					emit finished();
					return;
				}
				m_pClient = &ftpClient;
				bool r = ftpClient.UploadFile(*this, savename.toStdWString());

				bCancel = ftpClient.Logout();

				// 201014 허 건대리
				// 파일 Opne 후, Clolse를 필히 해주어야 함.
				Close();
			}
		}
	}

	if (bCancel)
	{
		m_pDataContext->volume_data.threadStop = true;
	}

	emit finished();
}



//// ROI Upload  //////////////////////////////////////////////////////////////

bool FTPUploadROI::Open(const tstring& strFileName, const tstring& strMode)
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

bool FTPUploadROI::Close()
{
	FILE* pFile = m_pFile;
	m_pFile = NULL;
	return pFile && fclose(pFile) == 0;
}

bool FTPUploadROI::Seek(long lOffset, TOriginEnum enOrigin)
{
	return m_pFile && fseek(m_pFile, lOffset, enOrigin) == 0;
}

long FTPUploadROI::Tell()
{
	if (!m_pFile)
		return -1L;
	return ftell(m_pFile);
}

size_t FTPUploadROI::Write(const void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fwrite(pBuffer, itemSize, itemCount, m_pFile);
}

size_t FTPUploadROI::Read(void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fread(pBuffer, itemSize, itemCount, m_pFile);
}

tstring FTPUploadROI::GetLocalStreamName() const
{
	return m_strFileName;
}

UINT FTPUploadROI::GetLocalStreamSize() const
{
	if (!m_pFile)
		return 0;

	return m_uSize;
}

void FTPUploadROI::SetLocalStreamOffset(DWORD dwOffsetFromBeginOfStream)
{
	Seek(dwOffsetFromBeginOfStream, FTPUploadROI::orBegin);
}

void FTPUploadROI::OnBytesReceived(const nsFTP::TByteVector& vBuffer, long lReceivedBytes)
{
	Write(&(*vBuffer.begin()), sizeof(nsFTP::TByteVector::value_type), lReceivedBytes);
}

void FTPUploadROI::OnPreBytesSend(char* pszBuffer, size_t bufferSize, size_t& bytesToSend)
{
	m_current += bufferSize;
	bytesToSend = Read(pszBuffer, sizeof(char), bufferSize);

	//abort 호출
	if (m_pDataContext->volume_data.threadStop)
	{
		if (NULL != m_pClient) m_pClient->Abort();
	}

	setProgressValue(m_current / float(m_uSize) * 100);
}

void FTPUploadROI::setProgressValue(int value, bool init)
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

FTPUploadROI::FTPUploadROI(DataContext* pDataContext, QString &strLatest)
{
	m_pDataContext = pDataContext;
	m_strLatest = strLatest;
	m_pFile = NULL;
	m_current = 0;
	m_pClient = NULL;
	setProgressValue(0, true);
}

void FTPUploadROI::threadRun()
{	
	m_pDataContext->volume_data.threadResult = 1;

	std::string mac_address;
	bool bCancel;
	//while ((bCancel = network::GetMacAddress(mac_address)) == true)
	{		
		nsFTP::CFTPProtocolOutput protocolOutput;
		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"lyi"), tstring(L"030801ab"));
// <<<<<<< .working
// 		//nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
// ||||||| .merge-left.r4405
// 		nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
// =======
// 		//nsFTP::CLogonInfo logonInfo(tstring(L"147.47.229.147"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
// 		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.synology.me"), nsFTP::DEFAULT_FTP_PORT, tstring(L"admin_visual_printing"), tstring(L"5zJu4L"));
// >>>>>>> .merge-right.r4449
// 		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), 32769, tstring(L"lyi"), tstring(L"030801ab"));

		nsFTP::CLogonInfo logonInfo(
			tstring(CONFIG_MANAGER->getConfig_VisualPrint()->getUrl().toStdWString()),
			nsFTP::DEFAULT_FTP_PORT,
			LOGIN_ID, LOGIN_PASSWD);

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
			// 213030 허 건 과장 
			// 메시지 박스 에러발생 유발로 주석처리
			//QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0001)).exec();

			m_pDataContext->volume_data.threadResult = -3;
		}
		else
		{
			ftpClient.SetResumeMode(false); // File 덮어쓰기 옵션

											/*tstring wmac_address;
											QString qstrIDName = "SRF/stl/public/data/" + LICENSE_DATA->getAuthenticatedID();
											wmac_address = qstrIDName.toStdWString().c_str();
											int nReturn;
											nReturn = ftpClient.MakeDirectory(wmac_address);
											nReturn = ftpClient.ChangeWorkingDirectory(wmac_address);*/

											//		WIN_MANAGER->m_tstrPath
			int nReturn;
			/*nReturn = ftpClient.MakeDirectory(WIN_MANAGER->m_tstrUploadFullPath);
			nReturn = ftpClient.ChangeWorkingDirectory(WIN_MANAGER->m_tstrUploadFullPath);*/
			nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath);
			nReturn = ftpClient.MakeDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);
			nReturn = ftpClient.ChangeWorkingDirectory(WIN_MANAGER->tstrUploadIDPath + WIN_MANAGER->tstrUploadDateTimePath);

			QDateTime time = QDateTime::currentDateTime();
			QString savename = m_strLatest.section("\\", -1);

			if (!Open(m_strLatest.toStdWString(), _T("rb")))
			{
				emit finished();
				return;
			}
			m_pClient = &ftpClient;
			bool r = ftpClient.UploadFile(*this, savename.toStdWString());

			bCancel = ftpClient.Logout();
		}
	}

	if (bCancel)
	{
		m_pDataContext->volume_data.threadStop = true;
	}
	
	emit finished();
}


// Web Link //////////////////////////////////////////////////////////////////////////////
WebLink::WebLink(DataContext* pDataContext, VisualPrintConfig* pConfig)
{
	m_pDataContext = pDataContext;

	QString qstrIDName = LICENSE_DATA->getAuthenticatedID();
	m_pVisualPrintConfig = pConfig;
//	QDesktopServices::openUrl(QUrl(QString("http://147.47.229.147:2999/?reqid=" + qstrIDName)));

	//QDesktopServices::openUrl(QUrl(QString("http://147.47.229.147:2999/?reqid=" + WIN_MANAGER->qstrURL_LinkPath)));
	//QDesktopServices::openUrl(QUrl(QString("http://medicalip.synology.me:2999/?reqid=" + WIN_MANAGER->qstrURL_LinkPath)));
	
	QString url = QString("http://" + m_pVisualPrintConfig->getUrl() + ":")
		+ QString::number(m_pVisualPrintConfig->getPort())
		+ QString("/?reqid=" + WIN_MANAGER->qstrURL_LinkPath);
	
	QDesktopServices::openUrl(QUrl(url));
	
	m_pFile = NULL;
	m_current = 0;
	setProgressValue(0, true);
}

bool WebLink::Open(const tstring& strFileName, const tstring& strMode)
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

bool WebLink::Close()
{
	FILE* pFile = m_pFile;
	m_pFile = NULL;
	return pFile && fclose(pFile) == 0;
}

bool WebLink::Seek(long lOffset, TOriginEnum enOrigin)
{
	return m_pFile && fseek(m_pFile, lOffset, enOrigin) == 0;
}

long WebLink::Tell()
{
	if (!m_pFile)
		return -1L;
	return ftell(m_pFile);
}

size_t WebLink::Write(const void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fwrite(pBuffer, itemSize, itemCount, m_pFile);
}

size_t WebLink::Read(void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fread(pBuffer, itemSize, itemCount, m_pFile);
}

void WebLink::setProgressValue(int value, bool init)
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

void WebLink::threadRun()
{
	m_pDataContext->volume_data.threadResult = 1;

	std::string mac_address;
	bool bCancel;
	//while ((bCancel = network::GetMacAddress(mac_address)) == true)
	{
		//QMessageBox::warning(this, "address", mac_address.c_str());
		nsFTP::CFTPProtocolOutput protocolOutput;
		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));
		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"lyi"), tstring(L"030801ab"));
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), 32769, tstring(L"lyi"), tstring(L"030801ab"));
		
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
			m_pDataContext->volume_data.threadResult = -3;
			//break;
		}
		else
		{
			ftpClient.SetResumeMode(false); // File 덮어쓰기 옵션

			tstring wmac_address;
			//		wmac_address.assign(mac_address.begin(), mac_address.end());
			//		ftpClient.MakeDirectory(wmac_address);
			wmac_address = L"modip1/STLRendering/data/VisualPrinting";
			ftpClient.ChangeWorkingDirectory(wmac_address);
			//		ftpClient.ChangeWorkingDirectory(L"modip1\\STLRendering\\data\\VisualPrinting\\kidney\\binary");

			QDateTime time = QDateTime::currentDateTime();
			//		QString savename = m_strLatest;// QString("%1.mip").arg(time.toString(Qt::ISODateWithMs));
			//		QStringList list = m_strLatest.split("\\");
			QString savename = m_strLatest.section("\\", -1);

			if (!Open(m_strLatest.toStdWString(), _T("rb")))
			{
				emit finished();
				return;
			}
			//		m_pClient = &ftpClient;
			//		bool r = ftpClient.UploadFile(*this, savename.toStdWString());

			bCancel = ftpClient.Logout();
		}
	}

	if (bCancel)
	{
		m_pDataContext->volume_data.threadStop = true;
	}

	emit finished();
}


// Visual Print Dialog Worker //////////////////////////////////////////////////////////////////////////////
VisualPrintDlgWorker::VisualPrintDlgWorker(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

//	QString qstrIDName = LICENSE_DATA->getAuthenticatedID();
//	QDesktopServices::openUrl(QUrl(QString("http://147.47.229.147:2999/?reqid=" + qstrIDName)));
	m_pFile = NULL;
	m_current = 0;
	setProgressValue(0, true);
}

bool VisualPrintDlgWorker::Open(const tstring& strFileName, const tstring& strMode)
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

bool VisualPrintDlgWorker::Close()
{
	FILE* pFile = m_pFile;
	m_pFile = NULL;
	return pFile && fclose(pFile) == 0;
}

bool VisualPrintDlgWorker::Seek(long lOffset, TOriginEnum enOrigin)
{
	return m_pFile && fseek(m_pFile, lOffset, enOrigin) == 0;
}

long VisualPrintDlgWorker::Tell()
{
	if (!m_pFile)
		return -1L;
	return ftell(m_pFile);
}

size_t VisualPrintDlgWorker::Write(const void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fwrite(pBuffer, itemSize, itemCount, m_pFile);
}

size_t VisualPrintDlgWorker::Read(void* pBuffer, size_t itemSize, size_t itemCount)
{
	if (!m_pFile)
		return 0;
	return fread(pBuffer, itemSize, itemCount, m_pFile);
}

void VisualPrintDlgWorker::setProgressValue(int value, bool init)
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

	if ((val + 20) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void VisualPrintDlgWorker::threadRun()
{
	m_pDataContext->volume_data.threadResult = 1;

	// save stl & js file
	{
		int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

		QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
		QString folder_name = "Temp";
		QString save_path = current_path + "\\" + folder_name;

		if (!QDir(save_path).exists())
		{
			QDir().mkdir(save_path);
		}

		for (int i = 0; i < n_mesh; ++i)
		{
			auto		mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

			auto		mesh_name = m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(i);

			QString save_file = save_path + "\\" + mesh_name +".stl";
			WIN_MANAGER->saveMeshFilesVisualPrint(save_file, i, mesh_info->uid, EX_FILES_STL, false, false);
		}

		// For js file
		QString file = save_path + "\\files.js";
		//QFile::remove(file);

		FILE *fp = fopen(file.toLocal8Bit().toStdString().c_str(), "wt");

		fputs("var files = [\n", fp);

		for (int i = 0; i < n_mesh; ++i)
		{
			auto		mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

			auto		mesh_name = m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(i);
						

			// Update js file
			{
				QString fileName = "";
				QString strSTL = "";

				strSTL = save_path;

				fileName = strSTL + "/" + mesh_name + ".stl";;
				fileName.replace("/", "\\");

				QString savename = fileName.section("\\", -1);

				// Mesh 정보 파일 Export			
				fputs("{\n", fp);

				// Mesh Name Export
				QString tempName = savename;
				tempName.chop(4);

				QString FullName = "\"name\": \"" + tempName + "\",\n";
				fputs(FullName.toStdString().c_str(), fp);

				// Export Path Info			
				QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
				FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
				fputs(FullName.toStdString().c_str(), fp);

				char cTemp[16];
				snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", mesh_info->color.r, mesh_info->color.g, mesh_info->color.b);
				tempName = QString(cTemp);
				FullName = "\"color\" : \"#" + tempName + "\",\n";
				fputs(FullName.toStdString().c_str(), fp);
				fputs("\"opacity\" : 1,\n", fp);
				fputs("\"visible\" : true,\n", fp);
				fputs("},\n", fp);
			}
		}

		fputs("];", fp);
		fclose(fp);
	}

	std::string mac_address;
	bool bCancel;
	//while ((bCancel = network::GetMacAddress(mac_address)) == true)
	{
		//QMessageBox::warning(this, "address", mac_address.c_str());
		nsFTP::CFTPProtocolOutput protocolOutput;
		nsFTP::CFTPClient ftpClient(nsSocket::CreateDefaultBlockingSocketInstance(), 30);
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"medipclient"), tstring(L"mcdbwj56!!"));
		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), nsFTP::DEFAULT_FTP_PORT, tstring(L"lyi"), tstring(L"030801ab"));
		//		nsFTP::CLogonInfo logonInfo(tstring(L"medicalip.net"), 32769, tstring(L"lyi"), tstring(L"030801ab"));

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
			m_pDataContext->volume_data.threadResult = -3;
			//break;
		}

		//ftpClient.SetResumeMode(false); // File 덮어쓰기 옵션

		//tstring wmac_address;
		////		wmac_address.assign(mac_address.begin(), mac_address.end());
		////		ftpClient.MakeDirectory(wmac_address);
		//wmac_address = L"modip1/STLRendering/data/VisualPrinting";
		//ftpClient.ChangeWorkingDirectory(wmac_address);
		////		ftpClient.ChangeWorkingDirectory(L"modip1\\STLRendering\\data\\VisualPrinting\\kidney\\binary");

		//QDateTime time = QDateTime::currentDateTime();
		////		QString savename = m_strLatest;// QString("%1.mip").arg(time.toString(Qt::ISODateWithMs));
		////		QStringList list = m_strLatest.split("\\");
		//QString savename = m_strLatest.section("\\", -1);

		//if (!Open(m_strLatest.toStdWString(), _T("rb")))
		//{
		//	emit finished();
		//	return;
		//}
		////		m_pClient = &ftpClient;
		////		bool r = ftpClient.UploadFile(*this, savename.toStdWString());

		//bCancel = ftpClient.Logout();

		//break;
	}

	if (bCancel)
	{
		m_pDataContext->volume_data.threadStop = true;
	}

	emit finished();
}