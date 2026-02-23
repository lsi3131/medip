#include "stdafx.h"
#include "GUIMessageBox.h"

QMessageBox::StandardButton GUIMessageBox::warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
	return QMessageBox::warning(parent, title, text, buttons);
}
