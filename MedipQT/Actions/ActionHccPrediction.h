#pragma once

#ifndef ACTION_HCCPREDICTION_H
#define ACTION_HCCPREDICTION_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>


class WorkHccPrediction : public QObject
{
	Q_OBJECT

public:
	WorkHccPrediction(
		const float& age,			// "Age" from UI;
		const float& gender,		// "Gender" from UI; NOTE: (Male: 1.f, Female: 0.f)
		const float& agent,			// "Antivirals agent" from UI; NOTE: (Entecavir: 1.f, Tenofovir: 0.f)
		const float& cirr,			// "Cirrhosis, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& hbeag,			// "Presence of HBeAg, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		const float& plt,			// "Platelet, baseline" from UI;
		const float& alb,			// "Albumin, baseline" from UI;
		const float& tb,			// "Total bilirubin, baseline" from UI;
		const float& alt,			// "ALT, baseline" from UI;
		const float& hbv,			// "HBV DNA, baseline" from UI;
		const float& liver,			// "Liver Volume" from DeepCatch;
		const float& spleen
	);
	

public:
	void setProgressValue(int value, bool init = false);

	void predict();

public slots:
	void threadRun();
signals:
	void progress(int);
	void finished();	
	
private:
	float m_age;
	float m_gender;
	float m_agent;
	float m_cirr;
	float m_hbeag;
	float m_plt;
	float m_alb;
	float m_tb;
	float m_alt;
	float m_hbv;
	float m_liver;
	float m_spleen;
};
#endif
