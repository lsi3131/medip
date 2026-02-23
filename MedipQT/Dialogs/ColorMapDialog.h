#pragma once

#ifndef CCOLORMAP_DLG_H
#define CCOLORMAP_DLG_H

#include <qdialog.h>
#include <QString>

class QLabel;
class AlphaColorMap;
class HoverPoints;
class QHBoxLayout;
class QButtonGroup;
class QGroupBox;
class QCheckBox;

class ColorMapDialog : public QDialog
{
	Q_OBJECT

public:
	ColorMapDialog(int type, QWidget *parent = NULL, bool hideWL=false);
	~ColorMapDialog();

	QGradientStops getColorMapStops();
	void setColorMapStops(QGradientStops &);

	bool isChanged() { return bChanged; }
	bool isAlphaChanged() { return bAlphaChanged; }
	bool isWLApply();
protected:
	bool eventFilter(QObject*, QEvent*) override;
private:
	AlphaColorMap * colormap;
	HoverPoints *alphaPoints;
	QHBoxLayout * pointLayout;
	QGroupBox * groupBox;
	QButtonGroup * pointGroup;
	QCheckBox * includeWL;
	bool bApply;
	bool bChanged;
	bool bAlphaChanged;
	bool bWLChanged;
	int isVolume;
	int m_preset;
	int m_custom;
	QPushButton *applyBtn;
private slots:
	void OnApply();
	void OnCancel();
	void OnReset();
	void CheckPoints();
	void MovePoint(int,qreal);
	void RemovePoint(int);
	void OnChangeAlpha();
};
#endif