#include "stdafx.h"
#include "FindSCUCommand.h"
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include "filemanager/dicom/Network/DIMSE_Data.h"
#include "filemanager/dicom/DicomDatasetFactory.h"
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"


namespace fm
{
	static int callBackCount = 0;

	static DicomTagID FIND_QUERY_TAG_ID_LIST[] =
	{
		DicomTagID::QueryRetrieveLevel,
		DicomTagID::SOPClassUID,
		DicomTagID::SOPInstanceUID,
		DicomTagID::MediaStorageSOPClassUID,
		DicomTagID::MediaStorageSOPInstanceUID,
		DicomTagID::StudyDate,
		DicomTagID::StudyTime,
		DicomTagID::StudyInstanceUID,
		DicomTagID::SeriesInstanceUID,
		DicomTagID::StudyID,
		DicomTagID::PatientName,
		DicomTagID::PatientBirthDate,
		DicomTagID::PatientBirthTime,
		DicomTagID::PatientID,
		DicomTagID::PatientSex,
		DicomTagID::PatientAge,
		DicomTagID::PatientID,
		DicomTagID::Modality,
		DicomTagID::ModalitiesInStudy,
		DicomTagID::StudyDescription,
		DicomTagID::SeriesDescription,
		DicomTagID::AccessionNumber,
		DicomTagID::ReferringPhysicianName,
		DicomTagID::PerformingPhysicianName,
		DicomTagID::RequestingPhysician,
		DicomTagID::InstitutionName,
		DicomTagID::NumberOfPatientRelatedStudies,
		DicomTagID::NumberOfPatientRelatedSeries,
		DicomTagID::NumberOfPatientRelatedInstances,
		DicomTagID::NumberOfStudyRelatedSeries,
		DicomTagID::NumberOfStudyRelatedInstances,
		DicomTagID::NumberOfSeriesRelatedInstances,
		DicomTagID::AcquisitionDate,
		DicomTagID::SeriesDate,
		DicomTagID::SeriesTime,
		DicomTagID::Manufacturer,
		DicomTagID::ManufacturerModelName,
		DicomTagID::InstanceNumber,
		DicomTagID::SeriesNumber,
		DicomTagID::ImagesInAcquisition,
		DicomTagID::RETIRED_AcquisitionsInSeries,
		DicomTagID::RETIRED_ImagesInSeries,
		DicomTagID::RETIRED_AcquisitionsInStudy,
		DicomTagID::RETIRED_ImagesInStudy,
	};

	static void FindRetrieveCallback(
		void* pCallbackData,
		T_DIMSE_C_FindRQ* request,
		int responseCount,
		T_DIMSE_C_FindRSP* response,
		DcmDataset* pResponseDataset)
	{
		FindSCUCommand* pCmd = (FindSCUCommand*)pCallbackData;

		DicomDataset dset(pResponseDataset);

		DICOM_HEADER_INFO dcmHeaderInfo = dset.ToDcmHeaderInfo();
		qInfo() << "status : " << response->DimseStatus <<
			", message id : " << response->MessageIDBeingRespondedTo <<
			", dcm series info : " << dcmHeaderInfo.NumberOfSeriesRelatedInstances << ", response count : " << responseCount <<
			", callback count : " << callBackCount++;
		//qDebug() << "callback count : " << callBackCount++;

		pCmd->Update_FIND_InProgress(dset);
	}

	FindSCUCommand::FindSCUCommand(DicomNetworkSCU* pSCU,
		std::wstring AETitle,
		DicomHostInfo hostInfo,
		DicomFindOption findOption,
		std::vector<DicomDataset>* pDcmDatasetList) :
		SingleSendSCUCommand(pSCU, AETitle, hostInfo),
		m_findOption(findOption),
		m_progressCount(0),
		m_pOutDcmDatasetList(pDcmDatasetList)
	{
	}

	std::vector<DicomPresentationContext> FindSCUCommand::GetPresentationContexts()
	{
		EQueryModel queryModel = EQueryModel::StudyRoot;

		std::vector<DicomPresentationContext> ctx;
		//ctx.push_back(DicomPresentationContext(QuerySyntax[(int)queryModel].FindSyntax, ASC_SC_ROLE_DEFAULT));
		//ctx.push_back(DicomPresentationContext(QuerySyntax[(int)EQueryModel::PatientRoot].FindSyntax, ASC_SC_ROLE_DEFAULT));
		ctx.push_back(DicomPresentationContext(QuerySyntax[(int)EQueryModel::StudyRoot].FindSyntax, ASC_SC_ROLE_DEFAULT));
		//ctx.push_back(DicomPresentationContext(QuerySyntax[(int)EQueryModel::PatientStudyOnly].FindSyntax, ASC_SC_ROLE_DEFAULT));
		return ctx;
	}

	std::string FindSCUCommand::GetAbstractSyntax()
	{
		return GetPresentationContexts()[0].AbstractSyntaxName.c_str();
	}

	EDicomNetworkResult FindSCUCommand::SetupSendNetworkMessage(NetworkSendData* pNetSendData, DicomDataset* pDicomDataset)
	{
		SCUCommand::SetupSendNetworkMessage(pNetSendData, pDicomDataset);

		T_DIMSE_C_FindRQ* req = &(pNetSendData->Message.msg.CFindRQ);
		pNetSendData->Message.CommandField = DIMSE_C_FIND_RQ;
		req->DataSetType = DIMSE_DATASET_PRESENT;
		req->Priority = DIMSE_PRIORITY_MEDIUM;

		return EDicomNetworkResult::SUCCESS;
	}

