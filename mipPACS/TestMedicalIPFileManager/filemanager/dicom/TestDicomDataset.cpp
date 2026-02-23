#include "stdafx.h"
#include "CppUnitTest.h"
#include "filemanager/dicom/DicomDatasetFactory.h"
#include "filemanager/dicom/DicomDataset.h"
#include "DicomTestStub.h"
#include <qstring>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace fm;

namespace TestMedicalIPFileManager
{
	TEST_CLASS(TestDicomDataset)
	{
	private:
		DicomDataset* m_pDataset;
		DicomTestStub m_testStub;

	public:
		TEST_METHOD_INITIALIZE(SetUp)
		{
			m_pDataset = new DicomDataset();
		}

		TEST_METHOD_CLEANUP(TearDown)
		{
			delete m_pDataset;
		}


		TEST_METHOD(TestEmptyDataset_GetValueReturnFalse)
		{
			std::wstring getValue;
			Assert::IsFalse(m_pDataset->TryGetValueText(DicomTagID::PatientID, &getValue));
		}

		TEST_METHOD(TestTagExist)
		{
			Assert::IsFalse(m_pDataset->IsTagExists(DicomTagID::PatientID));
			m_pDataset->SetTagValue(DicomTagID::PatientID, L"");
			Assert::IsTrue(m_pDataset->IsTagExists(DicomTagID::PatientID));
		}

		TEST_METHOD(TestAddTagOne_And_GetValue)
		{
			std::wstring  getValue;
			m_pDataset->SetTagValue(DicomTagID::PatientID, L"id");

			Assert::IsTrue(m_pDataset->TryGetValueText(DicomTagID::PatientID, &getValue));
			Assert::AreEqual(L"id", getValue.data());
		}

		TEST_METHOD(TestAddTagOne_With_Hangul)
		{
			std::wstring  getValue;
			m_pDataset->SetTagValue(DicomTagID::PatientID, L"이상일");

			Assert::IsTrue(m_pDataset->TryGetValueText(DicomTagID::PatientID, &getValue));
			Assert::AreEqual(L"이상일", getValue.data());
		}

		TEST_METHOD(TestThreeAddTag_And_SetGet)
		{
			m_pDataset->SetTagValue(DicomTagID::PatientName, L"내이름");
			m_pDataset->SetTagValue(DicomTagID::PatientBirthDate, L"91.06.11");
			m_pDataset->SetTagValue(DicomTagID::StudyInstanceUID, L"111.222");

			std::wstring getValueName;
			std::wstring getValueBirthDate;
			std::wstring getValueStudyInstanceUID;

			Assert::IsTrue(m_pDataset->TryGetValueText(DicomTagID::PatientName, &getValueName));
			Assert::IsTrue(m_pDataset->TryGetValueText(DicomTagID::PatientBirthDate, &getValueBirthDate));
			Assert::IsTrue(m_pDataset->TryGetValueText(DicomTagID::StudyInstanceUID, &getValueStudyInstanceUID));
			Assert::AreEqual(L"내이름", getValueName.data());
			Assert::AreEqual(L"91.06.11", getValueBirthDate.data());
			Assert::AreEqual(L"111.222", getValueStudyInstanceUID.data());
		}

		TEST_METHOD(TestFindQueryDataset)
		{
			std::unique_ptr<DicomDataset> pFindQueryDataset;
			std::vector<DicomTagID> findQueryTagIdList;
			std::vector<DicomTagValuePair> findTagValues;

			findQueryTagIdList.push_back(DicomTagID::PatientName);
			findQueryTagIdList.push_back(DicomTagID::PatientID);
			findQueryTagIdList.push_back(DicomTagID::Modality);
			findQueryTagIdList.push_back(DicomTagID::StudyDescription);
			findQueryTagIdList.push_back(DicomTagID::AccessionNumber);

			findTagValues.push_back(DicomTagValuePair(DicomTagID::PatientName, L"한글이름"));
			findTagValues.push_back(DicomTagValuePair(DicomTagID::PatientID, L"id"));
			findTagValues.push_back(DicomTagValuePair(DicomTagID::Modality, L"modality"));
			findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyDescription, L"study_description"));
			findTagValues.push_back(DicomTagValuePair(DicomTagID::AccessionNumber, L"accession_number"));

			pFindQueryDataset = DicomDatasetFactory::CreateFindQueryDataset(fm::EQueryRetrieveLevel::QR_LEVEL_STUDY, findQueryTagIdList, findTagValues);

