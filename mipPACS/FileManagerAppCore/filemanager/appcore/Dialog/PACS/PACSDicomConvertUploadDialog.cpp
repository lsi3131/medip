#include "stdafx.h"
#include "PACSDicomConvertUploadDialog.h"
#include <qlayout>
#include <qpdfwriter.h>

namespace fm
{
	PACSDicomConvertUploadDialog::PACSDicomConvertUploadDialog(AppCoreContext* pContext, QWidget* parent) :
		QDialog(parent)
	{
		Widget = new PACSDicomConvertUploadWidget(pContext, this);
		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(0, 0, 0, 0);
	}
}
