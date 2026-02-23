#pragma once

#include "Windows/MessageBox/MessageBoxBase.h"

class GUIMessageBox : public MessageBoxBase
{
public:
	virtual QMessageBox::StandardButton warning(
		QWidget* parent,
		const QString& title,
		const QString& text,
		QMessageBox::StandardButtons buttons = QMessageBox::Ok) override;

};
