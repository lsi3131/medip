#include "stdafx.h"
#include "main.h"
#include "define.h"
#include "MedipQT.h"
#include "Windows/windowManager.h"
#include "Actions/ActionManager.h"
#include "graphics/volumedata.h"

#include "shellapi.h "
#include "mip/Log.h"

#include "System/UpdateManager.h"
#include "System/LogManager.h"
#include "System/ResourceManager.h"
#include "System/StringManager.h"
#include "System/LicenseManager.h"
#include "System/Config/ConfigManager.h"
#include "System/FileManager.h"
#include "System/ApplicationManager.h"
#include "System/ProductManager.h"
#include "System/MinidumpHelp.h"
#include "System/ShortcutManager.h"

#include "mipEngine/mipengine.h"
#include "mipDicom.h"
#include "Renderer/miprenderer.h"
#include "Renderer/Renderer.h"
#include "Macro/MacroRunner.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"
#include "Dialogs/SessionSelectDialog.h"
#include "Dialogs/Debug/DebugWidgetDialog.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

#include <QtWidgets/QApplication>
#include <QProcess>
#include <gtest/gtest.h>

//#define DEBUG_WIDGET_MODE

static bool IsArgumentsHasMacroMode(QStringList args)
{
	args.pop_front();

	QString macro;
	QStringListIterator it(args);
	while (it.hasNext())
	{
		QString arg = it.next();
		if (arg == "-macro")
		{
			if (it.hasNext())
			{
				macro = it.next();
			}
		}
	}

	return !macro.isEmpty();
}

HANDLE CreateAppMutex(const std::wstring& sessionName)
{
	std::wstring mutexName = L"";
#if defined(DEEP_CATCH_VER)
	mutexName = L"DeepCatch";
#elif defined(MEDIP_AI)
	mutexName = L"MEDIP_AI";
#else
	mutexName = L"MEDIP";
#endif

#if defined ALPHA_VERSION		//Alpha
	mutexName += L"_alpha";
#elif defined BETA_VERSION		//Beta
	mutexName += L"_beta";
#else										//Release

#endif

	if (!sessionName.empty())
	{
		mutexName += L"_" + sessionName;
	}

	HANDLE hMutex = NULL;
	hMutex = CreateMutex(NULL, TRUE, mutexName.c_str());
	return hMutex;
}

__declspec(noinline) int TRY_CODE(QApplication& app, int argc, char* argv[], bool allowMultiProcess)
{
	// 201104 허 건 대리
#ifdef DEV_EXCEPTION_DUMP
	MinidumpHelp dump;
	dump.install_self_mini_dump();
#endif

	HANDLE hMutex = NULL;
	QString sessionName;

	QStringList args = QApplication::arguments();
	QString errorMessage;

#ifdef USE_SESSION
	if (IsArgumentsHasMacroMode(args) == false)
	{
		SessionSelectDialog dlg;
		int result = dlg.exec();
		sessionName = dlg.GetSession();
		if (sessionName.isEmpty())
		{
			return 0;
		}
	}
#endif

	//if (!allowMultiProcess)
	{
		hMutex = CreateAppMutex(sessionName.toStdWString());
	}

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		bool res = true;
		if (argc >= 3)
		{
			QString strArgv = argv[1];

			if (QString("--Reboot") != strArgv)
				res = false;

			if (res)
			{
				strArgv = argv[2];
#if defined(DEEP_CATCH_VER)
				if (QString("--DeepCatch") != strArgv)
					res = false;
#elif defined(MEDIP_AI)
				if (QString("--MEDIP_AI") != strArgv)
					res = false;
#else
				if (QString("--Medip") != strArgv)
					res = false;
#endif
			}
		}
		else
			res = false;

		if (res)
		{
			DWORD dwRes = WaitForSingleObject(hMutex, INFINITE);
			res = ((WAIT_OBJECT_0 == dwRes) || (WAIT_ABANDONED == dwRes));

			if (!res)
			{
				hMutex = CreateAppMutex(sessionName.toStdWString());
				if (ERROR_SUCCESS != (dwRes = GetLastError()))
				{
#ifdef DEV_VER //todo outputdebugstring for medip
					QString str = QString("\n#########MEDIP#########");
					str.append(QString::number(dwRes));
					str.append(QString("\n#########MEDIP#########"));
					OutputDebugString((const wchar_t*)str.utf16());
#endif
					QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1017));
					return false;
				}
			}
		}
		else
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1010)).exec();
			return false;
		}
	}

	/* 임시 Hot Fix 버전에서는 "MEDIP"으로 App이름 설정 */
