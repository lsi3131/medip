#include "stdafx.h"
#include "ApplicationManager.h"
#include "stringManager.h"

#include "graphics/volumedata.h"
#include "defineMipEncoder.h"
#include "LicenseManager.h"
#include "ActionManager.h"
#include "windowManager.h"

#include "Dialogs/NoticeDisplayDlg.h"

#include "Network/Network.h"
#include "MedipQT.h"

const int ApplicationManager::AUTO_SAVE_DEFAULT_INTERVAL = 10;		// 10분
const int ApplicationManager::AUTO_SAVE_INTERVAL_RATIO = 60000;		// 1분은 60초

ApplicationManager::ApplicationManager()
{
	m_bAutoSaveFlag = false;
	m_nAutoSaveInterval = AUTO_SAVE_DEFAULT_INTERVAL;
	m_bFileOpened = false;
	m_pDocAutoSaveTimer = nullptr;
	connect(this, &ApplicationManager::fileLoadingFinishedSignal, this, &ApplicationManager::fileLoadingFinishedSlot);
}

ApplicationManager::~ApplicationManager()
{
	if (m_pDocAutoSaveTimer)
		m_pDocAutoSaveTimer->stop();
}

void ApplicationManager::AppInit()
{
#ifdef DEV_USE_APPDATA_PATH
	AppDataDirectoryInit();
#endif

	// auto save flag, interval getting.
	QString strVal = "";
	if (WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_autoSave, strVal))
	{
		if (!strVal.isEmpty())
			m_bAutoSaveFlag = QVariant(strVal).toBool();
	}
	strVal = "";
	if (WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_autoSaveInterval, strVal))
	{
		if (!strVal.isEmpty())
			m_nAutoSaveInterval = QVariant(strVal).toInt();
	}
}

void ApplicationManager::AppExit()
{
//	QApplication::quit();
	QApplication::closeAllWindows();
}

void ApplicationManager::AppLogOut()
{
	QVector<eLicenseConfigElementType> keyVector{ eLCETLoginAuto, eLCETLoginAutoID,  eLCETLoginAutoPWD };
	QStringList valueList;
	valueList << QVariant(false).toString() << "" << "";
	LICENSE_DATA->setConfig_License(keyVector, valueList);
	WIN_MANAGER->mainWindow->RebootProgram();
}

/* AppData로 경로 이동에 따른 초기화 처리 */
void ApplicationManager::AppDataDirectoryInit()
{
	QString strInstallDir = "";
	strInstallDir = QCoreApplication::applicationDirPath();
	// 1. 설치 경로의 license 폴더가 AppData 경로에 없으면 복사 진행.
	copyFromProgramFileDirToAppDataDir(STRING_MANAGER->m_strAppDataLocalPath, strInstallDir, "license");

	// 2. 설치 경로의 "config.dat" 파일을 AppData 경로에 없으면 복사 진행.
	QString sourceConfigFilePath = strInstallDir + "/config.dat";
	QString destinationConfigFilePath = STRING_MANAGER->m_strAppDataLocalPath + "/config.dat";
	if (!QFile::exists(destinationConfigFilePath))
		QFile::copy(sourceConfigFilePath, destinationConfigFilePath);

	QString strWeightPath = STRING_MANAGER->m_strLocalAISegWeightPath;
	QDir dir(strWeightPath);
	if (!dir.exists())
		QDir().mkpath(strWeightPath);
}

void ApplicationManager::copyFromProgramFileDirToAppDataDir(QString AppDataLocalProductPath, QString strInstallDir, QString processedDirName)
{
	QString prdRootPath = "";
	QString prdAppDataRootPath = "";
	QString copiedFilePath = ("");
	QString copiedFileName = ("");
	QString pastedFilePath = ("");
	QString pastedFileName = ("");
	QString exeFilePath = ("");
	QString pastedDirPath = ("");
	QDir sourceDir;
	QDir destinationDir;

	prdRootPath = strInstallDir + ("/") + processedDirName;
	prdAppDataRootPath = AppDataLocalProductPath + ("/") + processedDirName;
	QStringList strFilters;
	strFilters += "*.*";
	QDirIterator iterDir(prdRootPath, strFilters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (iterDir.hasNext())
	{
		iterDir.next();
		copiedFilePath = iterDir.filePath();
		exeFilePath = iterDir.filePath().mid(iterDir.path().length());
		pastedFilePath = prdAppDataRootPath + exeFilePath;
		//	pastedDirPath	= pastedFilePath;
		//	pastedDirPath.chop(iterDir.fileName().length() + 1);
		pastedDirPath = pastedFilePath.section("/", 0, -2);

		// 디렉토리가 없는 경우 디렉토리 생성부터.
		destinationDir.setPath(pastedDirPath);
		if (!destinationDir.exists())
			destinationDir.mkpath(".");

		// appdata에 program file에서 가져올 파일이 존재하면 유지, 존재하지 않으면 복사 진행.
		if (!QFile::exists(pastedFilePath))
		{
			copiedFileName = iterDir.fileName();
			if (copiedFileName.contains("medipVersionInfo", Qt::CaseInsensitive))
				continue;

			QFile::copy(copiedFilePath, pastedFilePath);
		}

	}
}

void ApplicationManager::SaveOpendFile()
{
	qInfo("auto saved.");
	
//	QString strFile = STRING_MANAGER->m_strTempFilePath;
	QString strFile = STRING_MANAGER->m_strAppDataLocalPath + QString("/dump/");

	if (!QDir(strFile).exists())
	{
		QDir().mkdir(strFile);
	}

	QDateTime time;
	strFile += time.currentDateTime().toString(QString("yyyy-MM-dd_hh-mm-ss"));

	MIP_ENCODER::PROJ_TYPE eType;

	if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP))
	{
		strFile += ".mip";
		eType = MIP_ENCODER::PT_MIP;
	}
	else if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH))
	{
		strFile += ".mipd";
		eType = MIP_ENCODER::PT_MIPD;
	}

	ACTION_MANAGER->action_FileWork_Export_MipFile(strFile, eType, false);

}

