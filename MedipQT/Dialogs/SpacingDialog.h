#ifndef SPACING_DLG_H
#define SPACING_DLG_H

#include "define.h"
#include <qdialog.h>
#include <qcheckbox.h>

class QLineEdit;

class SpacingDialog : public QDialog
{
	Q_OBJECT

public:
	SpacingDialog(double m_x, double m_y, double m_z, QWidget *parent);
	virtual ~SpacingDialog();

	
	void getSpacing(double spacing[3]);
	void OnIsotropicCheck(bool val);
	bool isAccept() { return isOK; }
	bool isIsotro() { return enableIsotro; }

private:
	QLineEdit *x_edit;
	QLineEdit *y_edit;
	QLineEdit *z_edit;
	QCheckBox *chkbox;
	double m_x_original_spacing;
	double m_y_original_spacing;
	double m_z_original_spacing;

	double* m_spacing;

	double m_x_spacing;
	double m_y_spacing;
	double m_z_spacing;
	bool isOK;
	bool enableIsotro;
	private slots:
	void OnOK();

};
#endif
















