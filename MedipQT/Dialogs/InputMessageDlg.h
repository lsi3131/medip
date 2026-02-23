#ifndef COMPONENTDLG_H
#define COMPONENTDLG_H

#include <QDialog>

class InputMessageDlg : public QDialog
{
	Q_OBJECT

public:
	InputMessageDlg(QString title, QString inputName, QWidget* parent = nullptr );
	~InputMessageDlg();

	int getInputNum(void);

private:

	QLineEdit* m_LineEditNumber = nullptr;

	int m_nInputNum = 0;

public slots :
	void OnOk(void);
	void OnCancel(void);
	
};

#endif // COMPONENTDLG_H
