#pragma once

#include "filemanager/appcore/SubWindow/FolderViewWidget.h"

using namespace fm;

class MockFolderViewWidget : public FolderViewWidget
{
public:
	MockFolderViewWidget(AppCoreContext* pContext, QWidget* parent = nullptr);
	virtual ~MockFolderViewWidget();

public:
	void WaitUntilMainViewDirectoryLoaded();

	QString GetDirectoryPath();
	QString GetFileName();
	QStringList GetMainViewFileNameList();

};
