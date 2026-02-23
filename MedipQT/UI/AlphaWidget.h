#pragma once

#ifndef ALPHA_WIDGET_H
#define ALPHA_WIDGET_H

#include "define.h"
#include <QWidget>
#include <QSlider>
#include <QSpinBox>

class VOLUME_DATA;

class AlphaWidget : public QWidget
{
	Q_OBJECT
public:
	AlphaWidget(VOLUME_DATA* pVolumeData, muint32 uid, QWidget* parent = NULL);


protected:
	bool eventFilter(QObject* watched, QEvent* evt) override;

private slots:
	void slot_OnSliderMoved(int pos);
	void slot_OnSliderPress();
	void slot_OnSliderRelease();


	void slot_OnSpinChanged(int val);


private:
	VOLUME_DATA* m_pVolumeData;
	QSlider* m_slider;
	QSpinBox* m_spinbox;
	bool m_press;
	muint32 m_uid;
	muint8 m_alpha;
};


#endif