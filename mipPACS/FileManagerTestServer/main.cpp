#include "stdafx.h"
#include <QtWidgets/QApplication>
#include <qmessagebox>
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/FileManagerAppCore.h"
#include "filemanager/appcore/Resource/StringManager.h"
#include "TestServerContext.h"
#include "MainWindow.h"
#include "main.h"

//using namespace fm;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//QIcon icon(":/Resource/logo2.png");

	if (g_testServerContext.Initialize() == false)
	{
		return 0;
	}

	if (g_testServerContext.GetServer()->StartServer() == false)
	{
		QMessageBox::critical(
			nullptr,
			fm::StringManager::GetString(STR_TITLE_ERROR),
			fm::StringManager::GetString(STR_ERR_INIT_ROOT_OBJECT),
			QMessageBox::StandardButton::Ok);
		return 0;
	}

	qInfo() << "Welcome Test Server";

	MainWindow w;
	//w.setWindowIcon(icon);
    w.show();
    int result = a.exec();

	qInfo() << "Good Bye";

	fm::Logger::Deinitialize();

	return result;
}
