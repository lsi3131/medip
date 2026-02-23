#include "stdafx.h"
#include "XmlDataFilePathManager.h"
#include "filemanager/connection/XmlConnection.h"

using namespace fm;

const std::wstring XmlDataFilePathManager::LAST_EDITED_FILE_LIST_XML_FILE_NAME = L"last_edited_file_list.xml";
const std::wstring XmlDataFilePathManager::IMPORTED_DICOM_INFO_LIST_XML_FILE_NAME = L"imported_dicom_info_list.xml";

std::wstring XmlDataFilePathManager::LastEditedFileListXmlFilePath(std::wstring xmlDataDirectoryPath)
{
	return xmlDataDirectoryPath + L"/" + LAST_EDITED_FILE_LIST_XML_FILE_NAME;
}

std::wstring XmlDataFilePathManager::ImportedDicomInfoListXmlFilePath(std::wstring xmlDataDirectoryPath)
{
	return xmlDataDirectoryPath + L"/" + IMPORTED_DICOM_INFO_LIST_XML_FILE_NAME;
}
