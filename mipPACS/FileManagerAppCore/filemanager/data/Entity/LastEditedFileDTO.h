#pragma once

#include <string>
#include "filemanager/export.h"
#include "filemanager/std/DateTime.h"

class QDomDocument;
class QDomNode;
class DataConnection;

namespace fm
{
	class FM_CORE_EXPORT LastEditedFileDTO
	{
	public:
		std::wstring FilePath;
		fm::DateTime EditTime;
		std::wstring SeriesInstanceUID;
		std::wstring StudyInstanceUID;
		std::wstring PatientName;
		std::wstring DateOfBirth;
		std::wstring PatientID;
		std::wstring Modality;
		std::wstring StudyDescription;
		std::wstring SeriesDescription;
		std::wstring AccessionNumber;
		std::wstring ExamID;
		std::wstring ReferringPhysician;
		std::wstring PerformingPhysician;
		std::wstring ReadingPhysician;

	public:
		void SetData(std::wstring column, std::wstring value);
		std::wstring GetData(std::wstring column);
	};

};
