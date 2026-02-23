#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/Net/ProcessManager.h"
#include "TestStub/PACSServer/DicooglePACSServer.h"
#include <qdir>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestDicomNetworkSCU)
	{
	private:
		DicooglePACSServer* m_pDicooglePACSServer;
		DicomNetworkSCU* m_pSCUQueryRetrieve;
		DicomNetworkSCU* m_pSCUStore;

		QString m_testDataDirPath;
		QString m_downloadDirPath;
		QString m_dcmDirPath;
		QString m_imageDirPath;
		QString m_pdfDirPath;
		QStringList m_sample1_ImageFilePathList_Ten;
		QStringList m_sample2_ImageFilePathList_Ten;
		QStringList m_sample3_ImageFilePathList_Ten;

		std::vector<DICOM_HEADER_INFO> m_foundDicomHeaders;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			/* Dicoogle은 java로 실행됨. 시작전에 모든 Java 모듈 삭제 */
			fm::ProcessManager::KillByName(L"JaVa.exe");

			m_pDicooglePACSServer = new DicooglePACSServer();
			m_pSCUQueryRetrieve = new DicomNetworkSCU();
			m_pSCUStore = new DicomNetworkSCU();

			m_testDataDirPath = QDir::currentPath() + "/TestData";
			m_downloadDirPath = m_testDataDirPath + "/Download";
			m_dcmDirPath = m_testDataDirPath + "/dcm";
			m_imageDirPath = m_dcmDirPath + "/image";
			m_pdfDirPath = m_dcmDirPath + "/pdf";

			QString imageFilePath;
			for (int i = 0; i < 10; ++i)
			{
				imageFilePath.sprintf("%s/sample_1/%d.dcm", m_imageDirPath.toLocal8Bit().data(), i);
				m_sample1_ImageFilePathList_Ten << imageFilePath;
			}

			for (int i = 0; i < 10; ++i)
			{
				imageFilePath.sprintf("%s/sample_2/%d.dcm", m_imageDirPath.toLocal8Bit().data(), i);
				m_sample2_ImageFilePathList_Ten << imageFilePath;
			}

			for (int i = 0; i < 10; ++i)
			{
				imageFilePath.sprintf("%s/sample_3/%d.dcm", m_imageDirPath.toLocal8Bit().data(), i);
				m_sample3_ImageFilePathList_Ten << imageFilePath;
			}
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			m_pDicooglePACSServer->KillAndWaitProcess();
			m_pDicooglePACSServer->DeleteStorageDirectory();
			delete m_pDicooglePACSServer;
			delete m_pSCUQueryRetrieve;
			delete m_pSCUStore;
		}

		TEST_METHOD(TestJaveEXEShouldBeCleared)
		{
			Assert::IsFalse(fm::ProcessManager::IsProcessExist(L"Java.exe"));
		}

		/*
			해당 테스트에 실패할 경우 Test/TestData가 정상적으로 Copy되었는지 확인한다.
		*/
		TEST_METHOD(TestDcmFileExist)
		{
			QDir dir;

			Assert::IsTrue(dir.exists(m_testDataDirPath));
			Assert::IsTrue(dir.exists(m_dcmDirPath));
			Assert::IsTrue(dir.exists(m_imageDirPath));
			Assert::IsTrue(dir.exists(m_pdfDirPath));

			QFile file;
			for (auto& path : m_sample1_ImageFilePathList_Ten)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}

			for (auto& path : m_sample2_ImageFilePathList_Ten)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}

			for (auto& path : m_sample3_ImageFilePathList_Ten)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}
		}

		TEST_METHOD(TestEcho_InvalidServer)
		{
			/* 대기시간 1초 정도 필요 */
			m_pDicooglePACSServer->RunAndWaitProcess(1000);

			DicomNetworkSCU SCU_Invalid;
			SetupSCU(&SCU_Invalid,
				"invalid-AE-Title", "localhost", 99999);

			Assert::AreNotEqual((int)EDicomNetworkResult::SUCCESS, (int)SCU_Invalid.Echo());
		}

		TEST_METHOD(TestEcho_To_Dicoogle)
		{
			m_pDicooglePACSServer->RunAndWaitProcess(1000);

			SetupQueryRetrieveSCU();

			Assert::AreEqual((int)EDicomNetworkResult::SUCCESS, (int)m_pSCUQueryRetrieve->Echo());
		}

		TEST_METHOD(TestStore_Single_DICOM)
		{
			m_pDicooglePACSServer->RunAndWaitProcess(300);

			//SetupQueryRetrieveSCU();
			//SetupStoreSCU();

			//Assert::IsTrue(m_pSCUStore->Store(m_sample1_ImageFilePathList_Ten[0].toStdString()));

			///* 다음 명령어까지 300 ~ 500ms 대기가 필요*/
			//QThread::msleep(500);

			//Assert::IsTrue(SCUQuery_FindAllList());
			//Assert::AreEqual(1, (int)m_foundDicomHeaders.size());
		}

		TEST_METHOD(TestStore_Multi_DICOM)
		{
			m_pDicooglePACSServer->RunAndWaitProcess(300);

			//SetupQueryRetrieveSCU();
			//SetupStoreSCU();

			//std::vector<std::string> filePathList;
			//filePathList.push_back(m_sample1_ImageFilePathList_Ten[0].toStdString());
			//filePathList.push_back(m_sample2_ImageFilePathList_Ten[1].toStdString());
			//filePathList.push_back(m_sample3_ImageFilePathList_Ten[2].toStdString());

			//Assert::IsTrue(m_pSCUStore->Store(filePathList));

			///* 좀 더 많은 대기시간 필요 */
			//QThread::msleep(900);

			//Assert::IsTrue(SCUQuery_FindAllList());
			//Assert::AreEqual(3, (int)m_foundDicomHeaders.size());
		}

		TEST_METHOD(Dicoogle_Cant_DoGet)
		{
		}

	private:
		void SetupQueryRetrieveSCU()
		{
			SetupSCU(
				m_pSCUQueryRetrieve,
				m_pDicooglePACSServer->AETitle,
				m_pDicooglePACSServer->IP,
				m_pDicooglePACSServer->QueryRetrievePort
			);
		}

		void SetupStoreSCU()
		{
			SetupSCU(
				m_pSCUStore,
				m_pDicooglePACSServer->AETitle,
				m_pDicooglePACSServer->IP,
				m_pDicooglePACSServer->StorePort
			);
		}

		void SetupSCU(DicomNetworkSCU* pSCU, QString AETitle, QString IP, int port)
		{
			DicomHostInfo hostInfo = GetHostInfo(AETitle, IP, port);

			pSCU->SetHostInfoQueryRetrieve(hostInfo);
			pSCU->SetAETitle(L"STORESCP");
		}

		//bool SCUQuery_FindAllList()
		//{
		//	return m_pSCUQueryRetrieve->Find(fm::EDcmFindMode::DFM_PATIENT_NAME, "", fm::EDcmModality::ALL_MODALITY, &m_foundDicomHeaders);
		//}

		DicomHostInfo GetHostInfo(QString AETitle, QString IP, int port)
		{
			std::string transferSyntax = "";
			int maxAssoc = 0;
			EDcmProtocol protocol = C_GET;
			int timeout = 30;

			return DicomHostInfo(
				AETitle.toStdWString(),
				IP.toStdString(),
				transferSyntax,
				port,
				maxAssoc,
				protocol,
				timeout);
		}

	private:
	};
}