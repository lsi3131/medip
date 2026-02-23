#pragma once

#ifndef SIZE_DLG_H
#define SIZE_DLG_H

#include "define.h"
#include <qdialog.h>
#include <qcolor.h>

class QLineEdit;

class SizeDialog : public QDialog
{
	Q_OBJECT

public:
	SizeDialog(QString title, int type = 0, QWidget* parent = NULL);

	float	getWidth();
	float	getHeight();
	float	getDepth();
	int		getResolution(int _index = 0);
	QString getName();
private:
	QLineEdit* m_lineWidth;
	QLineEdit* m_lineHeight;
	QLineEdit* m_lineDepth;
	QLineEdit* m_lineResol[2];

	QLineEdit* m_lineName;

	int m_type;
	static int mResol[3]; //0:cylinder 1,2 : sphere
	static float mSize[3][3]; //0:cube, 1:cylinder 2: sphere

private slots:
	void OnWidthEditFinish();
	void OnHeightEditFinish();
	void OnDepthEditFinish();
	void OnResolEditFinish();
	void OnResol2EditFinish();

	void OnCreate();
};
#endif