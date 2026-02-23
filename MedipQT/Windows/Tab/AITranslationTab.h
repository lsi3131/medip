#pragma once

#ifndef AITRANSLATIONTAB_H
#define AITRANSLATIONTAB_H

#include "CollapseWidget.h"
#include <QComboBox>
#include <QButtonGroup>

class AITranslationTab : public CollapseWidget
{
	Q_OBJECT

public:
	explicit AITranslationTab(QWidget* parent = NULL);
	AITranslationTab* m_tabSet;
	int		getAIType();
	void	LoadProject(bool reload = false);
signals:
	void	sig_disablePredictBtn(bool visible);
	void	sig_checkedPredictBtn(bool visible);

protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

private:
	QComboBox* m_ComboProj;
	QButtonGroup* m_GrProc;
	bool		m_GPU; //GPU Use : true, CPU : false

private slots:
	void	slot_OnProcChanged(int index);
	void	slot_OnPredict();

};
#endif

