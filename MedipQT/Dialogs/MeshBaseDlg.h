#pragma once

#ifndef MESHBASE_DLG_H
#define MESHBASE_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "System/resourceManager.h"
#include <QDialog>

#include "DataContext.h"

class QLineEdit;
class QSlider;
class QComboBox;
class QCheckBox;

class MeshBaseDlg : public QDialog
{
	Q_OBJECT
	
protected:
	enum enValueType
	{
		emLE_INT,
		emLE_DOUBLE,
		emLE_TEXT
	};

	enum enBtnIconType
	{
		emBTN_NORMAL,
		emBTN_M_OVER,
		emBTN_M_DOWN,
	};

	struct _Slider
	{
	public:
		_Slider() :
			pSlidier(nullptr), type(emLE_INT), min(0.0), max(100.0), initVal(50.0) {}
		_Slider(QSlider* _Sldier, enValueType _type = emLE_INT, double _min = 0.0, double _max = 100.0, double _initVal = 50.0) :
			pSlidier(nullptr), type(emLE_INT), min(0.0), max(100.0), initVal(50.0) {}

		void operator = (const _Slider& vValue)
		{
			this->pSlidier = vValue.pSlidier;
			this->type = vValue.type;
			this->min = vValue.min;
			this->max = vValue.max;
			this->initVal = vValue.initVal;
		};

		void init()
		{
			this->pSlidier = nullptr;
			this->type = emLE_INT;
			this->min = 0.0;
			this->max = 100.0;
			this->initVal = 50.0;
		};

		void setValue(QSlider* _Sldier = nullptr, enValueType _type = emLE_INT, double _min = 0.0, double _max = 100.0, double _initVal = 50.0)
		{
			this->pSlidier = _Sldier;
			this->type = _type;
			this->min = _min;
			this->max = _max;
			this->initVal = _initVal;
		};

	public:
		QSlider* pSlidier;
		enValueType type;
		double min;
		double max;
		double initVal;
	};

	struct _Button
	{
	public:
		_Button() :
			pButton(nullptr) {	RICON[0] = ICON_CLEAR; RICON[1] = ICON_CLEAR; RICON[2] = ICON_CLEAR; }
		_Button(QPushButton* _btn, int _icNormal, int _MOver, int _Down) :
			pButton(_btn) {	RICON[0] = _icNormal; RICON[1] = _MOver; RICON[2] = _Down; }

		void operator = (const _Button& vValue)
		{
			this->pButton  = vValue.pButton;
			this->RICON[0] = vValue.RICON[0];
			this->RICON[1] = vValue.RICON[1];
			this->RICON[2] = vValue.RICON[2];
		};
		void setValue(QPushButton* _btn, int _icNormal, int _MOver, int _Down)
		{
			this->pButton  = _btn;
			this->RICON[0] = _icNormal;
			this->RICON[1] = _MOver;
			this->RICON[2] = _Down;
		};
	public:
		QPushButton* pButton;
		int RICON[3]; // [0] - normal, [1] - mouseOver, [2] mouseDown
	};
public:	
	MeshBaseDlg(DataContext* pDataContext, QWidget *parent = NULL);
	MeshBaseDlg(DataContext* pDataContext, QString _title, QWidget *parent = NULL);
	virtual ~MeshBaseDlg();

	bool initTopology(void);
	void setTopology(mip::MeshTopology* pMT) { m_pMT = pMT; };
	mip::MeshTopology*	getTopology() { return m_pMT; };

public:
	virtual void reject(bool bForce = false);

protected:
	QLineEdit* initLineEdit(QWidget* parent, QString objectName, int _ScalarType = emLE_INT, double min = 0, double max = 100, double initVal = 1.0, int demical = 0, int fixWidth = 0, int fixHeight = 0);
	void initSlider(QWidget* parent, QString ObjName, _Slider& _out, int Orientation = Qt::Horizontal, int _ScalarType = emLE_INT, double min = 0, double max = 100, double initVal = 50, double fixWidth = 0, double fixHeight = 0, int TickInterval = 1);
	void setBtnIcon(QPushButton* pBtn = nullptr, int iConType = emBTN_NORMAL, bool checked = true);
	void changePressedBtnIcon();

	void setSliderValue(QString name, float value);
	void setLineEditValue(QString name, float value);

protected:
	bool eventFilter(QObject* target, QEvent* e) override;

protected slots:
	void OnReleased();

protected:
	std::vector<QLineEdit*> listLE;
	std::vector<QComboBox*> listCB;
	std::vector<_Slider>	 listSd;
	std::vector<_Button> listBtn;

	mip::MeshTopology* m_pMT;

	QPushButton* HoverWidget;

	DataContext* m_pDataContext;
};
#endif