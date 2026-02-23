/*
	작성자 : 이상일
	목적 :
	EntityDataManager에서 Xml DAO에 사용할 XmlConnection을 관리하는 Class
*/

#pragma once

#include "filemanager/export.h"
#include <string>

namespace fm
{
	class XmlConnection;

	class FM_CORE_EXPORT XmlDataFilePathManager
	{
	public:
		const static std::wstring LAST_EDITED_FILE_LIST_XML_FILE_NAME;
		const static std::wstring IMPORTED_DICOM_INFO_LIST_XML_FILE_NAME;

	public:
		static std::wstring LastEditedFileListXmlFilePath(std::wstring xmlDataDirectoryPath);
		static std::wstring ImportedDicomInfoListXmlFilePath(std::wstring xmlDataDirectoryPath);
	};
};
