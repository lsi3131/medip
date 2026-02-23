#include "stdafx.h"
#include "ConsoleMessageBox.h"
#include <cstdio>

QMessageBox::StandardButton ConsoleMessageBox::warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
	std::cout << text.toStdString() << std::endl;
	return QMessageBox::StandardButton::Ok;
}
