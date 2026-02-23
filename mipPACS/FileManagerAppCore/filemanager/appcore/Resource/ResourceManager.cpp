#include "stdafx.h"
#include "ResourceManager.h"
#include <qfile>

namespace fm
{
	ResourceManager g_ResourceManager;

	const char* MAIN_RDO_RESOURCE_TEMPLATE =
		"QRadioButton {"
		"background-color: rgb(83,83,83);"
		"color: rgb(255, 255, 255);"
		"font: \"Malgun Gothic\" \"Arial\";"
		"font-size : 15px;"
		"}"

		"QRadioButton:hover {"
		"background-color: rgb(72,72,72);"
		"color: rgb(255,255,255);"
		"}"

		"QRadioButton:checked {"
		"background-color: rgb(65,65,65);"
		"color: %5;"
		"}"

		"QRadioButton::indicator {"
		"background-color: rgb(83,83,83);"
		"image: url(%1/%2.png);"
		"width : 40px;"
		"height: 40px;"
		"}"

		"QRadioButton::indicator:hover {"
		"background-color: rgb(72,72,72);"
		"image: url(%1/%3.png);"
		"width : 40px;"
		"height: 40px;"
		"}"

		"QRadioButton::indicator:checked {"
		"background-color: rgb(65,65,65);"
		"image: url(%1/%4.png);"
		"width : 40px;"
		"height: 40px;"
		"}";

	const char* PUSH_BUTTON_RESOURCE_TEMPLATE =
		"QPushButton{\
			border-image: url(%1/%2.png);\
			width:%6px;\
			height:%7px;\
		}\
		QPushButton:hover{\
			border-image: url(%1/%3.png);\
			width:%6px;\
			height:%7px;\
		}\
		QPushButton:disabled{\
			border-image: url(%1/%4.png);\
			width:%6px;\
			height:%7px;\
		}\
		QPushButton:hover:pressed {\
			border-image: url(%1/%5.png);\
			width:%6px;\
			height:%7px;\
		}";

	ResourceManager::ResourceManager()
	{
		m_resourceDirpath_DeepCatch = ":/Resource/Images/deepcatch";
		m_resourceDirpath_Medip = ":/Resource/Images/medip";

		m_resourceDirpath = m_resourceDirpath_Medip;
		//m_resourceDirpath = m_resourceDirpath_DeepCatch;

		m_checkTextColor_DeepCatch = "rgb(165,79,153)";
		m_checkTextColor_Medip = "rgb(34,131,176)";

		m_checkTextColor = m_checkTextColor_Medip;
	}

	QString ResourceManager::GetMainStyleSheet(EProductType license)
	{
		/* Stylesheet Àû¿ë*/
		QString stylesheetPath;
		if (license == EProductType::DEEPCATCH)
		{
			stylesheetPath = ":/Resource/deepcatch_stylesheet.qss";
			m_resourceDirpath = m_resourceDirpath_DeepCatch;
			m_checkTextColor = m_checkTextColor_DeepCatch;
		}
		else
		{
			stylesheetPath = ":/Resource/default.qss";
			m_resourceDirpath = m_resourceDirpath_Medip;
			m_checkTextColor = m_checkTextColor_Medip;
		}

		QFile file(stylesheetPath);
		if (file.open(QFile::ReadOnly))
		{
			return file.readAll();
		}
		else
		{
			qWarning() << "fail to read stylesheet : " << stylesheetPath;
			return "";
		}
	}

	QString ResourceManager::GetRadioNavTabProject()
	{
		return GetRadioNavTab("folder_d", "folder_h", "folder_p");
	}

	QString ResourceManager::GetRadioNavTabDicom()
	{
		return GetRadioNavTab("dicom_d", "dicom_h", "dicom_p");
	}

	QString ResourceManager::GetRadioNavTabImport()
	{
		return GetRadioNavTab("import_file_d", "import_file_h", "import_file_p");
	}

	QString ResourceManager::GetRadioNavTabSave()
	{
		return GetRadioNavTab("folder_d", "folder_h", "folder_p");
	}