// MIP, MIPD 파일 로딩이 정상적으로 완료되면 auto save 조건 총족.
void ApplicationManager::fileLoadingFinishedSlot()
{
	m_bFileOpened = true;
	if (m_bAutoSaveFlag)
	{
		// document auto save timer start.
		if (!m_pDocAutoSaveTimer)
		{
			m_pDocAutoSaveTimer = new QTimer(this);
			connect(m_pDocAutoSaveTimer, SIGNAL(timeout()), this, SLOT(AutoSave()));
			m_pDocAutoSaveTimer->start(m_nAutoSaveInterval*AUTO_SAVE_INTERVAL_RATIO);
		}
		else
		{
			m_pDocAutoSaveTimer->stop();
			m_pDocAutoSaveTimer->start(m_nAutoSaveInterval*AUTO_SAVE_INTERVAL_RATIO);
		}
	}
	else
	{
		if (m_pDocAutoSaveTimer)
			m_pDocAutoSaveTimer->stop();
	}
}

void ApplicationManager::AutoSave()
{
	// 현재 동작하는 Thread가 없으면 auto save 동작.
	QString strLatest = "";
	bool latest = WIN_MANAGER->lastestPathGet(strLatest);
	if (latest)
		WIN_MANAGER->fileSave(strLatest);
}

// auto save flag가 업데이트 되면.
void ApplicationManager::updateAutoSaveFlag(bool val)
{ 
	if (m_bAutoSaveFlag != val)
	{
		m_bAutoSaveFlag = val;
		if (!m_bAutoSaveFlag)
		{
			if (m_pDocAutoSaveTimer)
				m_pDocAutoSaveTimer->stop();
		}
		else
		{
			if (m_bFileOpened)
			{
				if (!m_pDocAutoSaveTimer)
				{
					m_pDocAutoSaveTimer = new QTimer(this);
					connect(m_pDocAutoSaveTimer, SIGNAL(timeout()), this, SLOT(AutoSave()));
					m_pDocAutoSaveTimer->start(m_nAutoSaveInterval*AUTO_SAVE_INTERVAL_RATIO);
				}
				else
				{
					m_pDocAutoSaveTimer->stop();
					m_pDocAutoSaveTimer->start(m_nAutoSaveInterval*AUTO_SAVE_INTERVAL_RATIO);
				}
			}
			else
			{
				if (m_pDocAutoSaveTimer)
					m_pDocAutoSaveTimer->stop();
			}
		}
		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_autoSave, QVariant(m_bAutoSaveFlag).toString());
	}
}

// auto save interval이 업데이트 되면.
void ApplicationManager::updateAutoSaveInteval(int val)
{
	if (m_nAutoSaveInterval != val)
	{
		m_nAutoSaveInterval = val;
		if (m_pDocAutoSaveTimer)
		{
			m_pDocAutoSaveTimer->stop();
			m_pDocAutoSaveTimer->start(m_nAutoSaveInterval*AUTO_SAVE_INTERVAL_RATIO);
		}
		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_autoSaveInterval, QVariant(m_nAutoSaveInterval).toString());
	}
}

bool ApplicationManager::noticeDlgPopup()
{
	// 서버에서 getNoticeList API의 결과가 있으면 보여주고 없으면 리턴.
	QList<sNoticeListInfo> noticeListVec;
	if (network::getNoticeList(LicenseManager::NOTICE_SERVER_NOTICE_GETNOTICELIST_URL, noticeListVec) && !noticeListVec.isEmpty())
	{
		// 현재 로컬에 저장된 filterNoticeList 파일을 읽어 해당 Notcie Subject는 노티스 리스트에서 필터링하고 보여준다.
		QStringList filterNoticeList;
		LICENSE_DATA->getFilterNoticeList(filterNoticeList);
		QList<sNoticeListInfo>::iterator iter = noticeListVec.begin();
		while (iter != noticeListVec.end())
		{
			bool bdelete = false;
			for (int i = 0; i < filterNoticeList.size(); i++)
			{
				if (!iter->noticeSubject.compare(filterNoticeList.at(i)))
				{
					iter = noticeListVec.erase(iter);
					bdelete = true;
					break;
				}
			}
			if (!bdelete)
				++iter;
		}

		if (!noticeListVec.isEmpty())
		{
			NoticeDisplayDlg dlg(noticeListVec);
			dlg.exec();

			// 다시 보지 않기 체크된 notice는 필터 리스트에 저장.
			if (dlg.isCheckDontSeeAgain())
				LICENSE_DATA->setFilterNoticeList(noticeListVec);

			return true;	// 공지사항 팝업한 경우.
		}
		else
			return false;
	}
	else 
		return false;	// 공지사항 팝업 안한 경우.
}