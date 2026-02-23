#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/data/Entity/LastEditedFileDAO_Xml.h"
#include <qfile>
#include <qdir>
#include <qthread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestLastEditedFileDAO_Xml)
	{
	private:
		std::wstring m_testXmlFilePath = L"last_edit_file.xml";
		std::wstring m_defaultFilePath;
		std::vector<LastEditedFileDTO> m_getDatas;
		LastEditedFileDAO_Xml* m_pGateway;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_getDatas.clear();
			m_defaultFilePath = QDir::currentPath().toStdWString() + L"/dumi.dat";
			m_pGateway = new LastEditedFileDAO_Xml(m_testXmlFilePath);
			QFile::remove(QString::fromStdWString(m_testXmlFilePath));
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pGateway;
		}

		TEST_METHOD(WhenFileIsNotExist_ReturnEmpty)
		{
			Assert::IsTrue(m_pGateway->Initialize());
			Assert::IsFalse(m_pGateway->GetAll(m_getDatas));
			Assert::AreEqual(0, (int)m_getDatas.size());
		}

		TEST_METHOD(WhenNotInitialized_AddFail)
		{
			LastEditedFileDTO dto;
			dto.FilePath = m_defaultFilePath;
			Assert::IsFalse(m_pGateway->AddOrModify(dto));
		}

		TEST_METHOD(WhenFilePathNotExist_AddNewDTOFailed)
		{
			LastEditedFileDTO dto;
			m_pGateway->Initialize();
			Assert::IsFalse(m_pGateway->AddOrModify(dto));
		}

		TEST_METHOD(AddOneData)
		{
			LastEditedFileDTO dto;
			m_pGateway->Initialize();
			dto.FilePath = m_defaultFilePath;

			dto.SeriesInstanceUID = L"series";
			dto.StudyInstanceUID = L"study";
			dto.PatientName = L"name";
			dto.DateOfBirth = L"birth";
			dto.PatientID = L"id";
			dto.Modality = L"modality";
			dto.StudyDescription = L"study description";
			dto.SeriesDescription = L"series description";
			dto.AccessionNumber = L"accession number";
			dto.ExamID = L"exam id";
			dto.ReferringPhysician = L"referring physician";
			dto.PerformingPhysician = L"performing physician";
			dto.ReadingPhysician = L"reading physician";

			Assert::IsTrue(m_pGateway->AddOrModify(dto));
			Assert::IsTrue(m_pGateway->GetAll(m_getDatas));
			Assert::AreEqual(1, (int)m_getDatas.size());
			Assert::AreEqual(L"series", m_getDatas[0].SeriesInstanceUID.c_str());
			Assert::AreEqual(L"study", m_getDatas[0].StudyInstanceUID.c_str());
			Assert::AreEqual(L"name", m_getDatas[0].PatientName.c_str());
			Assert::AreEqual(L"birth", m_getDatas[0].DateOfBirth.c_str());
			Assert::AreEqual(L"id", m_getDatas[0].PatientID.c_str());
			Assert::AreEqual(L"modality", m_getDatas[0].Modality.c_str());
			Assert::AreEqual(L"study description", m_getDatas[0].StudyDescription.c_str());
			Assert::AreEqual(L"series description", m_getDatas[0].SeriesDescription.c_str());
			Assert::AreEqual(L"accession number", m_getDatas[0].AccessionNumber.c_str());
			Assert::AreEqual(L"exam id", m_getDatas[0].ExamID.c_str());
			Assert::AreEqual(L"referring physician", m_getDatas[0].ReferringPhysician.c_str());
			Assert::AreEqual(L"performing physician", m_getDatas[0].PerformingPhysician.c_str());
			Assert::AreEqual(L"reading physician", m_getDatas[0].ReadingPhysician.c_str());
		}

		TEST_METHOD(AddModifyFilePath)
		{
			LastEditedFileDTO dto;
			m_pGateway->Initialize();
			dto.FilePath = m_defaultFilePath;
			//=== save - 1 ====
			dto.SeriesInstanceUID = L"series";
			dto.StudyInstanceUID = L"study";
			dto.PatientName = L"name";
			dto.DateOfBirth = L"birth";
			dto.PatientID = L"id";
			dto.Modality = L"modality";
			dto.StudyDescription = L"study description";
			dto.SeriesDescription = L"series description";
			dto.AccessionNumber = L"accession number";
			dto.ExamID = L"exam id";
			dto.ReferringPhysician = L"referring physician";
			dto.PerformingPhysician = L"performing physician";
			dto.ReadingPhysician = L"reading physician";

			m_pGateway->AddOrModify(dto);
			m_pGateway->GetAll(m_getDatas);
			fm::DateTime prevDateTime = m_getDatas[0].EditTime;

			//=== save - 2 ====
			QThread::msleep(1000);		//1초 대기하여 시간 갱신
			dto.SeriesInstanceUID = L"series2";
			dto.StudyInstanceUID = L"study2";
			dto.PatientName = L"name2";
			dto.DateOfBirth = L"birth2";
			dto.PatientID = L"id2";
			dto.Modality = L"modality2";
			dto.StudyDescription = L"studydescription2";
			dto.SeriesDescription = L"seriesdescription2";
			dto.AccessionNumber = L"accessionnumber2";
			dto.ExamID = L"examid2";
			dto.ReferringPhysician = L"referringphysician2";
			dto.PerformingPhysician = L"performingphysician2";
			dto.ReadingPhysician = L"readingphysician2";		
			m_pGateway->AddOrModify(dto);
			m_pGateway->GetAll(m_getDatas);
			fm::DateTime currentDateTime = m_getDatas[0].EditTime;

			Assert::AreNotEqual(prevDateTime.ToYYYYMMDDhhmmss(), currentDateTime.ToYYYYMMDDhhmmss());

			Assert::AreEqual(1, (int)m_getDatas.size());
			Assert::AreEqual(L"series2", m_getDatas[0].SeriesInstanceUID.c_str());
			Assert::AreEqual(L"study2", m_getDatas[0].StudyInstanceUID.c_str());
			Assert::AreEqual(L"name2", m_getDatas[0].PatientName.c_str());
			Assert::AreEqual(L"birth2", m_getDatas[0].DateOfBirth.c_str());
			Assert::AreEqual(L"id2", m_getDatas[0].PatientID.c_str());
			Assert::AreEqual(L"modality2", m_getDatas[0].Modality.c_str());
			Assert::AreEqual(L"studydescription2", m_getDatas[0].StudyDescription.c_str());
			Assert::AreEqual(L"seriesdescription2", m_getDatas[0].SeriesDescription.c_str());
			Assert::AreEqual(L"accessionnumber2", m_getDatas[0].AccessionNumber.c_str());
			Assert::AreEqual(L"examid2", m_getDatas[0].ExamID.c_str());
			Assert::AreEqual(L"referringphysician2", m_getDatas[0].ReferringPhysician.c_str());
			Assert::AreEqual(L"performingphysician2", m_getDatas[0].PerformingPhysician.c_str());
			Assert::AreEqual(L"readingphysician2", m_getDatas[0].ReadingPhysician.c_str());
		}

	private:
	};
}