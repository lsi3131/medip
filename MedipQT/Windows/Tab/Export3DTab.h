#pragma once

#ifndef EXPORT3DTAB_H
#define EXPORT3DTAB_H

#include "defineMEDIP.h"
#include "CollapseWidget.h"
#include<QSlider>
#include<QLineEdit>
#include<QCheckBox>

#include "DataContext.h"

class Export3DTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit Export3DTab(QWidget* parent = NULL);

	QSlider* m_slideSmooth;
	QLineEdit* m_textSmooth;
	QCheckBox* m_chkbox;

	DataContext* m_pDataContext;

public:
	void		Init(DataContext* pDataContext);

	bool		isExtract();

protected:
	bool		eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void		slot_OnTextChanged(const QString& txt);
	void		slot_OnTextEditFinished();
	void		slot_OnSliderMoved(int val);

	void		slot_OnCreatesurface();
	void		slot_OnExportSTL();
	void		slot_OnExportOBJ();
	void		slot_OnExportVTK();
	void		slot_OnExport3MF();

};
#endif