	EDicomNetworkResult FindSCUCommand::SendAndReceiveMessage(DicomDataset* pDicomDataset)
	{
		callBackCount = 0;
		SetupQueryDataset();

		OFCondition ofcond;
		T_DIMSE_C_FindRSP rsp;
		int responseCount = 0;
		DcmDataset* statusDetail = NULL;

		ofcond = DIMSE_findUser(
			m_pNetSendData->Assoc,
			m_pNetSendData->PresentationContextID,
			&m_pNetSendData->Message.msg.CFindRQ,
			m_pSCUQueryDataset->Data(),
			responseCount,
			FindRetrieveCallback, this,
			m_pNetSendData->BlockMode,
			m_hostInfo.Timeout(),
			&rsp, &statusDetail);

		if (ofcond.bad())
		{
			qInfo() << "DIMSE condition failed. OFCondition : " << ofcond.text();
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_FIND;
		}

		if (DICOM_SUCCESS_STATUS(rsp.DimseStatus))
		{
			return EDicomNetworkResult::SUCCESS;
		}
		else
		{
			qInfo() << "DoFind response failed. DIMSE status : " << DU_cfindStatusString(rsp.DimseStatus);
			return EDicomNetworkResult::FAIL_TO_SCU_SEND_DIMSE_FIND;
		}
	}

	void FindSCUCommand::Update_FIND_InProgress(DicomDataset& dcmDataset)
	{
		DicomNetworkSCUStatus status(EDcmNetworkDIMSEType::FIND_SCU);
		status.InProgressCount = ++m_progressCount;

		if (m_pOutDcmDatasetList)
		{
			m_pOutDcmDatasetList->push_back(dcmDataset);
		}

		emit m_pSCU->updateSCUStatus(status);
	}

	void FindSCUCommand::SetupQueryDataset()
	{
		/* Find 검색어 초기화*/
		std::wstring patientName = L"";
		std::wstring patientID = L"";
		std::wstring patientSex = L"";
		std::wstring accessionNumber = L"";
		std::wstring examID = L"";
		std::wstring studyDescription = L"";
		std::wstring referringPhysician = L"";
		std::wstring performingPhysician = L"";
		std::wstring readingPhysician = L"";
		std::wstring institutionName = L"";
		std::wstring modalityText = L"";
		std::wstring dateRange = L"";

		if (!m_findOption.Text.empty())
		{
			m_findOption.Text = L"*" + m_findOption.Text + L"*";
		}

		if (m_findOption.Mode == EDcmFindMode::PATIENT_NAME)
		{
			patientName = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::PATIENT_ID)
		{
			patientID = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::PATIENT_SEX)
		{
			patientSex = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::ACCESSION_NUMBER)
		{
			accessionNumber = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::EXAM_ID)
		{
			examID = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::STUDY_DESCRIPTION)
		{
			studyDescription = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::REFERRING_PHYSICIAN)
		{
			referringPhysician = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::PERFORMING_PHYSICIAN)
		{
			performingPhysician = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::READING_PHYSICIAN)
		{
			readingPhysician = m_findOption.Text;
		}
		else if (m_findOption.Mode == EDcmFindMode::INSTITUTION_NAME)
		{
			institutionName = m_findOption.Text;
		}

		if (m_findOption.Modality == EDcmModality::ALL_MODALITY)
		{
			modalityText = L"";
		}
		else
		{
			modalityText = StringUtil::MultiByteToWide(EDcmModality_Text((int)m_findOption.Modality));
		}

		std::vector<DicomTagValuePair> findTagValues;
		findTagValues.push_back(DicomTagValuePair(DicomTagID::PatientName, patientName.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::PatientID, patientID.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::PatientSex, patientSex.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::AccessionNumber, accessionNumber.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyID, examID.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyDescription, studyDescription.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::ReferringPhysicianName, referringPhysician.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::PerformingPhysicianName, performingPhysician.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::InstitutionName, institutionName.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::Modality, modalityText.data()));
		findTagValues.push_back(DicomTagValuePair(DicomTagID::ModalitiesInStudy, modalityText.data()));

		/* StudyDate로만 비교 진행 */
		if (m_findOption.HasDateRange())
		{
			dateRange = m_findOption.GetDateRange_Text();
			findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyDate, dateRange.data()));
		}
		else
		{
			findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyDate, L""));
		}
		findTagValues.push_back(DicomTagValuePair(DicomTagID::StudyInstanceUID, m_findOption.StudyInstanceUID));

		std::vector<DicomTagID> dcmTagIDList;
		dcmTagIDList.assign(FIND_QUERY_TAG_ID_LIST, FIND_QUERY_TAG_ID_LIST + _countof(FIND_QUERY_TAG_ID_LIST));
		//dcmTagIDList.assign(DICOM_TAG_ID_TABLE_FIND, DICOM_TAG_ID_TABLE_FIND + GetTagTableCount_FindSCU());
		//dcmTagIDList.assign(DICOM_TAG_ID_TABLE_FIND, DICOM_TAG_ID_TABLE_FIND + GetTagTableCount_FindSCU());

		m_pSCUQueryDataset = DicomDatasetFactory::CreateFindQueryDataset(
			m_findOption.QueryRetrieveLevel, 
			dcmTagIDList,
			findTagValues);

	}
}
