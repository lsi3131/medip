#include "stdafx.h"
#include "GUISettingConfig.h"

#define FOLDER_WIDGET_GROUP_NAME "FolderWidgetGroup"

namespace fm
{
	GUISettingConfig::GUISettingConfig() :
		m_pSettings(nullptr)
	{
	}

	GUISettingConfig::~GUISettingConfig()
	{
		if (m_pSettings)
		{
			delete m_pSettings;
			m_pSettings = nullptr;
		}
	}

	bool GUISettingConfig::Load(std::wstring filepath)
	{
		if (m_pSettings)
		{
			delete m_pSettings;
			m_pSettings = nullptr;
		}
		m_pSettings = new QSettings(QString::fromStdWString(filepath), QSettings::Format::IniFormat);

		m_pSettings->beginGroup("MainWidget");
		m_mainWidgetInfo.Geometry = m_pSettings->value("Geometry").toByteArray();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("FolderViewWidget");
		m_folderViewWidgetInfo.CenterHorizontalContainerSplitterStatus = m_pSettings->value("CenterHorizontalContainerSplitter").toByteArray();
		m_folderViewWidgetInfo.LeftContainerSplitterStatus = m_pSettings->value("LeftContainerSplitter").toByteArray();
		m_folderViewWidgetInfo.FileViewHeaderStatus = m_pSettings->value("FileViewHeader").toByteArray();
		m_folderViewWidgetInfo.SubWindowInfo_Folder.Show = m_pSettings->value("SubWindow_Folder_Show", true).toBool();
		m_folderViewWidgetInfo.SubWindowInfo_Favorite.Show = m_pSettings->value("SubWindow_Favorite_Show", true).toBool();
		m_folderViewWidgetInfo.SubWindowInfo_System.Show = m_pSettings->value("SubWindow_System_Show", true).toBool();
		m_folderViewWidgetInfo.SubWindowInfo_Recent.Show = m_pSettings->value("SubWindow_Recent_Show", true).toBool();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PatientListWidget");
		m_patientListWidgetInfo.CenterHorizontalContainerSplitterStatus = m_pSettings->value("CenterHorizontalContainerSplitterStatus").toByteArray();
		m_patientListWidgetInfo.RightContainerSplitterStatus = m_pSettings->value("RightContainerSplitterStatus").toByteArray();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PACSSearchDownloadWidget");
		m_pacsSearchDownloadWidgetInfo.CenterHorizontalContainerSplitterStatus = m_pSettings->value("CenterHorizontalContainerSplitterStatus").toByteArray();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PACSExportWidgetInfo");
		m_pacsExportWidgetInfo.CenterVerticalContainerSplitterStatus = m_pSettings->value("CenterVerticalContainerSplitterStatus").toByteArray();
		m_pacsExportWidgetInfo.BottomHorizontalContainerSplitterStatus = m_pSettings->value("BottomHorizontalContainerSplitterStatus").toByteArray();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("DicomPreviewWidgetInfo");
		m_dcmPreviewWidgetInfo.CenterVerticalContainerSplitterStatus = m_pSettings->value("CenterVerticalContainerSplitterStatus").toByteArray();
		m_dcmPreviewWidgetInfo.BottomHorizontalContainerSplitterStatus = m_pSettings->value("BottomHorizontalContainerSplitterStatus").toByteArray();
		m_pSettings->endGroup();

		m_pSettings->beginGroup("DicomImportDataDialogInfo");
		m_dcmImportDataDialogInfo.Geometry = m_pSettings->value("Geometry").toByteArray();
		m_dcmImportDataDialogInfo.CenterVerticalContainerSplitterStatus = m_pSettings->value("CenterVerticalContainerSplitterStatus").toByteArray();
		m_dcmImportDataDialogInfo.BottomHorizontalContainerSplitterStatus = m_pSettings->value("BottomHorizontalContainerSplitterStatus").toByteArray();;
		m_pSettings->endGroup();

		return true;
	}

