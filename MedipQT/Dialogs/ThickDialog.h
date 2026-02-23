#pragma once

#ifndef THICK_DLG_H
#define THICK_DLG_H

#include <qdialog.h>

class AnalMPRPlaneView;
class QLineEdit;
class QComboBox;
class ThickDialog : public QDialog
{
	Q_OBJECT

public:
	ThickDialog(float _thick, AnalMPRPlaneView* view);

	float	getThickness() { return m_thick/10; }


private slots:
	void	OnEditFinished();
	void	OnComboChanged(int index);
	void	OnApply();

private:
	AnalMPRPlaneView*	m_view;
	float			m_thick;
	QLineEdit*		m_edit;
	float			m_maxThick;
	QComboBox*		m_comboUnit;
};
#endif
