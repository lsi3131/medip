#include "stdafx.h"
#include "MeshBaseDlg.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"

#include "System/styleManager.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"

#include "MeshEdit\CMeshWorkManager.h"
#include "DataContext.h"

MeshBaseDlg::MeshBaseDlg(DataContext* pDataContext, QWidget *parent /*= NULL*/) : 
	QDialog(parent, Qt::FramelessWindowHint), 
	m_pMT(nullptr), 
	HoverWidget(nullptr),
	m_pDataContext(pDataContext)
{
	setWindowTitle(QString("None"));
}

MeshBaseDlg::MeshBaseDlg(DataContext* pDataContext, QString _title, QWidget *parent /*= NULL*/) : 
	QDialog(parent, Qt::FramelessWindowHint), 
	m_pMT(nullptr), 
	HoverWidget(nullptr),
	m_pDataContext(pDataContext)
{
	setWindowTitle(_title);
}

MeshBaseDlg::~MeshBaseDlg()
{
}

void MeshBaseDlg::reject(bool bForce)
{
	if (!ACTION_MANAGER->isActionFinished() && !bForce) return; //20210928_byPHS_기능수행중 Reject 안되게 수정

	QString _title = windowTitle();	
	printf_s("\n %s ::reject()", _title.toStdString().c_str());
	setWindowOpacity(0.1);
	setVisible(false);	

	MESH_WORK_MANAGER->UpdateWorkMode(MESH_WORK_NONE, true);
	close();
}

bool MeshBaseDlg::initTopology(void)
{
	//
	int MeshIdx = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	setTopology(m_pDataContext->m_MeshData.GetTempMesh(MeshIdx));

	return true;
}


QLineEdit * MeshBaseDlg::initLineEdit(QWidget * parent, QString objectName, int _ScalarType, double min, double max, double initVal, int demical, int fixWidth, int fixHeight)
{
	QLineEdit* lineEdit = new QLineEdit(parent);
	lineEdit->setStyleSheet(STYLE_MANAGER->editBoxTab);
	lineEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	QValidator* qValidator = nullptr;
	if (_ScalarType == emLE_INT)
	{
		lineEdit->setValidator(new QIntValidator(min, max, parent));
		lineEdit->setText(QString::number((int)initVal));		
	}
	else if (_ScalarType == emLE_DOUBLE)
	{
		lineEdit->setValidator(new QDoubleValidator(min, max, demical, parent));
		lineEdit->setText(QString::number(initVal, 'f', demical));
	}
	else if (_ScalarType == emLE_TEXT)
	{
		//lineEdit->setValidator(new QIntValidator(min, max, parent));
		//lineEdit->setText(QString::number((int)initVal));
		lineEdit->setText(QString(""));
	}
	
	lineEdit->setObjectName(objectName);
	lineEdit->setAlignment(Qt::AlignRight);

	if(fixWidth != 0) lineEdit->setFixedWidth(fixWidth);
	if(fixHeight != 0) lineEdit->setFixedWidth(fixHeight);
	return lineEdit;
}

//
void MeshBaseDlg::initSlider(QWidget * parent, QString ObjName,_Slider& _slider, int Orientation, int _ScalarType, double min, double max, double initVal, double fixWidth, double fixHeight, int TickInterval)
{
	QSlider* pSlider = new QSlider((Qt::Orientation)Orientation, parent);
	pSlider->setObjectName(ObjName);
	pSlider->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	pSlider->setSliderDown(true);
	pSlider->installEventFilter(this);
	pSlider->setMouseTracking(true);
	
	if (_ScalarType == emLE_INT)
	{
		pSlider->setMinimum(min);
		pSlider->setMaximum(max);
		pSlider->setValue(initVal);
	}
	else
	{
		pSlider->setMinimum(0);
		pSlider->setMaximum(100);
		pSlider->setValue((float(initVal - min) / (max - min)) * 100);
	}
	pSlider->setTickInterval(TickInterval);

	if (fixWidth != 0) pSlider->setFixedWidth(fixWidth);
	if (fixHeight != 0) pSlider->setFixedWidth(fixHeight);

	_slider.init();
	_slider.setValue(pSlider, (enValueType)_ScalarType, min, max, initVal);	
}



