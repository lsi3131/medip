#include "stdafx.h"
#include "define.h"
#include "OpenSourceLicenseDlg.h"
#include "ActionManager.h"
#include "ActionMaskListsImageCalculate.h"
#include "ShortcutManager.h"
#include "windowManager.h"
#include "StyleManager.h"
#include "UI/MaskList.h"
#include "Windows/Tabwindow.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/VolumeCalculator.h"
#include <qlayout>
#include <qradiobutton>

//=======================================
//		OpenSourceLicenseDlg
//=======================================
OpenSourceLicenseDlg::OpenSourceLicenseDlg(QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	setWindowTitle("Open Source Software License Notice");
	setStyleSheet(QString(
		"QWidget {"
		"color: white;"
		"font-family:Arial;"
		"background: rgba(82, 82, 82, 255); "
		"}"
	));

#ifdef DEEP_CATCH_VER
	QFile file(":/License_DeepCatch.html");
#else
	QFile file(":/License.html");
#endif
	if (file.open(QIODevice::ReadOnly))
	{
		QString html = QString::fromUtf8(file.readAll());
		m_textBrowser->setHtml(html);
		m_textBrowser->setOpenExternalLinks(true);
	}

	m_textBrowser->setReadOnly(true);
}


OpenSourceLicenseDlg::~OpenSourceLicenseDlg()
{
}
