#pragma once

#ifndef CAPTURETAB_H
#define CAPTURETAB_H

#include "CollapseWidget.h"
#include <QComboBox>
#include <QButtonGroup>
#include <QPushButton>

class CaptureTab : public CollapseWidget
{
	Q_OBJECT

public:
	CaptureTab(QWidget* parent = NULL);

	void	cancelCapture();
	void	CaptureFull(bool save = true);

	bool	getCaptureType() { return m_type; }/*default(false) : data & roi, true : false + coord + annotation +...etc*/
	bool	getCaptureMode() { return m_mode; }/*default(false) : Rectangle, true : specific window*/

public slots:
	void	slot_OnCapture();
	void	slot_OnTypeChanged(int);
	void	slot_OnComboChanged(int);

private:
	void	Capture4Windows();

private:
	QComboBox* m_comboMode;
	QButtonGroup* m_groupType;

	QPushButton* m_btnCapture;

	bool		m_type;	/*default(false) : data & roi, true : false + coord + annotation +...etc*/
	bool		m_mode;		/*default(false) : Rectangle, true : specific window*/
};
#endif