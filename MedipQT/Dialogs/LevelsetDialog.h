#ifndef LEVELSET_DLG_H
#define LEVELSET_DLG_H

#include "define.h"
#include <qdialog.h>

class QLineEdit;

class LevelsetDialog : public QDialog
{
	Q_OBJECT

public:
	LevelsetDialog(QWidget *parent=NULL);
	virtual ~LevelsetDialog();

	int getIter();
	double getLambda();
	int getRadious();
	bool isAccept() { return isOK; }
private:
	QLineEdit *lineIter;
	QLineEdit *lineLam;
	QLineEdit *lineRadious;

	bool isOK;

	int iterVal;
	double lamVal;
	int radiVal;

private slots:
	void OnOK();
};
#endif
















