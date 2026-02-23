#include "stdafx.h"
#include "UpdateManager.h"

#include "LicenseManager.h"
#include "styleManager.h"
#include "windowManager.h"
#include "stringManager.h"
#include "../version.h"

const QString UpdateManager::MEDIP_NOTICE_SERVER_URL				= tr("https://medicalip.net/licence/getVersion");
const QString UpdateManager::LICENSE_SERVER_GETVERSION_PATH			= tr("licence/getVersion");

const QString UpdateManager::UPDATER_NOTICE_SERVER_URL				= tr("http://medicalip.net/notice/version/getVersion");

const QString UpdateManager::MEDIP_UPDATER_FILENAME					= tr("MedicalIPUpdater.exe");
const int UpdateManager::UPDATER_CALL_MAX_WAITTING_TIME				= 1000;

//#include <process.h>
#include <TlHelp32.h>

UpdateManager::UpdateManager(void)
{
	updaterFilePath = qApp->applicationDirPath() + tr("/") + MEDIP_UPDATER_FILENAME;
}

UpdateManager::~UpdateManager(void)
{

}

void eventProcess(void *pEvent)
{
	LPCallBackEvent event = (LPCallBackEvent)pEvent;
	switch (event->nType)
	{
		case eCBETUpdateRequestEvent:
		{
			if (event->nEvent == eUIETMedipUpdateRequestEvent)
			{
				int nRet = ((LPUpdateRequestResult)(event->pRetData))->nResult;
				switch (nRet)
				{
					case eURRTSuccess:			// request 성공
					case eURRTNetworkError:
					case eURRTJsonInvalid:
					break;
				}
			}
			else if (event->nEvent == eUIETUpdaterUpdateRequestEvent)
			{
				int nRet = ((LPUpdateRequestResult)(event->pRetData))->nResult;
				switch (nRet)
				{
					case eURRTSuccess:			// request 성공
					{
						// update request 성공 후 다운로드 request 진행.
						DownloadRequestEvent event;
						event.nType = eUIETUpdaterDownloadRequestEvent;
						event.destinationPath = qApp->applicationDirPath();
						event.strConfigPath = STRING_MANAGER->m_strAppDataLocalPath;
						updaterLib::getInstance()->setEvent(&event, eventProcess);
					}
					break;
					case eURRTNetworkError:
					case eURRTJsonInvalid:
					case eURRTNeedToRedirect:
					case eURRTNetworkTimerOut:
					{
						UPDATE_DATA->progressEnd();
						// 실패 후 메시지 표시.
						QMessageBox::warning(nullptr, "Updater update", UPDATE_DATA->updateReqResultMessage(nRet));
					}
					break;
					default:
					{
						UPDATE_DATA->progressEnd();
					}
					break;
				}
			}
		}
		break;
		case eCBETDownloadRequestEvent:
		{
			int nRet = ((LPDownloadRequestResult)(event->pRetData))->nResult;
			switch (nRet)
			{
				case eFDFTSuccess:			// request 성공
				{
					// 다운로드 완료 콜백 받으면 프로그레스바 end
					UPDATE_DATA->progressEnd();
				}
				break;
				case eFDFTLoginFail:
				{
					// 다운로드 완료 콜백 받으면 프로그레스바 end
					UPDATE_DATA->progressEnd();
					QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0002)).exec();
				}
				break;
				case eFDFTFTPDownloadFail:
				{
					// 다운로드 완료 콜백 받으면 프로그레스바 end
					UPDATE_DATA->progressEnd();
					QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0003)).exec();
				}
				break;
				case eFDFTVerifyFail:
				{
					// 다운로드 완료 콜백 받으면 프로그레스바 end
					UPDATE_DATA->progressEnd();
					QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0004)).exec();
				}
				break;
				case eFDFTFTPListFail:
				{
					// 다운로드 완료 콜백 받으면 프로그레스바 end
					UPDATE_DATA->progressEnd();
					QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0005)).exec();
				}
				break;
				default:
				{
					UPDATE_DATA->progressEnd();
				}
				break;
			}
		}
		break;
		case eCBETProgressBarUpdateEvent:
		{
			// 프로그레스바 업데이트.
			qint64 received = ((LPProgressBarUpdateResult)(event->pRetData))->nAccumulationSize;
			qint64 total = ((LPProgressBarUpdateResult)(event->pRetData))->nTotalSize;
			UPDATE_DATA->progressUpdate(received, total);
		}
		break;
	}
}