bool MeshBaseDlg::eventFilter(QObject * target, QEvent * e)
{
	const int normal = 0; const int MOver = 1; const int Down = 2;	
	std::vector<std::vector<QIcon>> IconDimArr;
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		std::vector<QIcon> iconList;
		iconList.push_back(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[normal], 0, 0));
		iconList.push_back(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[MOver], 0, 0));
		iconList.push_back(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[Down], 0, 0));

		IconDimArr.push_back(iconList);
	}

	if (target == NULL)
		return QWidget::eventFilter(target, e);

	if (!target->inherits("QPushButton") && !target->inherits("QToolButton"))
		return QWidget::eventFilter(target, e);

	QEvent::Type _type = e->type();
	int res = 0;
	bool press = false;

	if (_type == QEvent::HoverLeave || _type == QEvent::Leave || _type == QEvent::Show)
	{
		if (HoverWidget)
		{
			if (!HoverWidget->isChecked())
			{
				for (int ii = 0; ii < listBtn.size(); ii++)
				{
					if (listBtn[ii].pButton == HoverWidget)
					{
						HoverWidget->setIcon(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[emBTN_NORMAL], 0, 0));
						break;
					}
				}
			}
		}
	}

	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent *evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
				{
					res = 1;
					press = e->type() == QEvent::MouseButtonPress;
				}
			}
		}
	}

	if (1 != res)
	{
		QHoverEvent *evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}

	if (res == 1)
	{
		for (int ii = 0; ii < listBtn.size(); ii++)
		{
			if (target == listBtn[ii].pButton && !listBtn[ii].pButton->isChecked())
			{
				listBtn[ii].pButton->setIcon(IconDimArr[ii][MOver]);
				HoverWidget = listBtn[ii].pButton;
			}

			if (target == listBtn[ii].pButton && listBtn[ii].pButton->isChecked())
			{
				listBtn[ii].pButton->setIcon(IconDimArr[ii][Down]);
			}
		}
	}

	return false;
}

void MeshBaseDlg::changePressedBtnIcon()
{
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if(listBtn[ii].pButton->hasFocus())	
		{
			//printf_s("\n hasFocus changePressedBtnIcon() %s", listBtn[ii].pButton->objectName().toStdString().c_str());

			if (!listBtn[ii].pButton->isChecked())
			{
				listBtn[ii].pButton->setIcon(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[emBTN_M_DOWN], 0, 0));
				listBtn[ii].pButton->setChecked(true);
			}
			else
			{
				//listBtn[ii].pButton->setIcon(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[emBTN_M_OVER], 0, 0));
				//listBtn[ii].pButton->setChecked(false);
			}
		}
	}
}

void MeshBaseDlg::setBtnIcon(QPushButton* pBtn, int iConType, bool checked)
{
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if (listBtn[ii].pButton == pBtn)
		{
			listBtn[ii].pButton->setChecked(checked);
			listBtn[ii].pButton->setIcon(RESOURCE_MANAGER->getIcon((RICON)listBtn[ii].RICON[iConType], 0, 0));
		}
	}
}


void MeshBaseDlg::OnReleased()
{
	for (int ii = 0; ii < listBtn.size(); ii++)
	{
		if (listBtn[ii].pButton == HoverWidget)
		{
			if (!listBtn[ii].pButton->isChecked())
				listBtn[ii].pButton->setChecked(true);
			//else
			//	listBtn[ii].pButton->setChecked(false);
		}
	}
}

void MeshBaseDlg::setSliderValue(QString name, float value)
{
	_Slider sSlider;
	for (int ii = 0; ii < listSd.size(); ii++)
	{
		sSlider = listSd[ii];
		if (sSlider.pSlidier == nullptr) continue;
		if (name == sSlider.pSlidier->objectName()) break;
	}

	//if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
	{
		value -= sSlider.min;
		value /= (sSlider.max - sSlider.min);
		value *= 100;
	}

	if (sSlider.pSlidier != nullptr)
	{
		sSlider.pSlidier->blockSignals(true);
		sSlider.pSlidier->setValue((value));
		//sSlider.pSlidier->setSliderPosition(value);
		sSlider.pSlidier->blockSignals(false);
	}
	
}

void MeshBaseDlg::setLineEditValue(QString name, float value)
{
	//QLineEdit
	QLineEdit* pLineEdit = nullptr;
	for (int ii = 0; ii < listLE.size(); ii++)
	{
		pLineEdit = listLE[ii];
		if (pLineEdit == nullptr) continue;
		if (name == pLineEdit->objectName()) break;
	}

	if (pLineEdit != nullptr)
	{
		pLineEdit->blockSignals(true);
		pLineEdit->setText(QString::number(value));
		pLineEdit->blockSignals(false);
	}	
}