#ifndef TEMP_HOX_FIX_RELEASE
#if defined ALPHA_VERSION		//Alpha
	QString appName = LICENSE_DATA->getProductName() + "_alpha";
	APP_MNG->setAppName(appName);
#elif defined BETA_VERSION		//Beta
	QString appName = LICENSE_DATA->getProductName() + "_beta";
	APP_MNG->setAppName(appName);
#else										//Release
	APP_MNG->setAppName(LICENSE_DATA->getProductName());
#endif

	app.setApplicationName(APP_MNG->getAppName());		// application name 설정
#else
	APP_MNG->setAppName(LICENSE_DATA->getProductName());
#endif

	STRING_MANAGER->Initialize(sessionName);
	LICENSE_DATA->initLicenseMng();								// License dll에서 string manager의 app local path를 사용해야하기 때문에 생성자에서 함수 분리.
	APP_MNG->AppInit();

#if  !defined(DEV_NO_LOG)
	LOG_MNG->installMessageHandler();
#endif

	qInfo() << "program loaded.";

	WindowManager::GetSingletonRenderer();

	app.instance();

	// 원격접속인 경우 강제 접속 해제시키는 모듈
	QString pscpParam = "";
	pscpParam.append("1 /dest:console");
	QString basePath = "C:\\";

	// 	SHELLEXECUTEINFO pscpCmd;
	// 
	// 	pscpCmd.cbSize = sizeof(SHELLEXECUTEINFO);
	// 	pscpCmd.lpFile = L"tscon"; //프로세스 경로
	// 	pscpCmd.lpParameters = (const wchar_t*)pscpParam.utf16(); //tarCmd.lpParameters = _T("-zcvf"); //전달 인자
	// 	pscpCmd.nShow = SW_SHOWMINIMIZED; //실행시 최소화
	// 	pscpCmd.lpVerb = L"runas"; //관리자 권한 실행
	// 	pscpCmd.fMask = SEE_MASK_NOCLOSEPROCESS; //프로세스 핸들값 사용
	// 	pscpCmd.lpDirectory = (const wchar_t*)(basePath.utf16());

		//	if (ShellExecuteEx(&pscpCmd))
	{
		//TRACE("ShellExecuteEx Success\n");
		//		qDebug() << "Success : " << "ShellExecuteEx(&m_sInfo)" << endl;
	}

	//std::string strTmp = mip::Format("mipEngine Version :%s \n", mip::IsLIBVersion().c_str());

	std::string strMipEngine = mip::Format("mipEngine Version :%s \n", mipEngine().getLibVersion().c_str());
	printf("%s", strMipEngine.c_str());
	std::string strMipRenderer = mip::Format("mipRenderer Version :%s \n", mipRenderer().getLibVersion().c_str());
	printf("%s", strMipRenderer.c_str());
	std::string strMipDicom = mip::Format("mipDicom Version :%s \n", mipDicom().getLibVersion().c_str());
	printf("%s", strMipDicom.c_str());

	std::string strSettingProduct = LICENSE_DATA->getProductName().toUtf8();
	PRODUCT_MANAGER->initProductResource(strSettingProduct);
	if (RESOURCE_MANAGER->init() == false)
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0003)).exec();
		return -1;
	}

	/* login, license, update */
	char* lastParam = argv[argc - 1];
	if (!strcmp(lastParam, RUN_BY_UPDATER_INDICATOR))	// updater에 의해서 실행된 경우.(update 이후)
	{
		// 로그인 이후에 updater를 실행시키기 위해 medip이 종료되었다가 updater에 의해 다시 실행시켜줬기 때문에 
		// 로그인 이전 상태를 불러와 셋팅해준다.
		LICENSE_DATA->licenseResultInfoApply();
		if (!LICENSE_DATA->sseSubscribe())
		{
			QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), "SSE Subscribe fail!!!");
			return -1;
		}
	}
	else // 사용자가 실행한 경우.(default)
	{
		// 공지 사항 팝업.
		APP_MNG->noticeDlgPopup();

		// updater가 실행되있는 경우 메딥을 실행하지 않도록 처리.
		if (UPDATE_DATA->existProcess(UPDATE_DATA->MEDIP_UPDATER_FILENAME))
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1011)).exec();
			return -1;
		}

		// "medipVersionInfo.dat" 파일이 없는 경우 예외 처리.
		if (!LICENSE_DATA->IsLoadFail_MedipVersionInfoFile())
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0002)).exec();
			return -1;
		}

#if  !defined(DEV_NO_UPDATE)
		// updater update.
		UPDATE_DATA->updaterUpdate();