	QString ResourceManager::GetRadioNavTabPatientList()
	{
		return GetRadioNavTab("patient_list_d", "patient_list_h", "patient_list_p");
	}

	QString ResourceManager::GetRadioNavTabDownload()
	{
		return GetRadioNavTab("pacs_down_d", "pacs_down_h", "pacs_down_p");
	}

	QString ResourceManager::GetRadioNavTabUpload()
	{
		return GetRadioNavTab("pacs_upload_d", "pacs_upload_h", "pacs_upload_p");
	}

	QString ResourceManager::GetRadioNavTabSearch()
	{
		return GetRadioNavTab("pacs_search_d", "pacs_search_h", "pacs_search_p");
	}

	QString ResourceManager::GetRadioNavTabSetting()
	{
		return GetRadioNavTab("setting_d", "setting_h", "setting_p");
	}


	QString ResourceManager::GetRadioNavTab(QString normal, QString hover, QString checked)
	{
		return QString(MAIN_RDO_RESOURCE_TEMPLATE).
			arg(m_resourceDirpath).
			arg(normal).
			arg(hover).
			arg(checked).
			arg(m_checkTextColor);
	}
	QString ResourceManager::GetButtonCancel()
	{
		return GetPushButton("cancel_d", "cancel_h", "cancel_d", "cancel_p", 102, 32);
	}

	QString ResourceManager::GetButtonSelectFolder()
	{
		return GetPushButton("select_folder_d", "select_folder_h", "select_folder_d", "select_folder_p", 102, 32);
	}

	QString ResourceManager::GetButtonOpen()
	{
		return GetPushButton("open_d", "open_h", "open_d", "open_p", 102, 32);
	}

	QString ResourceManager::GetButtonSave()
	{
		return GetPushButton("save_d", "save_h", "save_d", "save_p", 102, 32);
	}

	QString ResourceManager::GetButtonUpload()
	{
		return GetPushButton("upload_d", "upload_h", "upload_d", "upload_p", 102, 32);
	}

	QString ResourceManager::GetButtonImportDicom()
	{
		return GetPushButton("import_dicom_d", "import_dicom_h", "import_dicom_d", "import_dicom_p", 203, 32);
	}

	QString ResourceManager::GetButtonPrev()
	{
		return GetPushButton("prev_d", "prev_h", "prev_disabled", "prev_p", 32, 32);
	}

	QString ResourceManager::GetButtonNext()
	{
		return GetPushButton("next_d", "next_h", "next_disabled", "next_p", 32, 32);
	}

	QString ResourceManager::GetButtonParent()
	{
		return GetPushButton("parent_d", "parent_h", "parent_d", "parent_p", 32, 32);
	}

	QString ResourceManager::GetButtonSearch()
	{
		return GetPushButton("search_d", "search_h", "search_d", "search_p", 32, 32);
	}

	QString ResourceManager::GetButtonDownload()
	{
		return GetPushButton("download_d", "download_h", "download_disabled", "download_p", 32, 32);
	}

	QString ResourceManager::GetButtonPopupAdd()
	{
		return GetPushButton("popup_new_d", "popup_new_h", "popup_new_d", "popup_new_p", 32, 32);
	}
	QString ResourceManager::GetButtonPopupEdit()
	{
		return GetPushButton("popup_modify_d", "popup_modify_h", "popup_modify_disabled", "popup_modify_p", 32, 32);
	}
	QString ResourceManager::GetButtonPopupDelete()
	{
		return GetPushButton("popup_delet_d", "popup_delet_h", "popup_delet_disabled", "popup_delet_p", 32, 32);
	}

	QString ResourceManager::GetPushButton(QString normal, QString hover, QString disabled, QString pressed, int width, int height)
	{
		QString text = QString(PUSH_BUTTON_RESOURCE_TEMPLATE).
			arg(m_resourceDirpath).
			arg(normal).
			arg(hover).
			arg(disabled).
			arg(pressed).
			arg(width).
			arg(height);

		return text;
	}

}