// license 통과 후 update 내역 확인.
// update 내역이 있으면 updater 실행, 없으면 medip continue.
bool UpdateManager::medipUpdate()
{
	bool bResult = false;
	// 오프라인인 경우는 update를 안한다.
	if (LICENSE_DATA->getSelectedMode() == LAM_ONLINE)
	{
		/*
		// update 내역이 있는지 검사.(동기)
		UpdateRequestEvent event;
		event.nType = eUIETMedipUpdateAvailableCheckEvent;
		event.url = LICENSE_DATA->getUrl(eLSPTGetVersion);
		event.version = VER_FILE_VERSION_STR;
		event.serviceID = LICENSE_DATA->getLastLoginResultInfo().strServiceId;
		QString accessToken = tr("");
		accessToken = LICENSE_DATA->getLastLoginResultInfo().strAccessToken;
		event.accessToken = accessToken;
		event.companyName = LICENSE_DATA->getCompanyName();
		event.productName = LICENSE_DATA->getProductName();
		*/
		//if (updaterLib::getInstance()->isAvailableUpdate(&event))
		{
			LICENSE_DATA->licenseResultInfoFileSave();

			// update 내역이 존재하므로 updater 실행.
			// medip 죽이고 updater 실행.
			bResult = medipUpdaterExecute();
		}
	}
	return bResult;
}

quint16 UpdateManager::isUpdaterUpdateAvailable(QString &strVersion)
{
	quint16 nResult;
	// 오프라인인 경우는 update를 안한다.
	if (LICENSE_DATA->getSelectedMode() == LAM_ONLINE)
	{
		// update 내역이 있는지 검사.(동기)
		UpdateRequestEvent event;
		event.nType = eUIETUpdaterUpdateAvailableCheckEvent;
		event.url = UPDATER_NOTICE_SERVER_URL;
		event.version = strVersion;
		nResult = updaterLib::getInstance()->isAvailableUpdate(&event);
	}
	return nResult;
}

// updater' updater.
void UpdateManager::updaterUpdate()
{
	// 오프라인인 경우는 update를 안한다.
	if (LICENSE_DATA->getSelectedMode() == LAM_ONLINE)
	{
		// updater의 update 내역이 있을 경우만 진행.
		QString curUpdaterVersion = tr("");
		quint16 nResultCode;
		getUpdaterVersion(curUpdaterVersion);
		if ((nResultCode = isUpdaterUpdateAvailable(curUpdaterVersion)) == eURRTSuccess)
		{
			// update request 요청.
			UpdateRequestEvent event;
			event.nType = eUIETUpdaterUpdateRequestEvent;
			event.url = UPDATER_NOTICE_SERVER_URL;
			// updater.exe 버전 parsing.
			event.version = curUpdaterVersion;
			updaterLib::getInstance()->setEvent(&event, eventProcess);

			// 이벤트를 보내고 동기식 프로그레스바 show.
			progressBegin();

		}
		else
		{
			// 실패 후 메시지 표시.
			if (nResultCode != eURRTNoDownloadList)
				QMessageBox::warning(nullptr, "Updater update available check", updateReqResultMessage(nResultCode));
		}
	}
}

bool UpdateManager::medipUpdaterExecute()
{
	// arguments[1] : medip current version
	// arguments[2] : medip 실행 파일 경로.
	// arguments[3~] : medip 실행 Parameter.
	QStringList arguments = { VER_FILE_VERSION_STR };
	QString accessToken = tr("");
	accessToken = LICENSE_DATA->getLastLoginResultInfo().strAccessToken;
	arguments << accessToken;
	QStringList strListArgv = QCoreApplication::arguments();
	foreach(const QString &str, strListArgv)
	{
		arguments << str;
	}
//	QString medipProgramFilePath = LICENSE_DATA->getMedipPath() + tr("\\") + MEDIP_UPDATER_FILENAME;
	QString programFilePath = QCoreApplication::applicationDirPath() + tr("/") + MEDIP_UPDATER_FILENAME;
	QProcess *medipProcess = new QProcess(NULL);
	medipProcess->start(programFilePath, arguments);

	if (medipProcess->waitForStarted(UPDATER_CALL_MAX_WAITTING_TIME))	// updater 실행 성공.
		return true;
	else // updater 실행 실패.
		return false;
}