#endif

		// system time validation check. 
		if (!LICENSE_DATA->IsValidateSystemDate())
		{
			QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1018));
			return -1;
		}

		// system time config에 저장.
		LICENSE_DATA->saveSystemDate();

		// license check.s
		if (!LICENSE_DATA->licenseDlgPopup())
			return -1;

#if  !defined(DEV_NO_UPDATE)
		// update 있으면 updater call 후 medip 종료.
		if (UPDATE_DATA->medipUpdate())
			return 0;	// 업데이트 진행하는 경우 main은 정상 종료.
#endif
	}

	/* Product Type Setting전에 MEDIP인 경우에 Light(회원가입시 기본탑재) Product type만 남는 경우 필터링하여 메시지 표시 처리 */
	LICENSE_DATA->checkProductTypeStateChange();

	/* Product Type Setting */
	if (!LICENSE_DATA->productTypeSetting())
		return -1;

	if (!g_Renderer->init3D(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN)))
	{
		// 이 부분에서 1080tix2 쓰고 있는 PC에서 작동하지 않은 이력이 있음 (		
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0008)).exec();
		return -1;
	}

	PRODUCT_FACTORY;
	//g_Renderer->initFont(STRING_MANAGER->fontFileName.toLocal8Bit().constData());

	bool r = g_Renderer->initShader();
	WIN_MANAGER->initPresetTexture();


	{
		QImage* image = RESOURCE_MANAGER->getTexture_Font();
		if (image != NULL)
		{
			if (WIN_MANAGER->setFontTexture(image) == false)
			{
				// font error;
			}
		}
	}

	g_Renderer->doneCurrent();

	//////////////////////////////////////////////////////////////////////////////
	if ((!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
		!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && argc > 2)
	{
		QString strMacro = QString::fromUtf8(argv[1]);
		if (!strMacro.compare("-macro"))
		{
			ACTION_MANAGER->m_IsMacroMode = true;
			// macroMode일 때 UI 비활성화 개발 예정
		}
	}

	/* CONFIG 데이터 초기화 */
	CONFIG_MANAGER->init();


#ifdef DEV_FILE_MANAGER
	bool canPACSDownload = PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Import_PACSDownload);
	bool canPACSUpload = PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Export_PACSUpload);
	fm::EProductType productType = GetProductType(LICENSE_DATA);
	fm::ELanguageType languageType = fm::ELanguageType::ENG;

	if (STRING_MANAGER->locMap.loc == LOCALE::KOREAN)
	{
		languageType = fm::ELanguageType::KOR;
	}
	else
	{
		languageType = fm::ELanguageType::ENG;
	}

	QString fileManagerAppName = APP_MNG->getAppName() + "/" + sessionName;
	FILE_MANAGER->App.Initialize(
		fileManagerAppName,
		fm::ProductFunctionType(productType, canPACSDownload, canPACSUpload),
		QIcon::fromTheme("Medip", RESOURCE_MANAGER->getIcon(ICON_APP)),
		languageType);
#endif
	//////////////////////////////////////////////////////////////////////////////

	DATA_CONTEXT->SetWindowManager(WIN_MANAGER);
	DATA_CONTEXT->SetShorcutManager(SHORTCUT_MANAGER);

	OmniverseConfig* pOmniConfig = DATA_CONTEXT->GetOmniverseContext()->GetConfig();
	if (pOmniConfig->Init(STRING_MANAGER->OmniverseConfigJsonFilePath.toStdString()) == false)
	{
		qCritical() << "fail to intialize omniverse config : " << STRING_MANAGER->OmniverseConfigJsonFilePath;
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), STRING_MANAGER->getString(STR_APP_INIT_FAIL));
		return FALSE;
	}

	if (DATA_CONTEXT->GetOmniversePresetManager()->Init(STRING_MANAGER->Omniverse_AppData_DirectoryPath_Preset.toStdString()) == false)
	{
		qCritical() << "fail to initalize omniverse preset : " << STRING_MANAGER->Omniverse_AppData_DirectoryPath_Preset;
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), STRING_MANAGER->getString(STR_APP_INIT_FAIL));
		return FALSE;
	}

	MedipQT w;

	int res;
	if (FALSE == (res = w.init(DATA_CONTEXT)))
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), STRING_MANAGER->getString(STR_APP_INIT_FAIL));
		return -1;
	}
	else if (-1 == res)
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), STRING_MANAGER->getString(STR_APP_LICENSE_FAIL));
		return -1;
	}
	else if (-2 == res)
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), STRING_MANAGER->getString(STR_APP_PASSWORD_FAIL));
		return -1;
	}

	w.move(0, 0);
	w.showMaximized();

