#include "stdafx.h"
#include "GUI_Initializer.h"
#include <qapplication.h>

GUI_Initializer::GUI_Initializer()
{
	int argc = 1;
	char* argv[] = { "null" };
	m_app = new QApplication(argc, argv);
}

GUI_Initializer::~GUI_Initializer()
{
	delete m_app;
}
