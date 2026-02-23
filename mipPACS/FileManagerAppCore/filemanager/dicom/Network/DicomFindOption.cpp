#include "stdafx.h"
#include "DicomFindOption.h"

namespace fm
{
	DicomFindOption::DicomFindOption() :
		Mode(EDcmFindMode::PATIENT_NAME),
		Modality(EDcmModality::ALL_MODALITY),
		QueryRetrieveLevel(EQueryRetrieveLevel::QR_LEVEL_SERIES),
		m_allDate(true)
	{
	}

	DicomFindOption::DicomFindOption(EDcmFindMode mode, std::wstring text, EDcmModality modality, EQueryRetrieveLevel qrLevel) :
		Mode(mode),
		Text(text),
		Modality(modality),
		QueryRetrieveLevel(qrLevel),
		m_allDate(true)
	{
	}

	DicomFindOption::DicomFindOption(std::wstring studyInstanceUID) :
		Mode(EDcmFindMode::PATIENT_NAME),
		Modality(EDcmModality::ALL_MODALITY),
		QueryRetrieveLevel(EQueryRetrieveLevel::QR_LEVEL_SERIES),
		StudyInstanceUID(studyInstanceUID),
		m_allDate(true)
	{
		DicomFindOption();
	}

	void DicomFindOption::SetDateRange(DateTime start, DateTime end)
	{
		m_allDate = false;
		m_startDateTime = start;
		m_endDateTime = end;
	}

	bool DicomFindOption::HasDateRange()
	{
		return m_allDate == false;
	}

	bool DicomFindOption::IsSearchByStudyInstanceUIDMode()
	{
		return !StudyInstanceUID.empty();
	}

	std::wstring DicomFindOption::GetDateRange_Text()
	{
		QString textStartDate;
		QString textEndDate;

		textStartDate.sprintf("%04d%02d%02d", m_startDateTime.Year(), m_startDateTime.Month(), m_startDateTime.Day());
		textEndDate.sprintf("%04d%02d%02d", m_endDateTime.Year(), m_endDateTime.Month(), m_endDateTime.Day());

		return (textStartDate + "-" + textEndDate).toStdWString();
	}
}

