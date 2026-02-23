#pragma once

class WindowManager;
class DataContext;
class ActionManager;

class FileWriterMesh
{
public:
	FileWriterMesh(WindowManager* pWinManager, DataContext* pDataContext, ActionManager* pActionManager);

public:
	bool SaveMeshFiles(QString filepath, muint8 UID, EXPORT_FILES _fType, bool bPatientCoordinate, bool appendType, bool bEnableProgress, QString qstrProgressTitle);
	bool saveMeshFilesVisualPrint(QString filepath, muint8 UID, EXPORT_FILES _fType, bool appendType, bool bEnableProgress, QString qstrProgressTitle);

private:
	bool initForExport(QString filepath, muint8 UID, EXPORT_FILES _fType, bool appendType, const std::vector<QString>& extensionList, EXPORT_FILES startRange, EXPORT_FILES endRange);

private:
	WindowManager* m_pWinManager;
	DataContext* m_pDataContext;
	ActionManager* m_pActionManager;

	EXPORT_FILES m_exportFileType;
	QString m_filepath;
};
