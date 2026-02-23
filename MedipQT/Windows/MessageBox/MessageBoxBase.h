#pragma once

#include <memory>
#include <QMessageBox>

class MessageBoxBase
{
public:
	static std::unique_ptr<MessageBoxBase> NewForGUI();
	static std::unique_ptr<MessageBoxBase> NewForConsole();

public:
	virtual QMessageBox::StandardButton warning(
		QWidget* parent, 
		const QString& title,
		const QString& text, 
		QMessageBox::StandardButtons buttons = QMessageBox::Ok) = 0;
};
