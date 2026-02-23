#pragma once

#include <qsettings>
#include <qrect>

namespace fm
{
	struct MainWidgetInfo
	{
		QByteArray Geometry;
	};

	struct FolderViewWidgetInfo
	{
		struct SubWindowInfo
		{
			bool Show = false;
		};

		QByteArray CenterHorizontalContainerSplitterStatus;
		QByteArray LeftContainerSplitterStatus;
		QByteArray FileViewHeaderStatus;

		SubWindowInfo SubWindowInfo_Folder;
		SubWindowInfo SubWindowInfo_Favorite;
		SubWindowInfo SubWindowInfo_System;
		SubWindowInfo SubWindowInfo_Recent;
	};

	struct PatientListWidgetInfo
	{
		QByteArray CenterHorizontalContainerSplitterStatus;
		QByteArray RightContainerSplitterStatus;
	};

	struct PACSSearchDownloadWidgetInfo
	{
		QByteArray CenterHorizontalContainerSplitterStatus;
	};

	struct PACSExportWidgetInfo
	{
		QByteArray CenterVerticalContainerSplitterStatus;
		QByteArray BottomHorizontalContainerSplitterStatus;
	};

	struct DicomPreviewWidgetInfo
	{
		QByteArray CenterVerticalContainerSplitterStatus;
		QByteArray BottomHorizontalContainerSplitterStatus;
	};

	struct DicomImportDataDialogInfo
	{
		QByteArray Geometry;
		QByteArray CenterVerticalContainerSplitterStatus;
		QByteArray BottomHorizontalContainerSplitterStatus;
	};

	class GUISettingConfig
	{
	public:
		GUISettingConfig();
		~GUISettingConfig();

	public:
		bool Load(std::wstring filepath);
		bool Save();

	public:
		MainWidgetInfo GetMainWidgetInfo();
		void SetMainWidgetInfo(const MainWidgetInfo& info);

		FolderViewWidgetInfo GetFolderViewWidgetInfo();
		void SetFolderViewWidgetInfo(const FolderViewWidgetInfo& info);

		PatientListWidgetInfo GetPatientListWidgetInfo();
		void SetPatientListWidgetInfo(const PatientListWidgetInfo& info);

		PACSSearchDownloadWidgetInfo GetPACSSearchDownloadWidget();
		void SetPACSSearchDownloadWidget(const PACSSearchDownloadWidgetInfo& info);

		PACSExportWidgetInfo GetPACSExportWidget();
		void SetPACSExportWidget(const PACSExportWidgetInfo& info);

		DicomPreviewWidgetInfo GetDicomPreviewWidgetInfo();
		void SetDicomPreviewWidgetInfo(const DicomPreviewWidgetInfo& info);

		DicomImportDataDialogInfo GetDicomImportDataDialogInfo();
		void SetDicomImportDataDialogInfo(const DicomImportDataDialogInfo& info);
	private:
		QSettings* m_pSettings;

		MainWidgetInfo m_mainWidgetInfo;
		FolderViewWidgetInfo m_folderViewWidgetInfo;
		PatientListWidgetInfo m_patientListWidgetInfo;
		PACSSearchDownloadWidgetInfo m_pacsSearchDownloadWidgetInfo;
		PACSExportWidgetInfo m_pacsExportWidgetInfo;
		DicomPreviewWidgetInfo m_dcmPreviewWidgetInfo;
		DicomImportDataDialogInfo m_dcmImportDataDialogInfo;
	};
}
