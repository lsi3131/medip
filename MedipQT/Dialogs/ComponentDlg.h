#ifndef COMPONENTDLG_H
#define COMPONENTDLG_H

#include <QDialog>

class ComponentDlg : public QDialog
{
	Q_OBJECT

public:
	ComponentDlg(QWidget* parent = nullptr );
	~ComponentDlg();

	int getClusterNum(void);

private:

	QLineEdit* m_LineEditNumber = nullptr;

	int m_nClusterNum = 0;

public slots :
	void OnOk(void);
	void OnCancel(void);
	
};

#endif // COMPONENTDLG_H