#if !defined(DEEPCATCH_VER_MACRO)
	if (argc > 1)
	{
		QString filename = QString::fromLocal8Bit(argv[1]);
		if (filename.compare("--Reboot") && filename.compare("-macro") && filename.compare(RUN_BY_UPDATER_INDICATOR))	// visual studio release 모드로 돌릴 때 들어오는 인자값 예외처리
		{
			bool bLatest = WIN_MANAGER->fileOpen(filename);															// updater에 의해 실행된 경우 마지막 인자로 "run_by_updater"가 추가된다.
			if (bLatest)
			{
				// 				QString ext = filename.section('.', -1);
				// 				if (!ext.contains(QRegularExpression(EXT_FILE_LIST)))
				WIN_MANAGER->lastestPathSave(filename);
			}
		}
	}
#endif

	if (ACTION_MANAGER->m_IsMacroMode)
	{
		w.IgnoreCloseEvent = true;
		QString macroFileParam = QString::fromLocal8Bit(argv[2]);
		QString strMacroFileName = macroFileParam;
		if (!macroFileParam.contains(QDir::separator(), Qt::CaseInsensitive) && !macroFileParam.contains("/", Qt::CaseInsensitive))
			strMacroFileName = QCoreApplication::applicationDirPath() + "/" + macroFileParam;
		// txt parsing

		int nRow = -1, nCol = -1;
		if (!ACTION_MANAGER->SettingMacroData(strMacroFileName, nRow, nCol))
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0005).arg(nRow).arg(nCol)).exec();
			return false;
		}

		ACTION_MANAGER->action_DeepCatch_MACRO_Start();

		// close 명령

	}

	int nResult = 0;
#ifdef USE_MEDIP_MACRO

	if (IsArgumentsHasMacroMode(args))
	{
		ACTION_MANAGER->m_IsMacroMode = true;
		MacroRunner macroRunner(DATA_CONTEXT->GetOmniverseContext(), WIN_MANAGER);
		macroRunner.Run(args);

		//bool macroRunSuccess = MacroRunner::RunWithArgments(args, &errorMessage);
		//if (macroRunSuccess)
		//{
		//	nResult = app.exec();
		//}
		//else
		//{
		//	QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), errorMessage);
		//	nResult = -1;
		//}
	}
	else
	{
		nResult = app.exec();
	}
#else
	nResult = app.exec();
#endif 



	bool bMutex = ReleaseMutex(hMutex);
	bool bHandle = CloseHandle(hMutex);
	hMutex = NULL;
	g_Renderer->release();
	// 	qDebug() << "bMutex : " << bMutex;
	// 	qDebug() << "bHandle : " << bHandle;
	// 	qDebug() << "nResult : " << nResult;

	return nResult;
}

__declspec(noinline) void EXCEPTION_CODE(unsigned long exceptionCode)
{
	QString errorMessage;
	// jhc [2022.01.05] - DeepCatch이고 macro모드인데 exception이 발생한 경우 기록 처리.
	if ((!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
		!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && ACTION_MANAGER->m_IsMacroMode)
	{
		ACTION_MANAGER->MacroErrorRecord(ACTION_MANAGER->m_ListMacroCommandLine.first(), errorMessage);
	}

	qInfo() << "EXCEPTION_CODE : " << exceptionCode << ", message : " << errorMessage;
	APP_MNG->SaveOpendFile();
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

#if defined(_WIN32) && !defined(_DEBUG) && 0
	if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
#endif

	if (argc >= 2)
	{
		QString arg(argv[1]);
		if (arg.contains("--gtest_list_tests") || arg.contains("--gtest_output") || arg.contains("--gtest_catch_exceptions"))
		{
			testing::InitGoogleTest(&argc, argv);

			int testsHaveErrors = RUN_ALL_TESTS();

			return testsHaveErrors;
		}
	}

	bool allowMultiObject = false;
#ifdef ALLOW_MULTI_OBJECT
	allowMultiObject = true;
#endif

#ifdef DEBUG_WIDGET_MODE
	DebugWidgetDialog dlg;
	dlg.exec();
	return 0;
#endif


	int ret = 0;
#ifdef DEV_EXCEPTION_DUMP	// 201104 허 건 대리 <= dump 저장수행
	ret = TRY_CODE(app, argc, argv, allowMultiObject);
#else
	__try
	{
		ret = TRY_CODE(app, argc, argv, allowMultiObject);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		EXCEPTION_CODE(GetExceptionCode());
	}
#endif

	return ret;
}

