#pragma once

#include "filemanager/std/DateTime.h"
#include "filemanager/std/defines.h"
#include "filemanager/export.h"
#include <string>
#include <utility>

namespace fm
{
	enum class EQueryModel {
		PatientRoot,
		StudyRoot,
		PatientStudyOnly,
		MAXCOUNT
	};
	DECLARE_ENUM_TEXT(EQueryModel, QUERY_MODEAL_TABLE, (int)EQueryModel::MAXCOUNT);

	enum class EDcmNetworkDIMSEType
	{
		ECHO_SCU,
		STORE_SCU,
		FIND_SCU,
		MOVE_SCU,
		GET_SCU,
		MAXCOUNT
	};
	DECLARE_ENUM_TEXT(EDcmNetworkDIMSEType, DCM_NETWORK_DIMSE_TYPE, (int)EDcmNetworkDIMSEType::MAXCOUNT);


	enum EDcmProtocol
	{
		C_MOVE,
		C_GET,
		MAXCOUNT,
	};
	DECLARE_ENUM_TEXT(EDcmProtocol, DCM_PROTOCOL_TABLE, (int)EDcmProtocol::MAXCOUNT);

	enum class EDcmFindMode
	{
		PATIENT_NAME,
		PATIENT_ID,
		PATIENT_SEX,
		ACCESSION_NUMBER,
		EXAM_ID,
		STUDY_DESCRIPTION,
		REFERRING_PHYSICIAN,
		PERFORMING_PHYSICIAN,
		READING_PHYSICIAN,
		INSTITUTION_NAME,
		MAXCOUNT,
	};
	DECLARE_ENUM_TEXT(EDcmFindMode, DCM_FIND_MODE_TABLE, (int)EDcmFindMode::MAXCOUNT);

	enum class EDcmModality
	{
		ALL_MODALITY,
		CR,
		DX,
		CT,
		MR,
		XA,
		MG,
		US,
		PT,
		NM,
		RF,
		RG,
		PX,
		ES,
		XC,
		GM,
		SC,
		OT,
		SR,
		MAXCOUNT
	};
	DECLARE_ENUM_TEXT(EDcmModality, DCM_MODALITY_MODE_TABLE, (int)EDcmModality::MAXCOUNT);

	enum EQueryRetrieveLevel
	{
		QR_LEVEL_PATIENT,
		QR_LEVEL_STUDY,
		QR_LEVEL_SERIES,
		QR_LEVEL_IMAGE,
	};

	struct QUERY_ABSTRACT_SYNTAX
	{
		const char* FindSyntax;
		const char* MoveSyntax;
		const char* GetSyntax;
	};

	struct DICOM_HEADER_INFO
	{
		DateTime StudyDateTime;
		DateTime SeriesDateTime;
		std::wstring StudyInstanceUID;
		std::wstring SeriesInstanceUID;
		std::wstring PatientName;
		DateTime DateOfBirth;
		std::wstring PatientID;
		std::wstring PatientSex;
		std::wstring PatientAge;
		std::wstring Modality;
		std::wstring StudyDescription;
		std::wstring SeriesDescription;
		std::wstring AccessionNumber;
		std::wstring ExamID;
		std::wstring ReferringPhysician;
		std::wstring PerformingPhysician;
		std::wstring RequestingPhysician;
		std::wstring ReadingPhysician;
		int SeriesNumber;

		int NumberOfPatientRelatedStudies;
		int NumberOfPatientRelatedSeries;
		int NumberOfPatientRelatedInstances;
		int NumberOfStudyRelatedSeries;
		int NumberOfStudyRelatedInstances;
		int NumberOfSeriesRelatedInstances;

		int RETIRED_AcquisitionsInSeries;
		int RETIRED_ImagesInSeries;
		int RETIRED_AcquisitionsInStudy;
		int RETIRED_ImagesInStudy;

		int ImagesInAcqusition;

		int InstanceNumber;

		std::wstring Manufacturer;
		std::wstring ManufacturerModelName;
		std::wstring InstitutionName;

		std::wstring Tag;
		std::wstring Comment;
	};

	enum EDicomNetworkResult
	{
		SUCCESS,
		FAIL_TO_SCU_ASSOCIATION_NETWORK,
		FAIL_TO_SCU_SETUP_PRESENTATION_CONTEXT_ID,
		FAIL_TO_SCU_SETUP_SEND_NETWORK_MESSAGE,
		FAIL_TO_SCU_RELEASE_ASSOCIATION_NETWORK,
		FAIL_TO_SCU_SEND_DIMSE_ECHO,
		FAIL_TO_SCU_SEND_DIMSE_FIND,
		FAIL_TO_SCU_SEND_DIMSE_GET,
		FAIL_TO_SCU_SEND_DIMSE_MOVE,
		FAIL_TO_SCU_SEND_DIMSE_STORE,
		INVALID_DOWNLOAD_PROTOCOL,
		FAIL_TO_STORE_SCP_INITALIZE_LISTNER,
		FAIL_TO_STORE_SCP_RECEIVE_ASSOCIATION,
		FAIL_TO_STORE_SCP_DOWNLOADING,
		FAIL_TO_STORE_SCP_RELEASE_ASSOC,
		FAIL_TO_STORE_SCP_DROP_NETWORK,
	};

};

