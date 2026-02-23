#include "stdafx.h"
#include "ImportedDicomInfoDTO.h"
#include <qfileinfo>
#include <qdir>

using namespace fm;

#define FILE_LIST_DELIMETER "\\"
#define FILE_LIST_DELIMETER_WIDE L"\\"

ImportedDicomInfoDTO::ImportedDicomInfoDTO() :
	ImageCount(0),
	SeriesCountInStudy(0)
{
}

void fm::ImportedDicomInfoDTO::SetData(std::wstring column, std::wstring value)
{
	if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_PATH)
	{
		DicomFilePath = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID)
	{
		SeriesInstanceUID = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID)
	{
		StudyInstanceUID = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_NAME)
	{
		PatientName = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DATE_OF_BIRTH)
	{
		DateOfBirth.SetFormattedText(StringUtil::WideToMulitiByte(value));
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_DATE)
	{
		SeriesDateTime.SetFormattedText(StringUtil::WideToMulitiByte(value));
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_DATE)
	{
		StudyDateTime.SetFormattedText(StringUtil::WideToMulitiByte(value));
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_IMPORTED_DATE)
	{
		ImportedDateTime.SetFormattedText(StringUtil::WideToMulitiByte(value));
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_ID)
	{
		PatientID = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_AGE)
	{
		PatientAge = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_SEX)
	{
		PatientSex = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_MODALITY)
	{
		Modality = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_DESCRIPTION)
	{
		StudyDescription = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_DESCRIPTION)
	{
		SeriesDescription = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_ACCESSION_NUMBER)
	{
		AccessionNumber = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_EXAM_ID)
	{
		ExamID = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_REFERRING_PHYSICIAN)
	{
		ReferringPhysician = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PERFORMING_PHYSICIAN)
	{
		PerformingPhysician = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_READING_PHYSICIAN)
	{
		ReadingPhysician = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_REQUESTING_PHYSICIAN)
	{
		RequestPhysician = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_NUMBER)
	{
		SeriesNumber = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_IMAGE_COUNT)
	{
		ImageCount = _wtoi(value.data());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_COUNT_IN_STUDY)
	{
		SeriesCountInStudy = _wtoi(value.data());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_TAG)
	{
		Tag = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_DIRECTORY_PATH)
	{
		DicomDirectoryPath = value;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_LIST)
	{
		DicomFileList = SplitDicomFileList(value);
	}
}

std::wstring fm::ImportedDicomInfoDTO::GetData(std::wstring column)
{
	if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_PATH)
	{
		return DicomFilePath;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_INSTANCE_UID)
	{
		return SeriesInstanceUID;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_INSTANCE_UID)
	{
		return StudyInstanceUID;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_NAME)
	{
		return PatientName;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DATE_OF_BIRTH)
	{
		return StringUtil::MultiByteToWide(DateOfBirth.ToFormatText_DateTime());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_DATE)
	{
		return StringUtil::MultiByteToWide(SeriesDateTime.ToFormatText_DateTime());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_DATE)
	{
		return StringUtil::MultiByteToWide(StudyDateTime.ToFormatText_DateTime());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_IMPORTED_DATE)
	{
		return StringUtil::MultiByteToWide(ImportedDateTime.ToFormatText_DateTime());
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_ID)
	{
		return PatientID;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_AGE)
	{
		return PatientAge;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PATIENT_SEX)
	{
		return PatientSex;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_MODALITY)
	{
		return Modality;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_STUDY_DESCRIPTION)
	{
		return StudyDescription;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_DESCRIPTION)
	{
		return SeriesDescription;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_ACCESSION_NUMBER)
	{
		return AccessionNumber;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_EXAM_ID)
	{
		return ExamID;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_REFERRING_PHYSICIAN)
	{
		return ReferringPhysician;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_PERFORMING_PHYSICIAN)
	{
		return PerformingPhysician;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_READING_PHYSICIAN)
	{
		return ReadingPhysician;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_REQUESTING_PHYSICIAN)
	{
		return RequestPhysician;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_NUMBER)
	{
		return SeriesNumber;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_IMAGE_COUNT)
	{
		return QString::number(ImageCount).toStdWString();
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_SERIES_COUNT_IN_STUDY)
	{
		return QString::number(SeriesCountInStudy).toStdWString();
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_TAG)
	{
		return Tag;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_DIRECTORY_PATH)
	{
		return DicomDirectoryPath;
	}
	else if (column == ImportedDicomInfoDefines::COL_NAME_DICOM_FILE_LIST)
	{
		return JoinDicomFileList(DicomFileList);
	}

	return L"";
}

bool fm::ImportedDicomInfoDTO::CompareData(std::wstring column, std::wstring value)
{
	return GetData(column) == value;
}

bool fm::ImportedDicomInfoDTO::FindData(std::wstring column, std::wstring value)
{
	return GetData(column).find(value) != std::wstring::npos;
}

