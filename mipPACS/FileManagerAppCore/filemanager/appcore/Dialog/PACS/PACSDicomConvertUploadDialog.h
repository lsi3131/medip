#pragma once

#include "filemanager/appcore/UI/PACSDicomConvertUploadWidget.h"
#include <qdialog>

namespace fm
{
	class PACSDicomConvertUploadDialog : public QDialog
	{
		Q_OBJECT

	public:
		PACSDicomConvertUploadDialog(AppCoreContext* pContext, QWidget* parent);

	public:
		PACSDicomConvertUploadWidget* Widget;
	};
}
