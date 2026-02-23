#pragma once

#ifndef HCCPREDICTION_DLG_H
#define HCCPREDICTION_DLG_H

#include "define.h"
#include <QDialog>

class QLineEdit;
class QSlider;
class QComboBox;
class QCheckBox;
class QRadioButton;
class QButtonGroup;
class VOLUME_DATA;

#define HCC_PREDICTION_DLG
#define USE_HCC_PREDICTION

#define HCC_DLG_WIDTH 640
#define HCC_DLG_HEIGHT 480
#define DLG_TITLE ("Hepatocellular Carcinoma (HCC) Risk Prediction")

class HCCPredictionDlg : public QDialog
{
	Q_OBJECT

public:
	HCCPredictionDlg(VOLUME_DATA* pVolumeData, QWidget* parent = nullptr);
	~HCCPredictionDlg();

#ifdef HCC_PREDICTION_DLG
public:
	void setAge(float val) { m_fAge = val; }
	void setPlatelet(float val)	 { m_fPlatelet = val; }
	void setAlbumin(float val)	 { m_fAlbumin = val; }
	void setBilirubin(float val) { m_fBilirubin = val; }
	void setALT(float val)  { m_fALT = val; }
	void setHBV(float val)  { m_fHBV = val; }
	void setGender(int val) { m_bGender = val; }
	void setAgent(int val)	{ m_bAntivirals = val; }
	void setCirr(int val)	{ m_bCirrhosis = val; }
	void setHbeag(int val)	{ m_bPresence = val; }

	float getAge() { return m_fAge; }
	float getPlatelet()  { return m_fPlatelet; }
	float getAlbumin()	 { return m_fAlbumin; }
	float getBilirubin() { return m_fBilirubin; }
	float getALT()  { return m_fALT; }
	float getHBV()  { return m_fHBV; }
	int getGender() { return m_bGender; }
	int getAgent()	{ return m_bAntivirals; }
	int getCirr()	{ return m_bCirrhosis; }
	int getHbeag()	{ return m_bPresence; }
	
public slots:
	//void OnOK(void);
	void OnClear(void);
	void OnPrediction(void);

	void OnLineEditAge(void);
	void OnLineEditPlatelet(void);
	void OnLineEditAlbumin(void);
	void OnLineEditBilirubin(void);
	void OnLineEditALT(void);
	void OnLineEditHBV(void);

	void OnRadioGender(int idx);
	void OnRadioAntivirals(int idx);
	void OnRadioCirrhosis(int idx);
	void OnRadioPresence(int idx);

private:
	void _initUI();
	void _OnLineEdit(QLineEdit* _pLineEdit, float & output);
	void _getLiverSpleenValue(float & fLiver, float & fSpleen);
	template<typename T>
	void _createMaskPtBit(std::vector<T*> & _maskPtList, std::vector<T> & _maskVitList, int* MaskMap, int Max);

private:
	//
	QPushButton* m_pBtnOK = nullptr;
	QPushButton* m_pBtnClear = nullptr;
	QPushButton* m_pBtnPrediction = nullptr;

	//Gender
	QRadioButton* m_radioGenderMale = nullptr;
	QRadioButton* m_radioGenderFeMale = nullptr;
	//Antivirals agent
	QRadioButton* m_radioEnteCavir = nullptr;
	QRadioButton* m_radioTenofovir = nullptr;
	//Cirrhosis, baseline
	QRadioButton* m_radioCirrhosisYes = nullptr;
	QRadioButton* m_radioCirrhosisNo = nullptr;
	//Presence of HBeAg, baseline
	QRadioButton* m_radioPresenceYes = nullptr;
	QRadioButton* m_radioPresenceNo = nullptr;

	QButtonGroup *m_btnGroupGender = nullptr;
	QButtonGroup *m_btnGroupAntivirals = nullptr;
	QButtonGroup *m_btnGroupCirrhosis = nullptr;
	QButtonGroup *m_btnGroupPresence = nullptr;

	//
	QLineEdit*	m_LineEditAge = nullptr;
	QLineEdit*	m_LineEditPlatelet = nullptr;
	QLineEdit*	m_LineEditAlbumin = nullptr;
	QLineEdit*	m_LineEditBilirubin = nullptr;
	QLineEdit*	m_LineEditALT = nullptr;
	QLineEdit*	m_LineEditHBV = nullptr;

	QVBoxLayout *pVLayout = nullptr;

private:
	QWidget* MedipChartWidget	= nullptr;
	VOLUME_DATA *m_pVolumeData	= nullptr;

private:
	float	m_fAge = 0.0f;
	float	m_fPlatelet = 0.0f;
	float	m_fAlbumin = 0.0f;
	float	m_fBilirubin = 0.0f;
	float	m_fALT = 0.0f;
	float	m_fHBV = 0.0f;

	int	m_bGender = 0;	// 1 - Male / 0 - Femal
	int	m_bAntivirals = 0;	// 1 - Entecavir / 0 - Tenofovir
	int	m_bCirrhosis = 0;	// 1 - Yes / 0 - No
	int	m_bPresence = 0;	// 1 - Yes / 0 - No

#endif
};

#endif