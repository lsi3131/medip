#pragma once

#include "filemanager/define.h"
#include <QString>

namespace fm
{
	class ResourceManager
	{
	public:
		ResourceManager();
	public:
		QString GetMainStyleSheet(EProductType license);

		QString GetRadioNavTabProject();
		QString GetRadioNavTabDicom();
		QString GetRadioNavTabImport();
		QString GetRadioNavTabSave();
		QString GetRadioNavTabPatientList();
		QString GetRadioNavTabDownload();
		QString GetRadioNavTabUpload();
		QString GetRadioNavTabSearch();
		QString GetRadioNavTabSetting();

		QString GetRadioNavTab(QString normal, QString hover, QString checked);

		QString GetButtonCancel();
		QString GetButtonSelectFolder();
		QString GetButtonOpen();
		QString GetButtonSave();
		QString GetButtonUpload();
		QString GetButtonImportDicom();

		QString GetButtonPrev();
		QString GetButtonNext();
		QString GetButtonParent();

		QString GetButtonSearch();
		QString GetButtonDownload();

		QString GetButtonPopupAdd();
		QString GetButtonPopupEdit();
		QString GetButtonPopupDelete();

		QString GetPushButton(QString normal, QString hover, QString disabled, QString pressed, int width, int height);

	private:
		QString m_resourceDirpath;
		QString m_resourceDirpath_DeepCatch;
		QString m_resourceDirpath_Medip;

		QString m_checkTextColor;
		QString m_checkTextColor_DeepCatch;
		QString m_checkTextColor_Medip;
	};

	extern ResourceManager g_ResourceManager;
}
