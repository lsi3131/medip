#include "stdafx.h"
#include "dicom_defines.h"
#include "Network/DicomNetworkSCP.h"
#include "Network/DicomNetworkSCU.h"
#include "Network/DicomNetworkSCPStatus.h"
#include "Network/DicomNetworkStoreSCPStatus.h"
#include "DicomInfomationModel.h"
#include <qmetatype>

//=============================================
//		Enum Table Definitions
//=============================================
namespace fm
{
	struct RegisterDicom
	{
		RegisterDicom()
		{
			qRegisterMetaType<DicomNetworkSCPStatus>("DicomNetworkSCPStatus");
			qRegisterMetaType<DicomNetworkSCUStatus>("DicomNetworkSCUStatus");
			qRegisterMetaType<DicomNetworkStoreSCPStatus>("DicomNetworkStoreSCPStatus");
			qRegisterMetaType<DicomDataset>("DicomDataset");
		}
	};
	static RegisterDicom __registerDicom__;

	const std::pair<int, std::string> QUERY_MODEAL_TABLE[(int)EQueryModel::MAXCOUNT] =
	{
		{ (int)EQueryModel::PatientRoot, "PatientRoot" },
		{ (int)EQueryModel::StudyRoot, "StudyRoot" },
		{ (int)EQueryModel::PatientStudyOnly, "PatientStudyOnly" },
	};

	const std::pair<int, std::string> DCM_NETWORK_DIMSE_TYPE[(int)EDcmNetworkDIMSEType::MAXCOUNT] =
	{
		{ (int)EDcmNetworkDIMSEType::ECHO_SCU, "ECHO SCU" },
		{ (int)EDcmNetworkDIMSEType::STORE_SCU, "STORE SCU" },
		{ (int)EDcmNetworkDIMSEType::FIND_SCU, "FIND SCU" },
		{ (int)EDcmNetworkDIMSEType::MOVE_SCU, "MOVE SCU" },
		{ (int)EDcmNetworkDIMSEType::GET_SCU, "GET SCU" },
	};

	const std::pair<int, std::string> DCM_PROTOCOL_TABLE[(int)EDcmProtocol::MAXCOUNT] =
	{
		{ (int)EDcmProtocol::C_MOVE, "C-MOVE" },
		{ (int)EDcmProtocol::C_GET, "C-GET" },
	};

	const std::pair<int, std::string> DCM_FIND_MODE_TABLE[(int)EDcmFindMode::MAXCOUNT] =
	{
		{ (int)EDcmFindMode::PATIENT_NAME, "PATIENT_NAME" },
		{ (int)EDcmFindMode::PATIENT_ID, "PATIENT_ID" },
		{ (int)EDcmFindMode::PATIENT_SEX, "PATIENT_SEX" },
		{ (int)EDcmFindMode::ACCESSION_NUMBER, "ACCESSION_NUMBER" },
		{ (int)EDcmFindMode::EXAM_ID, "EXAM_ID" },
		{ (int)EDcmFindMode::STUDY_DESCRIPTION, "STUDY_DESCRIPTION" },
		{ (int)EDcmFindMode::REFERRING_PHYSICIAN, "REFERRING_PHYSICIAN" },
		{ (int)EDcmFindMode::PERFORMING_PHYSICIAN, "PERFORMING_PHYSICIAN" },
		{ (int)EDcmFindMode::READING_PHYSICIAN, "READING_PHYSICIAN" },
		{ (int)EDcmFindMode::INSTITUTION_NAME, "INSTITUTION_NAME" },
	};

	const std::pair<int, std::string> DCM_MODALITY_MODE_TABLE[(int)EDcmModality::MAXCOUNT] =
	{
		{ (int)EDcmModality::ALL_MODALITY, "All Modality" },
		{ (int)EDcmModality::CR, "CR" },
		{ (int)EDcmModality::DX, "DX" },
		{ (int)EDcmModality::CT, "CT" },
		{ (int)EDcmModality::MR, "MR" },
		{ (int)EDcmModality::XA, "XA" },
		{ (int)EDcmModality::MG, "MG" },
		{ (int)EDcmModality::US, "US" },
		{ (int)EDcmModality::PT, "PT" },
		{ (int)EDcmModality::NM, "NM" },
		{ (int)EDcmModality::RF, "RF" },
		{ (int)EDcmModality::RG, "RG" },
		{ (int)EDcmModality::PX, "PX" },
		{ (int)EDcmModality::ES, "ES" },
		{ (int)EDcmModality::XC, "XC" },
		{ (int)EDcmModality::GM, "GM" },
		{ (int)EDcmModality::SC, "SC" },
		{ (int)EDcmModality::OT, "OT" },
		{ (int)EDcmModality::SR, "SR" },
	};

}
