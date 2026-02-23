#pragma once

#ifndef PRESET_DLG_H
#define PRESET_DLG_H

#include "define.h"
#include <qdialog.h>
#include <qstring.h>

class QLineEdit;
class AlphaColorMap;

class PresetDialog : public QDialog
{
	Q_OBJECT

public:
	PresetDialog(int index, bool type, QWidget *parent=NULL);

protected:
	bool eventFilter(QObject *, QEvent *) override;


public:
	QString	getTypeName() { return m_name; }

private:
	int				m_index;	/*preset index*/
	bool			m_type;		/*dialog type : default(false)_save, true_edit*/
	QLineEdit		*m_typeName;
	QLineEdit		*m_2DWW;
	QLineEdit		*m_2DWL;
	QLineEdit		*m_3DWW;
	QLineEdit		*m_3DWL;
	AlphaColorMap	*m_2DColor;
	AlphaColorMap	*m_3DColor;
	QPushButton		*m_save = nullptr;	/*save or edit btn*/
	QString			m_name;
private slots:
	void	OnSave();
	void	OnCancel();
};
#endif
