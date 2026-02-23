#pragma once

#ifndef NUM_DLG_H
#define NUM_DLG_H

#include "define.h"
#include <qdialog.h>
#include <qstring.h>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QButtonGroup>

class QLineEdit;


class NumDialog : public QDialog
{
	Q_OBJECT

public:
	NumDialog(QWidget* parent = NULL, int based = 1000);

	int m_x_diameter_min;
	int m_x_diameter_max;
	int m_y_diameter_min;
	int m_y_diameter_max;
	int m_z_diameter_min;
	int m_z_diameter_max;
	int m_number_of_point;
	int m_mean_min_HU;
	int m_mean_max_HU;
	int m_boundary_min_HU;
	int m_boundary_max_HU;	
	bool enablePointOnly;
	bool cavityMode;
	int m_cavity_boundary;

	void setTitle(QString & title);

	void	getNum();
	

	private slots:
	void	OnNumChanged();
	void	OnAccept();
	void	OnPointOnlyCheck(bool val);
	void	OnChangeMode(int type);
	

private:
	QLineEdit*	m_editNum;
	QLineEdit*	diameter_x_min_edit;
	QLineEdit*	diameter_x_max_edit;
	QLineEdit*	diameter_y_min_edit;
	QLineEdit*	diameter_y_max_edit;
	QLineEdit*	diameter_z_min_edit;
	QLineEdit*	diameter_z_max_edit;
	QLineEdit*	mean_HU_min_edit;
	QLineEdit*	mean_HU_max_edit;
	QLineEdit*	boundary_HU_min_edit;
	QLineEdit*	boundary_HU_max_edit;
	QLineEdit*	cavity_boundary_edit;

	QButtonGroup *m_maskGroup;
	QCheckBox *chkbox;

// 	int			m_bottom;
// 	int			m_top;

};
#endif