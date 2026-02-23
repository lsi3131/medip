#pragma once

#ifndef OPENSOURCE_LICENSE_DLG_H
#define OPENSOURCE_LICENSE_DLG_H

#include <qdialog>
#include "ui_OpenSourceLicenseDlg.h"

class OpenSourceLicenseDlg : public QDialog, public Ui::OpenSourceLicenseDlg
{
	Q_OBJECT

public:
	OpenSourceLicenseDlg(QWidget* parent = nullptr);
	~OpenSourceLicenseDlg();

private slots:

};
#endif