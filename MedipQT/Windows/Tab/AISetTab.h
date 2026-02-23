#pragma once

#ifndef AISETTAB_H
#define AISETTAB_H

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>

class VOLUME_DATA;
class ProductManager;

class AISetTab : public QGroupBox
{
	Q_OBJECT

public:
	explicit AISetTab(VOLUME_DATA* pVolumeData, ProductManager* pProductManager, QWidget* parent = NULL);

	void getWWWL(int& ww, int& wl);
	//	void getDepth(int &start, int &to);

//	void setOutName(QString filePath);
	int	getSliderOutVal();
	void setSliderOutVal(int nVal);
	void setDoubleSpinValue(double val);
	int getPreOutVal();

	void setEnableObject(bool enable);
	bool isGMMAutoApplyChecked(void);
	bool isHeatMapChecked(void);
	bool isHeatMapEnabled(void);
	void setHeatMapChecked(bool bCheck);
	//	void FreezeDepth(bool);

	// 	void setWWWL(AI_TYPE type, bool _init = true);
	// 	void setWWWL();

protected:
	bool	eventFilter(QObject* watched, QEvent* event) override;

private:
	VOLUME_DATA* m_pVolumeData;
	ProductManager* m_pProductManager;

	QLineEdit* m_editWW = nullptr;
	QLineEdit* m_editWL = nullptr;
	QCheckBox* m_syncWL = nullptr;

	QCheckBox* m_chBoxGMMautoApply;
	QPushButton* m_btnGMM;
	QCheckBox* m_chBoxColorMap;
	QDoubleSpinBox* m_editOut;
	QSlider* m_sliderOut;


	int			m_nPreSliderValue = -1;


	const int MIN_OUTPUT;
	const int MAX_OUTPUT;

private slots:
	void slot_OnWWEditFinished();
	void slot_OnWLEditFinished();
	void slot_OnStartEditFinished();
	void slot_OnToEditFinished();

	void	slot_OnLoad();

	void	slot_OnSliderMoved(int val);
	void	slot_OnOutputChanged();
	void	slot_OutsetApply();
	void	slot_OnSliderPressed(void);

	void slot_OnSyncBox(bool checked = false);
	void slot_OnSyncWL(bool checked = false);

	void slot_OnHeatMap(bool checked = false);
	void slot_OnGMM(void);
public:
	Qt::CheckState getColorMapState(void);

};
#endif