	bool GUISettingConfig::Save()
	{
		if (m_pSettings == nullptr)
		{
			return false;
		}

		m_pSettings->beginGroup("MainWidget");
		m_pSettings->setValue("Geometry", m_mainWidgetInfo.Geometry);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("FolderViewWidget");
		m_pSettings->setValue("CenterHorizontalContainerSplitter", m_folderViewWidgetInfo.CenterHorizontalContainerSplitterStatus);
		m_pSettings->setValue("LeftContainerSplitter", m_folderViewWidgetInfo.LeftContainerSplitterStatus);
		m_pSettings->setValue("FileViewHeader", m_folderViewWidgetInfo.FileViewHeaderStatus);
		m_pSettings->setValue("SubWindow_Folder_Show", m_folderViewWidgetInfo.SubWindowInfo_Folder.Show);
		m_pSettings->setValue("SubWindow_Favorite_Show", m_folderViewWidgetInfo.SubWindowInfo_Favorite.Show);
		m_pSettings->setValue("SubWindow_System_Show", m_folderViewWidgetInfo.SubWindowInfo_System.Show);
		m_pSettings->setValue("SubWindow_Recent_Show", m_folderViewWidgetInfo.SubWindowInfo_Recent.Show);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PatientListWidget");
		m_pSettings->setValue("CenterHorizontalContainerSplitterStatus", m_patientListWidgetInfo.CenterHorizontalContainerSplitterStatus);
		m_pSettings->setValue("RightContainerSplitterStatus", m_patientListWidgetInfo.RightContainerSplitterStatus);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PACSSearchDownloadWidget");
		m_pSettings->setValue("CenterHorizontalContainerSplitterStatus", m_pacsSearchDownloadWidgetInfo.CenterHorizontalContainerSplitterStatus);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("PACSExportWidgetInfo");
		m_pSettings->setValue("CenterVerticalContainerSplitterStatus", m_pacsExportWidgetInfo.CenterVerticalContainerSplitterStatus);
		m_pSettings->setValue("BottomHorizontalContainerSplitterStatus", m_pacsExportWidgetInfo.BottomHorizontalContainerSplitterStatus);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("DicomPreviewWidgetInfo");
		m_pSettings->setValue("CenterVerticalContainerSplitterStatus", m_dcmPreviewWidgetInfo.CenterVerticalContainerSplitterStatus);
		m_pSettings->setValue("BottomHorizontalContainerSplitterStatus", m_dcmPreviewWidgetInfo.BottomHorizontalContainerSplitterStatus);
		m_pSettings->endGroup();

		m_pSettings->beginGroup("DicomImportDataDialogInfo");
		m_pSettings->setValue("Geometry", m_dcmImportDataDialogInfo.Geometry);
		m_pSettings->setValue("CenterVerticalContainerSplitterStatus", m_dcmImportDataDialogInfo.CenterVerticalContainerSplitterStatus);
		m_pSettings->setValue("BottomHorizontalContainerSplitterStatus", m_dcmImportDataDialogInfo.BottomHorizontalContainerSplitterStatus);
		m_pSettings->endGroup();

		return true;
	}

	MainWidgetInfo GUISettingConfig::GetMainWidgetInfo()
	{
		return m_mainWidgetInfo;
	}

	void GUISettingConfig::SetMainWidgetInfo(const MainWidgetInfo& info)
	{
		m_mainWidgetInfo = info;
	}

	FolderViewWidgetInfo GUISettingConfig::GetFolderViewWidgetInfo()
	{
		return m_folderViewWidgetInfo;
	}

	void GUISettingConfig::SetFolderViewWidgetInfo(const FolderViewWidgetInfo& info)
	{
		m_folderViewWidgetInfo = info;
	}

	PatientListWidgetInfo GUISettingConfig::GetPatientListWidgetInfo()
	{
		return m_patientListWidgetInfo;
	}

	void GUISettingConfig::SetPatientListWidgetInfo(const PatientListWidgetInfo& info)
	{
		m_patientListWidgetInfo = info;
	}

	PACSSearchDownloadWidgetInfo GUISettingConfig::GetPACSSearchDownloadWidget()
	{
		return m_pacsSearchDownloadWidgetInfo;
	}

	void GUISettingConfig::SetPACSSearchDownloadWidget(const PACSSearchDownloadWidgetInfo& info)
	{
		m_pacsSearchDownloadWidgetInfo = info;
	}

	PACSExportWidgetInfo GUISettingConfig::GetPACSExportWidget()
	{
		return m_pacsExportWidgetInfo;
	}

	void GUISettingConfig::SetPACSExportWidget(const PACSExportWidgetInfo& info)
	{
		m_pacsExportWidgetInfo = info;
	}

	DicomPreviewWidgetInfo GUISettingConfig::GetDicomPreviewWidgetInfo()
	{
		return m_dcmPreviewWidgetInfo;
	}

	void GUISettingConfig::SetDicomPreviewWidgetInfo(const DicomPreviewWidgetInfo& info)
	{
		m_dcmPreviewWidgetInfo = info;
	}

	DicomImportDataDialogInfo GUISettingConfig::GetDicomImportDataDialogInfo()
	{
		return m_dcmImportDataDialogInfo;
	}

	void GUISettingConfig::SetDicomImportDataDialogInfo(const DicomImportDataDialogInfo& info)
	{
		m_dcmImportDataDialogInfo = info;
	}
}
