#include "stdafx.h"
#include "EntityDataManager.h"
#include "filemanager/data/entity/LastEditedFileDAO_Xml.h"
#include "filemanager/data/entity/ImportedDicomInfoDAO_Xml.h"

using namespace fm;

EntityDataManager::EntityDataManager() :
	m_ImportedDicomInfoDAO(nullptr),
	m_LastEditedFileDAO(nullptr)
{
}

EntityDataManager::~EntityDataManager()
{
	if (m_ImportedDicomInfoDAO != nullptr)
	{
		delete m_ImportedDicomInfoDAO;
		m_ImportedDicomInfoDAO = nullptr;
	}

	if (m_LastEditedFileDAO != nullptr)
	{
		delete m_LastEditedFileDAO;
		m_LastEditedFileDAO = nullptr;
	}
}

void fm::EntityDataManager::Initialize_XML(std::wstring xmlDataDir)
{
	std::wstring xmlFilePath;
	if (m_LastEditedFileDAO != nullptr)
	{
		delete m_LastEditedFileDAO;
	}

	xmlFilePath = XmlDataFilePathManager::LastEditedFileListXmlFilePath(xmlDataDir);
	m_LastEditedFileDAO = new LastEditedFileDAO_Xml(xmlFilePath);
	m_LastEditedFileDAO->Initialize();

	if (m_ImportedDicomInfoDAO != nullptr)
	{
		delete m_ImportedDicomInfoDAO;
	}

	xmlFilePath = XmlDataFilePathManager::ImportedDicomInfoListXmlFilePath(xmlDataDir);
	m_ImportedDicomInfoDAO = new ImportedDicomInfoDAO_Xml(xmlFilePath);
	m_ImportedDicomInfoDAO->Initialize();
}

ImportedDicomInfoDAO* EntityDataManager::GetImportedDicomInfoDAO()
{
	return m_ImportedDicomInfoDAO;
}

LastEditedFileDAO*  EntityDataManager::GetLastEditedFileDAO()
{
	return m_LastEditedFileDAO;
}