void fm::ImportedDicomInfoDTO::InitFromDcmDataset(const DicomDataset& dcmDataset)
{
	DICOM_HEADER_INFO dcmHeaderInfo = dcmDataset.ToDcmHeaderInfo();
	QString dcmFilePath = QString::fromStdWString(dcmDataset.GetValue_Extension(DCM_EXT_FILE_PATH));
	QFileInfo fileInfo(dcmFilePath);

	DicomFilePath = dcmFilePath.toStdWString();
	SeriesInstanceUID = dcmHeaderInfo.SeriesInstanceUID;
	StudyInstanceUID = dcmHeaderInfo.StudyInstanceUID;
	PatientName = dcmHeaderInfo.PatientName;
	PatientSex = dcmHeaderInfo.PatientSex;
	PatientAge = dcmHeaderInfo.PatientAge;
	DateOfBirth = dcmHeaderInfo.DateOfBirth;
	StudyDateTime = dcmHeaderInfo.StudyDateTime;
	SeriesDateTime = dcmHeaderInfo.SeriesDateTime;
	ImportedDateTime = DateTime::CurrentDateTime();
	OpenedDateTime = DateTime::CurrentDateTime();
	PatientID = dcmHeaderInfo.PatientID;
	Modality = dcmHeaderInfo.Modality;
	StudyDescription = dcmHeaderInfo.StudyDescription;
	SeriesDescription = dcmHeaderInfo.SeriesDescription;
	AccessionNumber = dcmHeaderInfo.AccessionNumber;
	ExamID = dcmHeaderInfo.ExamID;
	ReferringPhysician = dcmHeaderInfo.ReferringPhysician;
	PerformingPhysician = dcmHeaderInfo.PerformingPhysician;
	ReadingPhysician = dcmHeaderInfo.ReadingPhysician;
	RequestPhysician = dcmHeaderInfo.RequestingPhysician;
	SeriesNumber = QString::number(dcmHeaderInfo.SeriesNumber).toStdWString();

	Tag = L"";
	DicomDirectoryPath = fileInfo.dir().path().toStdWString();
}

DicomDataset fm::ImportedDicomInfoDTO::ToDcmDataset()
{
	DicomDataset dcmDatasetExt;

	dcmDatasetExt.SetValue_Extension(DCM_EXT_FILE_PATH, DicomFilePath);
	dcmDatasetExt.SetValue_Extension(DCM_EXT_IMPORT_DATETIME_FORMAT, ImportedDateTime.ToFormatText_DateTime());
	dcmDatasetExt.SetValue_Extension(DCM_EXT_OPENED_DATETIME_FORMAT, OpenedDateTime.ToFormatText_DateTime());
	dcmDatasetExt.SetValue_Extension(DCM_EXT_TAG, Tag);
	dcmDatasetExt.SetValue_Extension(DCM_EXT_DIRECTORY_PATH, DicomDirectoryPath);
	dcmDatasetExt.SetValue_Extension(DCM_EXT_FILE_LIST, JoinDicomFileList(DicomFileList));

	dcmDatasetExt.SetTagValue(fm::DicomTagID::SeriesInstanceUID, SeriesInstanceUID);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::StudyInstanceUID, StudyInstanceUID);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PatientName, PatientName);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PatientAge, PatientAge);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PatientSex, PatientSex);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PatientBirthDate, DateOfBirth.ToYYYYMMDD());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::SeriesDate, SeriesDateTime.ToYYYYMMDD());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::SeriesTime, SeriesDateTime.Tohhmmss());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::StudyDate, StudyDateTime.ToYYYYMMDD());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::StudyTime, StudyDateTime.Tohhmmss());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PatientID, PatientID);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::Modality, Modality);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::StudyDescription, StudyDescription);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::SeriesDescription, SeriesDescription);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::AccessionNumber, AccessionNumber);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::StudyID, ExamID);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::ReferringPhysicianName, ReferringPhysician);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::PerformingPhysicianName, PerformingPhysician);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::RequestingPhysician, RequestPhysician); //TODO : Requesting Physician 이름 확인할 것 
	dcmDatasetExt.SetTagValue(fm::DicomTagID::SeriesNumber, SeriesNumber);
	dcmDatasetExt.SetTagValue(fm::DicomTagID::NumberOfSeriesRelatedInstances, QString::number(ImageCount).toStdWString());
	dcmDatasetExt.SetTagValue(fm::DicomTagID::NumberOfStudyRelatedInstances, QString::number(SeriesCountInStudy).toStdWString());

	return dcmDatasetExt;
}

std::wstring fm::JoinDicomFileList(const std::vector<std::wstring>& dicomFileList)
{
	std::wstring text;
	if (dicomFileList.empty())
	{
		return text;
	}
	int i = 0;
	int count = dicomFileList.size();
	for (i = 0; i < (count - 1); ++i)
	{
		text += dicomFileList[i] + FILE_LIST_DELIMETER_WIDE;
	}
	if (i < count)
	{
		text += dicomFileList[i];
	}
	return text;
}

std::vector<std::wstring> fm::SplitDicomFileList(const std::wstring& dicomFileListText)
{
	QString text = QString::fromStdWString(dicomFileListText);
	QStringList strList = text.split(FILE_LIST_DELIMETER);
	std::vector<std::wstring> list;

	for (auto& str : strList)
	{
		list.push_back(str.toStdWString());
	}
	return list;
}