void UpdateManager::progressBegin()
{
	if (pProgressDlg != NULL)
	{
		pProgressDlg->deleteLater();
		pProgressDlg = NULL;
	}

//	pProgressDlg = new QProgressDialog((QWidget*)WIN_MANAGER->mainWindow);
	pProgressDlg = new QProgressDialog();
//	pProgressDlg->setWindowFlags(Qt::WindowCloseButtonHint);
	pProgressDlg->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	pProgressDlg->setStyleSheet(STYLE_MANAGER->mainFrame);
	pProgressDlg->setLabelText("Updater Auto Update");
//	pProgressDlg->setCancelButtonText(STRING_MANAGER->getString(STR_CANCEL));
	pProgressDlg->setRange(0, 100);
	pProgressDlg->setWindowTitle(STRING_MANAGER->getString(STR_WORK));
//	pProgressDlg->setWindowModality(Qt::ApplicationModal);
//	pProgressDlg->setMinimumDuration(500);
//	pProgressDlg->setMinimumDuration(0);
	pProgressDlg->setValue(0);
	pProgressDlg->show();

	QEventLoop loop;
	connect(pProgressDlg, SIGNAL(accepted()), &loop, SLOT(quit()));
	connect(pProgressDlg, SIGNAL(canceled()), &loop, SLOT(quit()));
	loop.exec();
}

void UpdateManager::progressUpdate(qint64 _received, qint64 _total)
{
	if (pProgressDlg)
	{
		if (_total > 0)
		{
			pProgressDlg->setValue(((_received * 100) / _total));
			// pProgressDlg->setValue(99);
		}
		else
			pProgressDlg->setValue(-1);
	}
}

void UpdateManager::progressEnd()
{
	if (pProgressDlg)
	{
		pProgressDlg->hide();
		pProgressDlg->done(QDialog::Accepted);
		pProgressDlg->deleteLater();
		pProgressDlg = NULL;
	}
}

void UpdateManager::getUpdaterVersion(QString &strVersion)
{
	// 버전정보를 담을 버퍼
	char* buffer = NULL;

	// 버전을 확인할 파일
	LPCWSTR name = (LPCWSTR)updaterFilePath.utf16();

	DWORD infoSize = 0;

	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
	infoSize = GetFileVersionInfoSize(name, 0);
	if (infoSize == 0) return;

	// 버퍼할당
	buffer = new char[infoSize];
	memset(buffer, 0x00, infoSize);
	if (buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if (GetFileVersionInfo(name, 0, infoSize, buffer) != 0)
		{
			VS_FIXEDFILEINFO* pFineInfo = NULL;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
			LPCWSTR strSubBlock = TEXT("\\");
			if (VerQueryValue(buffer, strSubBlock, (LPVOID*)&pFineInfo, &bufLen) != 0)
			{
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				// 파일버전 출력
			//	printf("version : %d,%d,%d,%d\n", majorVer, minorVer, buildNum, revisionNum);

				strVersion = QString::number(majorVer) + tr(".") + QString::number(minorVer) + tr(".") + QString::number(buildNum) + tr(".") + QString::number(revisionNum);
			}
		}
		delete[] buffer;
	}
}

bool UpdateManager::existProcess(const QString &_processName)
{
	bool bFind = false;
	QString processName = _processName;
	QString curProcess = "";

	HANDLE   hProcess = NULL;
	PROCESSENTRY32 pe32 = { 0 };

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcess, &pe32))
	{
		do
		{
			//	if (proc.Find(pe32.szExeFile) != -1) {
			curProcess = QString::fromWCharArray(pe32.szExeFile);
			if (!processName.compare(curProcess, Qt::CaseInsensitive))
			{
				bFind = true;
				break;
			}

		} while (Process32Next(hProcess, &pe32));
	}

	CloseHandle(hProcess);

	return bFind;
}

QString UpdateManager::updateReqResultMessage(qint16 resultCode)
{
	QString strResult = tr("");
	switch (resultCode)
	{
		case eURRTSuccess:
			strResult = "success";
		break;
		case eURRTNetworkError:
			strResult = "Network error occurred.";
		break;
		case eURRTJsonInvalid:
			strResult = "Received json data is empty";
		break;
		case eURRTNoDownloadList:
			strResult = "Received json data is invalid";
		break;
		case eURRTNeedToRedirect:
			strResult = "Network request need to be redirected.";
		break;
		case eURRTNetworkTimerOut:
			strResult = "Network time out.";
		break;
	}
	return strResult;
}