			std::wstring getValueStudyModel;

			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::QueryRetrieveLevel, &getValueStudyModel));
			Assert::AreEqual("study",QString::fromStdWString(getValueStudyModel).toLower().toLocal8Bit().data());

			std::wstring getValuePatientName;
			std::wstring getValuePatientID;
			std::wstring getValueModiality;
			std::wstring getValueStudyDescription;
			std::wstring getValueAccessionNumber;

			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientName, &getValuePatientName));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientID, &getValuePatientID));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::Modality, &getValueModiality));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::StudyDescription, &getValueStudyDescription));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::AccessionNumber, &getValueAccessionNumber));

			Assert::AreEqual(L"한글이름", getValuePatientName.data());
			Assert::AreEqual(L"id", getValuePatientID.data());
			Assert::AreEqual(L"modality", getValueModiality.data());
			Assert::AreEqual(L"study_description", getValueStudyDescription.data());
			Assert::AreEqual(L"accession_number", getValueAccessionNumber.data());

			/* 해당 Tag가 존재하는지 테스트 */
			std::wstring getValue;
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::QueryRetrieveLevel, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::StudyDate, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::StudyTime, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::StudyInstanceUID, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::SeriesInstanceUID, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientName, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientBirthDate, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientBirthTime, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PatientID, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::Modality, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::ModalitiesInStudy, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::StudyDescription, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::SeriesDescription, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::AccessionNumber, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::ReferringPhysicianName, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::PerformingPhysicianName, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::InstitutionName, &getValue));
			Assert::IsTrue(pFindQueryDataset->TryGetValueText(DicomTagID::NumberOfStudyRelatedInstances, &getValue));
		}

		TEST_METHOD(TestLoadFromFile)
		{
			AssertLoadFileCompareToPatientName(m_testStub.ImageFilePathList_Sample_CCC[0], L"CCC");
			AssertLoadFileCompareToPatientName(m_testStub.ImageFilePathList_Sample_BBB[0], L"BBB");
			AssertLoadFileCompareToPatientName(m_testStub.ImageFilePathList_Sample_AAA[0], L"AAA");
		}

		TEST_METHOD(TestDeepCopy_Operator)
		{
			DicomDataset* dataset1 = new DicomDataset();
			dataset1->SetTagValue(DicomTagID::PatientName, L"aaa");

			DicomDataset* dataset2 = new DicomDataset();

			*dataset2 = *dataset1;

			delete dataset1;

			std::wstring getValue;
			dataset2->TryGetValueText(DicomTagID::PatientName, &getValue);
			Assert::AreEqual(L"aaa", getValue.data());
			
			delete dataset2;
		}

		TEST_METHOD(TestDeepCopy_Contructor)
		{
			DicomDataset* dataset1 = new DicomDataset();
			dataset1->SetTagValue(DicomTagID::PatientID, L"bbb");

			DicomDataset* dataset2 = new DicomDataset(*dataset1);

			delete dataset1;

			std::wstring getValue;
			dataset2->TryGetValueText(DicomTagID::PatientID, &getValue);
			Assert::AreEqual(L"bbb", getValue.data());

			delete dataset2;
		}

		TEST_METHOD(TestAddTagFrom_OtherDataset)
		{
			DicomDataset dset1;

			dset1.SetTagValue(DicomTagID::PatientName, L"Name");
			dset1.SetTagValue(DicomTagID::PatientID, L"ID");
			dset1.SetTagValue(DicomTagID::PatientBirthDate, L"20080810");
			dset1.SetTagValue(DicomTagID::StudyDescription, L"Study");
			dset1.SetTagValue(DicomTagID::SeriesDescription, L"Series");

			std::vector<TagValuePair> tagValues = dset1.GetAllTagValues();
			Assert::AreEqual(5, (int)tagValues.size());
			AssertTagValueList(tagValues, DicomTagID::PatientName, L"Name");
			AssertTagValueList(tagValues, DicomTagID::PatientID, L"ID");
			AssertTagValueList(tagValues, DicomTagID::PatientBirthDate, L"20080810");
			AssertTagValueList(tagValues, DicomTagID::StudyDescription, L"Study");
			AssertTagValueList(tagValues, DicomTagID::SeriesDescription, L"Series");
		}

		TEST_METHOD(TestAppendDataset)
		{
			DicomDataset dset1;

			dset1.SetTagValue(DicomTagID::PatientName, L"Name1");
			dset1.SetTagValue(DicomTagID::PatientID, L"ID1");

			DicomDataset dset2;
			dset2.SetTagValue(DicomTagID::PatientID, L"ID2");
			dset2.SetTagValue(DicomTagID::SeriesDescription, L"Series");

			dset1.AppendDataset(&dset2);

			AssertDatasetHasTagValue(dset1, DicomTagID::PatientName, L"Name1");
			AssertDatasetHasTagValue(dset1, DicomTagID::PatientID, L"ID2");
			AssertDatasetHasTagValue(dset1, DicomTagID::SeriesDescription, L"Series");
		}

		TEST_METHOD(TestConverToDicomHeader)
		{
			DicomDataset dset1;
			dset1.SetTagValue(DicomTagID::StudyDate, L"19990101");
			dset1.SetTagValue(DicomTagID::StudyTime, L"222222");
			dset1.SetTagValue(DicomTagID::SeriesDate, L"19931230");
			dset1.SetTagValue(DicomTagID::SeriesTime, L"111111");
			dset1.SetTagValue(DicomTagID::PatientName, L"이름");
			dset1.SetTagValue(DicomTagID::PatientBirthDate, L"20000101");
			dset1.SetTagValue(DicomTagID::PatientID, L"아이디");
			dset1.SetTagValue(DicomTagID::Modality, L"CT");
			dset1.SetTagValue(DicomTagID::ModalitiesInStudy, L"MR");
			dset1.SetTagValue(DicomTagID::StudyDescription, L"study description");
			dset1.SetTagValue(DicomTagID::SeriesDescription, L"series description");
			dset1.SetTagValue(DicomTagID::AccessionNumber, L"111");
			dset1.SetTagValue(DicomTagID::ReferringPhysicianName, L"referring physician name");
			dset1.SetTagValue(DicomTagID::PerformingPhysicianName, L"performing physician name");
			dset1.SetTagValue(DicomTagID::NumberOfStudyRelatedInstances, L"100");
			dset1.SetTagValue(DicomTagID::NumberOfSeriesRelatedInstances, L"101");

			DICOM_HEADER_INFO dcmHeaderInfo = dset1.ToDcmHeaderInfo();
			Assert::AreEqual("19990101222222", dcmHeaderInfo.StudyDateTime.ToYYYYMMDDhhmmss().c_str());
			Assert::AreEqual("19931230111111", dcmHeaderInfo.SeriesDateTime.ToYYYYMMDDhhmmss().c_str());
			Assert::AreEqual(L"이름", dcmHeaderInfo.PatientName.c_str());
			Assert::AreEqual("20000101", dcmHeaderInfo.DateOfBirth.ToYYYYMMDD().c_str());
			Assert::AreEqual(L"아이디", dcmHeaderInfo.PatientID.c_str());
			Assert::AreEqual(L"CT", dcmHeaderInfo.Modality.c_str());
			Assert::AreEqual(L"study description", dcmHeaderInfo.StudyDescription.c_str());
			Assert::AreEqual(L"series description", dcmHeaderInfo.SeriesDescription.c_str());
			Assert::AreEqual(L"111", dcmHeaderInfo.AccessionNumber.c_str());
			Assert::AreEqual(L"referring physician name", dcmHeaderInfo.ReferringPhysician.c_str());
			Assert::AreEqual(L"performing physician name", dcmHeaderInfo.PerformingPhysician.c_str());
			Assert::AreEqual(100, dcmHeaderInfo.NumberOfStudyRelatedInstances);
			Assert::AreEqual(101, dcmHeaderInfo.NumberOfSeriesRelatedInstances);
		}

		TEST_METHOD(TestDicomType)
		{
			DicomDataset datasetPDF;
			DicomDataset datasetSecondaryCapture;
			DicomDataset datasetCT;
			Assert::IsTrue(datasetPDF.LoadFromFile(m_testStub.Source_Dcm_To_PDF_DcmFilePath));
			Assert::AreEqual((int)DicomDataset::EType::EncapsulatedPDFStorage, (int)datasetPDF.GetDicomType());

			Assert::IsTrue(datasetSecondaryCapture.LoadFromFile(m_testStub.Source_DCM_To_Img_DcmFilePath));
			Assert::AreEqual((int)DicomDataset::EType::Image_8bit, (int)datasetSecondaryCapture.GetDicomType());

			Assert::IsTrue(datasetCT.LoadFromFile(m_testStub.ImageFilePathList_Sample_AAA[0]));
			Assert::AreEqual((int)DicomDataset::EType::Image_16bit, (int)datasetCT.GetDicomType());
		}

	private:
		void AssertLoadFileCompareToPatientName(QString filepath, std::wstring patientName)
		{
			DicomDataset dataset;
			std::wstring getPatientName;

			Assert::IsTrue(dataset.LoadFromFile(filepath));
			Assert::IsTrue(dataset.TryGetValueText(DicomTagID::PatientName, &getPatientName));
			Assert::AreEqual(patientName, getPatientName);
		}

		void AssertDatasetHasTagValue(DicomDataset& dataset, DicomTagID dcmTagID, std::wstring value)
		{
			std::wstring outValue;
			Assert::IsTrue(dataset.TryGetValueText(dcmTagID, &outValue));
			Assert::AreEqual(value, outValue);
		}

		void AssertTagValueList(std::vector<TagValuePair> tagValues, DicomTagID dcmTagID, std::wstring value)
		{
			for (auto& tagValue : tagValues)
			{
				if (tagValue.first == dcmTagID)
				{
					Assert::AreEqual(value, tagValue.second);
					return;
				}
			}
			Assert::IsFalse(true);
		}

	private:
	};
}