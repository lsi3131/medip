#ifndef INTERPOLATIONDLG_H
#define INTERPOLATIONDLG_H

#include <QDialog>

class Interpolation3dDlg : public QDialog
{
	Q_OBJECT

public:
	Interpolation3dDlg(QWidget* parent = nullptr );
	~Interpolation3dDlg();

	void setInterPolDirection(int direction);
	int getInterPolDirection(void);

private:
	void updateInterPolationDirectionByComboBox();

private:

	QLineEdit* m_LineEditNumber = nullptr;
	QComboBox * m_ComboBox = nullptr;

	int m_n3dInterpolDir = 1; //1 = Axial, 2 = Coronal, 3 = Sagittal

public slots :
	void OnOk(void);
	void OnCancel(void);
	void OnComboChanged(int);
};

#endif // INTERPOLATIONDLG_H
