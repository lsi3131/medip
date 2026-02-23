#include "stdafx.h"
#include "FileWriterMesh.h"
#include "windowManager.h"
#include "DataContext.h"
#include "ActionManager.h"

static std::vector<QString> MESH_EXTENSION_LIST = { QString("stl"), QString("obj"), QString("vtk"), QString("3mf") };
static std::vector<QString> MESH_VISUALPRINT_EXTENSION_LIST = { QString("stl"), QString("obj"), QString("vtk") };

FileWriterMesh::FileWriterMesh(WindowManager* pWinManager, DataContext* pDataContext, ActionManager* pActionManager) :
	m_pWinManager(pWinManager),
	m_pDataContext(pDataContext),
	m_pActionManager(pActionManager),
	m_exportFileType(EX_FILES_NONE)
{
}

bool FileWriterMesh::SaveMeshFiles(QString filepath, muint8 UID, EXPORT_FILES _fType, bool bPatientCoordinate, bool appendType, bool bEnableProgress, QString qstrProgressTitle)
{
	if (initForExport(filepath, UID, _fType, appendType, MESH_EXTENSION_LIST, EX_FILES_STL, EX_FILES_USD) == false)
	{
		return false;
	}

	MeshLayerData meshLayerData = m_pDataContext->m_MeshData.GetMeshDataByLayerIndex(UID);
	mip::MeshTopology* pMesh = meshLayerData.Data;
	MeshInfo* pInfo = meshLayerData.Info;
	QVector<int> exportIndexList = m_pWinManager->exportList;

	switch (m_exportFileType)
	{
	case EX_FILES_STL:
	{
		if (m_pWinManager->exportList.empty())
		{
			m_pWinManager->exportList.push_back(UID);
		}

		m_pActionManager->action_FileWork_Export_Mesh_To_STLFile_Multi(m_pDataContext, filepath, bPatientCoordinate);
		break;
	}
	case EX_FILES_OBJ:
		m_pActionManager->action_FileWork_Export_OBJFile(m_pDataContext, m_filepath, pMesh, pInfo->upScale, bEnableProgress, qstrProgressTitle, bPatientCoordinate);
		break;
	case EX_FILES_VTK:
		m_pActionManager->action_FileWork_Export_VTKFile(m_pDataContext, m_filepath, pMesh, pInfo->upScale, bPatientCoordinate);
		break;
	case EX_FILES_3MF:
		m_pActionManager->action_FileWork_Export_3MFFile(m_pDataContext, m_filepath, pMesh, pInfo->upScale, bPatientCoordinate);
		break;

	case EX_FILES_USD:
		m_pActionManager->action_FileWork_Export_USDFile(m_pDataContext, m_filepath, exportIndexList.toStdVector());
		break;
	}

	return true;
}

bool FileWriterMesh::saveMeshFilesVisualPrint(QString filepath, muint8 UID, EXPORT_FILES _fType, bool appendType, bool bEnableProgress, QString qstrProgressTitle)
{
	if (initForExport(filepath, UID, _fType, appendType, MESH_VISUALPRINT_EXTENSION_LIST, EX_FILES_STL, EX_FILES_VTK) == false)
	{
		return false;
	}

	MeshLayerData meshLayerData = m_pDataContext->m_MeshData.GetMeshDataByLayerIndex(UID);
	mip::MeshTopology* pMesh = meshLayerData.Data;
	MeshInfo* pInfo = meshLayerData.Info;

	switch (m_exportFileType)
	{
	case EX_FILES_STL:
		m_pActionManager->action_FileWork_Export_Mesh_To_STLFile_Single(m_pDataContext, m_filepath, pMesh, pInfo->upScale, bEnableProgress, qstrProgressTitle);
		break;
	case EX_FILES_OBJ:
		m_pActionManager->action_FileWork_Export_OBJFile(m_pDataContext, m_filepath, pMesh, pInfo->upScale, bEnableProgress, qstrProgressTitle);
		break;
	case EX_FILES_VTK:
		m_pActionManager->action_FileWork_Export_VTKFile(m_pDataContext, m_filepath, pMesh, pInfo->upScale);
		break;
	}

	return true;
}

bool FileWriterMesh::initForExport(
	QString filepath, 
	muint8 UID, 
	EXPORT_FILES _fType, 
	bool appendType, 
	const std::vector<QString>& extensionList, 
	EXPORT_FILES startRange, 
	EXPORT_FILES endRange)
{
#ifdef ONLY_FOR_FTP_UPLOAD_VER

#else 
	if (!m_pWinManager->IsLicensePass())
	{
		QMessageBox::warning(NULL, "License", "The program must be licensed to activate.");
		return false;
	}

#endif
	m_filepath = filepath;
	m_exportFileType = EX_FILES_NONE;

	MeshLayerData meshLayerData = m_pDataContext->m_MeshData.GetMeshDataByLayerIndex(UID);
	mip::MeshTopology* pMesh = meshLayerData.Data;
	MeshInfo* pInfo = meshLayerData.Info;

	if (pMesh == nullptr)
	{
		return false;
	}

	if (pInfo == nullptr)
	{
		return false;
	}

	if (_fType != EX_FILES_NONE)
	{
		m_exportFileType = _fType;
	}

	if (m_exportFileType == EX_FILES_NONE)
	{
		return false;
	}

	if (!(startRange <= m_exportFileType && m_exportFileType <= endRange))
	{
		return false;
	}

	if (appendType)
	{
		m_filepath += MESH_EXTENSION_LIST[m_exportFileType];
	}

	bool isDuplicateFileExist = m_pWinManager->CheckDupFile(filepath, !appendType);

	if (isDuplicateFileExist)
	{
		QFile file(filepath);
		file.remove();
	}

	return true;
}

