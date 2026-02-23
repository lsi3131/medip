#ifndef ANNO_PATHUSAGE_DLG_H
#define ANNO_PATHUSAGE_DLG_H

#include "define.h"
#include <qdialog.h>

class QLabel;

class AnnoPathUsageDlg : public QDialog
{
	Q_OBJECT

public:
	AnnoPathUsageDlg(QWidget *parent = NULL);
	virtual ~AnnoPathUsageDlg();

	bool isShowAgain() { return isShow; }

private:
	bool isShow;

private slots:
	void OnChecked(bool);
};
#endif