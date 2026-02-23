#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DicomFindOption.h"
#include "filemanager/dicom/Network/DicomNetworkSCP.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/dicom/DicomDatasetFactory.h"
#include "filemanager/Net/ProcessManager.h"
#include "filemanager/appcore/Util/StringUtil.h"
#include "TestStub/PACSServer/dcmqrscpPACSServer.h"
#include "DicomTestStub.h"
#include "SCUEventHandler.h"
#include <qdir>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;


namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestDicomNetworkSCU_dcmqrscp)
	{
	private:
		dcmqrscpPACSServer* m_pPACSServer;
		DicomNetworkSCU* m_pQueryRetrieveSCU;
		DicomNetworkSCU* m_pStoreSCU;
		DicomNetworkSCP* m_pStoreSCP;

		DicomTestStub m_testStub;
		std::wstring m_dcmFileExtension = L".dcm";
		std::vector<fm::DicomDataset> m_datasetList;

		std::vector<fm::DicomDataset> m_FIND_DicomHeaders;
		std::vector<std::wstring> m_Download_FilePaths;
		SCUEventHandler* m_pEventHandler;

		int m_maxImageCount = 100;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pPACSServer = new dcmqrscpPACSServer();
			m_pQueryRetrieveSCU = new DicomNetworkSCU();
			m_pStoreSCU = new DicomNetworkSCU();
			m_pStoreSCP = new DicomNetworkSCP();
			m_pEventHandler = new SCUEventHandler();

			SetupQueryRetrieveSCU();
			SetupStoreSCU();
			SetupStoreSCP();

			QObject::connect(m_pQueryRetrieveSCU, &DicomNetworkSCU::updateSCUStatus, m_pEventHandler, &SCUEventHandler::onSCUCommunicationInProgress);
		}


		TEST_METHOD_CLEANUP(TearDown)
		{
			m_pPACSServer->KillAndWaitProcess();
			m_pPACSServer->DeleteStorageDirectory();
			delete m_pPACSServer;
			delete m_pQueryRetrieveSCU;
			delete m_pStoreSCU;
			delete m_pStoreSCP;
			delete m_pEventHandler;
			m_datasetList.clear();
		}

		/*
			해당 테스트에 실패할 경우 BuildEvent상 Test/TestData가 정상적으로 Copy되었는지 확인한다.
		*/
		TEST_METHOD(TestDcmFileExist)
		{
			QDir dir;

			Assert::IsTrue(dir.exists(m_testStub.TestDataDirPath));
			Assert::IsTrue(dir.exists(m_testStub.DcmDirPath));
			Assert::IsTrue(dir.exists(m_testStub.ImageDirPath));
			Assert::IsTrue(dir.exists(m_testStub.PDFDirPath));
			Assert::IsTrue(dir.exists(m_testStub.DownloadDirPath));

			QFile file;
			for (auto& path : m_testStub.ImageFilePathList_Sample_CCC)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}

			for (auto& path : m_testStub.ImageFilePathList_Sample_BBB)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}

			for (auto& path : m_testStub.ImageFilePathList_Sample_AAA)
			{
				Assert::IsTrue(file.exists(path), (std::wstring(L"file is not exist : ") + path.toStdWString()).data());
			}
		}

		TEST_METHOD(TestEcho_InvalidServer)
		{
			/* 대기시간 1초 정도 필요 */
			DicomNetworkSCU SCU_Invalid;
			SetupSCU(&SCU_Invalid,
				"invalid-AE-Title", "localhost", 99999);

			Assert::AreNotEqual((int)EDicomNetworkResult::SUCCESS, (int)SCU_Invalid.Echo());
		}

		TEST_METHOD(WhenServerNotRunning_QueryFailed)
		{
			Assert::AreNotEqual((int)EDicomNetworkResult::SUCCESS, (int)m_pQueryRetrieveSCU->Echo());
		}

		TEST_METHOD(TestEcho)
		{
			RunPACSServer();
			Assert::AreEqual((int)EDicomNetworkResult::SUCCESS, (int)m_pQueryRetrieveSCU->Echo());
		}

		TEST_METHOD(TestStore_Single_DICOM_File)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0]
				});

			FindAll();
			Assert::AreEqual(1, (int)m_FIND_DicomHeaders.size());
		}

		TEST_METHOD(TestStore_Single_DICOM_Dataset)
		{
			RunPACSServer();

			std::unique_ptr<fm::DicomDataset> pDataset = fm::DicomDatasetFactory::CreateStoreDataset();
			pDataset->SetTagValue(fm::DicomTagID::PatientID, L"아이디");
			pDataset->SetTagValue(fm::DicomTagID::PatientName, L"이름");

			StoreDatasets({ *pDataset });

			FindAll();

			Assert::AreEqual(1, (int)m_FIND_DicomHeaders.size());
			Assert::AreEqual(L"아이디", m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::PatientID).data());
			Assert::AreEqual(L"이름", m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::PatientName).data());
		}

		TEST_METHOD(TestStore_Multi_DICOM_Dataset)
		{
			RunPACSServer();

			std::unique_ptr<fm::DicomDataset> pDataset1 = fm::DicomDatasetFactory::CreateStoreDataset();
			std::unique_ptr<fm::DicomDataset> pDataset2 = fm::DicomDatasetFactory::CreateStoreDataset();
			std::unique_ptr<fm::DicomDataset> pDataset3 = fm::DicomDatasetFactory::CreateStoreDataset();

			pDataset1->SetTagValue(fm::DicomTagID::PatientID, L"id1");
			pDataset1->SetTagValue(fm::DicomTagID::PatientName, L"name1");
			pDataset2->SetTagValue(fm::DicomTagID::PatientID, L"id2");
			pDataset2->SetTagValue(fm::DicomTagID::PatientName, L"name2");
			pDataset3->SetTagValue(fm::DicomTagID::PatientID, L"id3");
			pDataset3->SetTagValue(fm::DicomTagID::PatientName, L"name3");

			StoreDatasets({
				*pDataset1,
				*pDataset2,
				*pDataset3,
				});

			FindAll();

			Assert::AreEqual(3, (int)m_FIND_DicomHeaders.size());
			Assert::AreEqual(L"id1", m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::PatientID).data());
			Assert::AreEqual(L"name1", m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::PatientName).data());
			Assert::AreEqual(L"id2", m_FIND_DicomHeaders[1].GetValueWString(fm::DicomTagID::PatientID).data());
			Assert::AreEqual(L"name2", m_FIND_DicomHeaders[1].GetValueWString(fm::DicomTagID::PatientName).data());
			Assert::AreEqual(L"id3", m_FIND_DicomHeaders[2].GetValueWString(fm::DicomTagID::PatientID).data());
			Assert::AreEqual(L"name3", m_FIND_DicomHeaders[2].GetValueWString(fm::DicomTagID::PatientName).data());
		}

		TEST_METHOD(TestStore_WithLoadedDataset)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			FindAll();

			Assert::AreEqual(3, (int)m_FIND_DicomHeaders.size());
			Assert::AreEqual(L"CCC", m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::PatientName).data());
			Assert::AreEqual(L"BBB", m_FIND_DicomHeaders[1].GetValueWString(fm::DicomTagID::PatientName).data());
			Assert::AreEqual(L"AAA", m_FIND_DicomHeaders[2].GetValueWString(fm::DicomTagID::PatientName).data());
		}

		TEST_METHOD(TestStore_Single_DICOM_File_FindByName)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0]
				});

			std::vector<DicomDataset> FIND_1;
			std::vector<DicomDataset> FIND_2;
			std::vector<DicomDataset> FIND_3;
			FIND_1 = SCU_FindByName("CCC");
			FIND_2 = SCU_FindByName("AAA");
			FIND_3 = SCU_FindByName("BBB");

			Assert::AreEqual(1, (int)FIND_1.size());
			Assert::AreEqual(0, (int)FIND_2.size());
			Assert::AreEqual(0, (int)FIND_3.size());
		}

		TEST_METHOD(TestStore_Multi_DifferentStudyUID_DICOM)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			FindAll();
			Assert::AreEqual(3, (int)m_FIND_DicomHeaders.size());
		}

		TEST_METHOD(TestStore_Multi_SameStudyUID_DICOM)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_CCC[1],
				m_testStub.ImageFilePathList_Sample_CCC[2]
				});

			m_pStoreSCU->Store(m_datasetList);

			FindAll();
			Assert::AreEqual(1, (int)m_FIND_DicomHeaders.size());
		}

		TEST_METHOD(WhenEmptyStorage_GETReturnEmptyList)
		{
			RunPACSServer();

			std::string someStudyUID = "123.456";
			int maxCount = 1;
			m_pQueryRetrieveSCU->GetBySeriesUID(
				StringUtil::MultiByteToWide(someStudyUID),
				m_testStub.DownloadDirPath.toStdWString(), m_dcmFileExtension, maxCount);
			Assert::AreEqual(0, (int)m_Download_FilePaths.size());

		}

		TEST_METHOD(GET_OneFile)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				});

			m_pStoreSCU->Store(m_datasetList);

			FindAll();

			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);
			int maxCount = 1;
			//Assert::AreEqual(
			//	(int)EDicomNetworkResult::SUCCESS,
			//	(int)m_pQueryRetrieveSCU->GetBySeriesUID(
			//		seriesUID,
			//		m_testStub.DownloadDirPath.toStdWString(),
			//		m_dcmFileExtension,
			//		maxCount));
			Assert::AreEqual(1, (int)m_Download_FilePaths.size());
			AssertFilePathListExist(m_Download_FilePaths);
		}

		TEST_METHOD(GET_ThreeDifferentStudyFile)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			FindAll();

			std::wstring seriesUID = m_FIND_DicomHeaders[1].GetValueWString(fm::DicomTagID::SeriesInstanceUID);
			int maxCount = 1;
			Assert::AreEqual(
				(int)EDicomNetworkResult::SUCCESS,
				(int)m_pQueryRetrieveSCU->GetBySeriesUID(
					seriesUID,
					m_testStub.DownloadDirPath.toStdWString(),
					m_dcmFileExtension,
					maxCount));
			Assert::AreEqual(1, (int)m_Download_FilePaths.size());
			AssertFilePathListExist(m_Download_FilePaths);
		}

		TEST_METHOD(GET_ThreeSameStudyFile)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_CCC[1],
				m_testStub.ImageFilePathList_Sample_CCC[2]
				});

			FindAll();

			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);
			int maxCount = 1;
			Assert::AreEqual(
				(int)EDicomNetworkResult::SUCCESS,
				(int)m_pQueryRetrieveSCU->GetBySeriesUID(
					seriesUID,
					m_testStub.DownloadDirPath.toStdWString(),
					m_dcmFileExtension,
					maxCount));
			Assert::AreEqual(3, (int)m_Download_FilePaths.size());
			AssertFilePathListExist(m_Download_FilePaths);
		}

		TEST_METHOD(MOVE_Failed_WhenStoreSCPNotRunning)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				});

			FindAll();
			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);

			Assert::AreNotEqual(
				(int)EDicomNetworkResult::SUCCESS,
				(int)m_pQueryRetrieveSCU->MoveBySeriesUID(seriesUID, m_pStoreSCP, m_maxImageCount, nullptr));
		}

		/* 해당 테스트가 실패하는 이유는 다른 SCP가 시작되었기 때문 */
		TEST_METHOD(StoreSCPShouldRun_Before_MoveTest)
		{
			//Assert::IsTrue(m_pStoreSCP->InitAndStartServer_Async(), L"SCP running failed");

			//m_pStoreSCP->Abort_Server()->wait();
		}

		TEST_METHOD(MOVE_Success_WhenStoreSCPRunning)
		{
			RunPACSServer();
			RunStoreSCPServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				});

			FindAll();
			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);

			Assert::AreEqual(
				(int)EDicomNetworkResult::SUCCESS,
				(int)m_pQueryRetrieveSCU->MoveBySeriesUID(seriesUID, m_pStoreSCP, m_maxImageCount));

			m_pStoreSCP->Abort_Server()->wait();
		}

		TEST_METHOD(MOVE_OneFile)
		{
			RunPACSServer();
			RunStoreSCPServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				});

			FindAll();
			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);

			m_pQueryRetrieveSCU->MoveBySeriesUID(seriesUID, m_pStoreSCP,m_maxImageCount);

			m_pStoreSCP->Abort_Server()->wait();

			AssertDownloadFileListCount(1);
			AssertFilePathListExist(m_Download_FilePaths);
		}

		TEST_METHOD(MOVE_SameThreeFiles)
		{
			RunPACSServer();
			RunStoreSCPServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_CCC[1],
				m_testStub.ImageFilePathList_Sample_CCC[2]
				});

			FindAll();
			std::wstring seriesUID = m_FIND_DicomHeaders[0].GetValueWString(fm::DicomTagID::SeriesInstanceUID);

			m_pQueryRetrieveSCU->MoveBySeriesUID(seriesUID, m_pStoreSCP, m_maxImageCount);

			m_pStoreSCP->Abort_Server()->wait();

			Assert::AreEqual(3, (int)m_Download_FilePaths.size());
			AssertFilePathListExist(m_Download_FilePaths);
		}

		TEST_METHOD(MOVE_DifferentThreeFiles)
		{
			RunPACSServer();
			RunStoreSCPServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			FindAll();
			std::wstring seriesUID = m_FIND_DicomHeaders[1].GetValueWString(fm::DicomTagID::SeriesInstanceUID);

			//m_pQueryRetrieveSCU->MoveBySeriesUID(seriesUID, m_pStoreSCP, m_maxImageCount);

			m_pStoreSCP->Abort_Server()->wait();

			Assert::AreEqual(1, (int)m_Download_FilePaths.size());
			AssertFilePathListExist(m_Download_FilePaths);
		}


		TEST_METHOD(TestFindAll_And_HandleSignal)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			FindAll();

			Assert::AreEqual(3, (int)m_pEventHandler->StatusList.size());
			Assert::AreEqual(3, m_pEventHandler->StatusList[2].InProgressCount);
		}

		TEST_METHOD(TestFindSpecific_And_HandleSignal)
		{
			RunPACSServer();

			StoreDatasetsFromFilePathList({
				m_testStub.ImageFilePathList_Sample_CCC[0],
				m_testStub.ImageFilePathList_Sample_BBB[0],
				m_testStub.ImageFilePathList_Sample_AAA[0]
				});

			fm::DicomDataset dataset;
			std::wstring name;
			dataset.LoadFromFile(m_testStub.ImageFilePathList_Sample_AAA[0]);
			dataset.TryGetValueText(fm::DicomTagID::PatientName, &name);

			fm::DicomFindOption findOption(fm::EDcmFindMode::PATIENT_NAME, name, fm::EDcmModality::ALL_MODALITY, fm::EQueryRetrieveLevel::QR_LEVEL_SERIES);

			m_pQueryRetrieveSCU->Find(findOption);

			Assert::AreEqual(1, (int)m_pEventHandler->StatusList.size());
			Assert::AreEqual(1, m_pEventHandler->StatusList[0].InProgressCount);
		}

		TEST_METHOD(TestStorePDF)
		{
			RunPACSServer();

			std::wstring updateDcmToPdfFilePath = L"test_dcm_to_pdf.dcm";

			Assert::IsTrue(DicomConverter::ConvertPDFFile_To_DcmFile(
				m_testStub.Source_PDF_To_Dcm_PDFFilePath.toStdWString(),
				updateDcmToPdfFilePath));

			fm::DicomDataset dataset;
			dataset.LoadFromFile(QString::fromStdWString(updateDcmToPdfFilePath));

			StoreDatasets({ dataset });
			FindAll();

			Assert::AreEqual(1, (int)m_FIND_DicomHeaders.size());
		}

		TEST_METHOD(TestFind_OutDateRange)
		{
			RunPACSServer();

			auto pDataset = fm::DicomDatasetFactory::CreateStoreDataset();

			/* Study Date만 확인 가능 */
			pDataset->SetTagValue(fm::DicomTagID::StudyDate, L"20140101");

			StoreDatasets({ *pDataset });

			fm::DicomFindOption findOption;
			findOption.SetDateRange(DateTime(2015, 1, 1), DateTime(2020, 12, 31));
			m_pQueryRetrieveSCU->Find(findOption, &m_FIND_DicomHeaders);

			Assert::AreEqual(0, (int)m_FIND_DicomHeaders.size());
		}

		TEST_METHOD(TestFind_InDateRange)
		{
			RunPACSServer();

			std::unique_ptr<fm::DicomDataset> pDataset1 = fm::DicomDatasetFactory::CreateStoreDataset();
			std::unique_ptr<fm::DicomDataset> pDataset2 = fm::DicomDatasetFactory::CreateStoreDataset();
			std::unique_ptr<fm::DicomDataset> pDataset3 = fm::DicomDatasetFactory::CreateStoreDataset();

			/* Patient ID가 없을 경우 1개로 취급된다. */
			pDataset1->SetTagValue(fm::DicomTagID::PatientID, L"id1");
			pDataset1->SetTagValue(fm::DicomTagID::StudyDate, L"20160101");

			pDataset2->SetTagValue(fm::DicomTagID::PatientID, L"id2");
			pDataset2->SetTagValue(fm::DicomTagID::StudyDate, L"20170101");

			pDataset3->SetTagValue(fm::DicomTagID::PatientID, L"id3");
			pDataset3->SetTagValue(fm::DicomTagID::StudyDate, L"20180101");

			StoreDatasets({
				*pDataset1,
				*pDataset2,
				*pDataset3,
				});

			fm::DicomFindOption findOption;
			findOption.SetDateRange(DateTime(2016, 1, 1), DateTime(2017, 12, 31));

			m_pQueryRetrieveSCU->Find(findOption, &m_FIND_DicomHeaders);
			Assert::AreEqual(2, (int)m_FIND_DicomHeaders.size());
		}

	private:
		void RunPACSServer()
		{
			Assert::IsTrue(m_pPACSServer->RunAndWaitProcess(0));
		}

		void RunStoreSCPServer()
		{
			m_pStoreSCP->InitializeNetworkAndStartServer_Async();
		}

		void AssertFilePathListExist(std::vector<std::wstring> filePathList)
		{
			for (auto& filepath : filePathList)
			{
				QDir dir;
				dir.exists(QString::fromStdWString(filepath));
			}
		}

		void AssertDownloadFileListCount(int count)
		{
			QDir dir(m_testStub.DownloadDirPath);
			QStringList fileList = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
			Assert::AreEqual(count, fileList.size());
		}

		void SetupQueryRetrieveSCU()
		{
			SetupSCU(
				m_pQueryRetrieveSCU,
				m_pPACSServer->AETitle,
				m_pPACSServer->IP,
				m_pPACSServer->QueryRetrievePort
			);
		}

		void SetupStoreSCU()
		{
			SetupSCU(
				m_pStoreSCU,
				m_pPACSServer->AETitle,
				m_pPACSServer->IP,
				m_pPACSServer->StorePort
			);
		}

		void SetupStoreSCP()
		{
			std::wstring AETitle_storeSCP = m_pPACSServer->StoreSCP_AETitle.toStdWString();
			int storeSCP_Port = m_pPACSServer->StoreSCP_Port;
			DicomListenerInfo listenerInfo;
			listenerInfo.Init(AETitle_storeSCP, storeSCP_Port, 30);

			m_pStoreSCP->SetDownloadInfo(m_testStub.DownloadDirPath.toStdWString(), L".dcm");
			m_pStoreSCP->SetListenerInfo(listenerInfo);
		}

		void SetupSCU(DicomNetworkSCU* pSCU, QString AETitle, QString IP, int port)
		{
			DicomHostInfo hostInfo = GetHostInfo(AETitle, IP, port);

			pSCU->SetHostInfoQueryRetrieve(hostInfo);
			pSCU->SetAETitle(L"QuerySCU");
		}

		std::vector<DicomDataset> FindAll()
		{
			fm::DicomFindOption findOption(fm::EDcmFindMode::PATIENT_NAME, L"", fm::EDcmModality::ALL_MODALITY, fm::EQueryRetrieveLevel::QR_LEVEL_SERIES);

			m_pQueryRetrieveSCU->Find(findOption, &m_FIND_DicomHeaders);
			return m_FIND_DicomHeaders;
		}

		std::vector<DicomDataset> SCU_FindByName(std::string name)
		{
			std::vector<DicomDataset> foundDicomHeaders;

			fm::DicomFindOption findOption(fm::EDcmFindMode::PATIENT_NAME, StringUtil::MultiByteToWide(name), fm::EDcmModality::ALL_MODALITY, fm::EQueryRetrieveLevel::QR_LEVEL_SERIES);

			m_pQueryRetrieveSCU->Find(findOption, &foundDicomHeaders);

			return foundDicomHeaders;
		}

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

		void AddToDatasetListFromFilePath(QString filepath)
		{
			DicomDataset dcmDataset;
			dcmDataset.LoadFromFile(filepath);
			m_datasetList.push_back(dcmDataset);
		}

		void StoreDatasets(std::vector<fm::DicomDataset> datasets)
		{
			Assert::AreEqual((int)EDicomNetworkResult::SUCCESS, (int)m_pStoreSCU->Store(datasets));
		}

		void StoreDatasetsFromFilePathList(std::vector<QString> filepathList)
		{
			for (QString& path : filepathList)
			{
				AddToDatasetListFromFilePath(path);
			}

			Assert::AreEqual((int)EDicomNetworkResult::SUCCESS, (int)m_pStoreSCU->Store(m_datasetList));
		}

	private:
	};
}