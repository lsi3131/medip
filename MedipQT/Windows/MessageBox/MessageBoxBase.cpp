#include "stdafx.h"
#include "MessageBoxBase.h"
#include "GUIMessageBox.h"
#include "ConsoleMessageBox.h"

std::unique_ptr<MessageBoxBase> MessageBoxBase::NewForGUI()
{
	return std::make_unique<GUIMessageBox>();
}

std::unique_ptr<MessageBoxBase> MessageBoxBase::NewForConsole()
{
	return std::make_unique<ConsoleMessageBox>();
}
