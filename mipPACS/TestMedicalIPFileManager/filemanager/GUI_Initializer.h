#pragma once

/*
	Qt GUI, Widget을 테스트하기 위해서 사용하는 class
	QApplication을 호출한다.
*/

class QApplication;

class GUI_Initializer
{
public:
	GUI_Initializer();
	~GUI_Initializer();

private:
	QApplication* m_app;
};
