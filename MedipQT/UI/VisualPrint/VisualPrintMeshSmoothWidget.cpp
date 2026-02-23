#include "stdafx.h"
#include "VisualPrintMeshSmoothWidget.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "StringManager.h"
#include "ActionManager.h"
#include "styleManager.h"
#include "DataContext.h"

///////////////////////////////////////////////////////////////////////////////
// Visual Print Mesh Smooth Widget Class Member Functions - Start
////////////////////////////////////////////////////////////////////////////////
// bCreate = true : 생성시, bCreate = false : Update시
VisualPrintMeshSmoothWidget::VisualPrintMeshSmoothWidget(DataContext* pDataContext, muint32 uid, bool bCreate, QWidget* parent /*= NULL*/) :
	QWidget(parent),
	m_pDataContext(pDataContext)
{
	setAutoFillBackground(true);

	m_muint32MeshInfoUID = uid;

	QHBoxLayout* layMain = new QHBoxLayout;
	QVBoxLayout* btnLayout = new QVBoxLayout;
	QVBoxLayout* sliLayout = new QVBoxLayout;

	layMain->addLayout(btnLayout);
	layMain->addLayout(sliLayout);

	/////////////////// Smooth UI - Start //////////////////////////////
	m_labelMeshSmooth = new QLabel(QString("Smooth  0"), this);
	m_labelMeshSmooth->setFixedWidth(75);
	m_labelMeshSmooth->setFixedHeight(10);

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo) {
		if (bCreate)
		{
			pMeshInfo->m_nSmoothLevel = 0;
			m_nSmoothLevel = 0;

			pMeshInfo->m_nReduceLevel = 0;
			m_nReductionLevel = 0;
		}
		else
		{
			m_nSmoothLevel = pMeshInfo->m_nSmoothLevel;
			m_nReductionLevel = pMeshInfo->m_nReduceLevel;
		}
	}
	else
	{
		m_nSmoothLevel = 0;
		m_nReductionLevel = 0;
	}

	btnLayout->addWidget(m_labelMeshSmooth);

	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setFixedHeight(20);
	m_slider->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_slider->setObjectName("SliderAlpha");
	m_slider->setRange(0, 100);
	// m_slider->setPageStep(1);
	// m_slider->setSingleStep(1);
	m_slider->setValue(m_nSmoothLevel);
	m_slider->setMouseTracking(true);
	m_slider->installEventFilter(this);
	m_press = false;
	connect(m_slider, &QSlider::sliderPressed, this, &VisualPrintMeshSmoothWidget::slot_OnSliderPress);
	connect(m_slider, &QSlider::valueChanged, this, &VisualPrintMeshSmoothWidget::slot_OnSliderMoved);
	connect(m_slider, &QSlider::sliderReleased, this, &VisualPrintMeshSmoothWidget::slot_OnSliderRelease);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	sliLayout->addWidget(m_slider);
	sliLayout->addWidget(emptyBox0);
	/////////////////// Smooth UI - End ////////////////////////////////////////////////////////////////


	/////////////////// Reduction UI - Start //////////////////////////////
	m_labelMeshReduce = new QLabel(QString("Reduce  0"), this);
	m_labelMeshReduce->setFixedWidth(75);
	m_labelMeshReduce->setFixedHeight(10);

	m_nReductionLevel = 0;

	btnLayout->addWidget(m_labelMeshReduce);

	m_sliderReduction = new QSlider(Qt::Horizontal, this);
	m_sliderReduction->setFixedHeight(20);
	m_sliderReduction->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_sliderReduction->setObjectName("SliderReduce");
	m_sliderReduction->setRange(0, 100);
	// m_slider->setPageStep(1);
	// m_slider->setSingleStep(1);
	m_sliderReduction->setValue(m_nReductionLevel);
	m_sliderReduction->setMouseTracking(true);
	m_sliderReduction->installEventFilter(this);

	m_bPressReduce = false;

	connect(m_sliderReduction, &QSlider::sliderPressed, this, &VisualPrintMeshSmoothWidget::slot_OnSliderPressReduce);
	connect(m_sliderReduction, &QSlider::valueChanged, this, &VisualPrintMeshSmoothWidget::slot_OnSliderMovedReduce);
	connect(m_sliderReduction, &QSlider::sliderReleased, this, &VisualPrintMeshSmoothWidget::slot_OnSliderReleaseReduce);

	sliLayout->addWidget(m_sliderReduction);
	sliLayout->addWidget(emptyBox0);
	/////////////////// Reduction UI - End ////////////////////////////////////////////////////////////////

	setLayout(layMain);
}

bool VisualPrintMeshSmoothWidget::eventFilter(QObject* watched, QEvent* evt)
{
	if (NULL == watched)
	{
		return false;
	}

	const QEvent::Type type = evt->type();

	int res = 0;

	if (type == QEvent::HoverEnter)
	{
		this->unsetCursor();
	}
	else if (type == QEvent::HoverLeave ||
		type == QEvent::Leave || type == QEvent::Show)
	{
	}

	if (watched->objectName().contains("Slider"))
	{
		if (
			type == QEvent::Scroll ||
			type == QEvent::Wheel)
		{
			return true;
		}
	}

	return QWidget::eventFilter(watched, evt);
}

void VisualPrintMeshSmoothWidget::setParams(int _val_smooth, int _val_reduce)
{
	m_slider->setValue(_val_smooth);
	m_labelMeshSmooth->setText(QString().sprintf("Smooth  %d", _val_smooth));

	m_sliderReduction->setValue(_val_reduce);
	m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", _val_reduce));

	m_nSmoothLevel = _val_smooth;
	m_nReductionLevel = _val_reduce;
	m_nRemeshLevel = 0;
}

void VisualPrintMeshSmoothWidget::setSmoothVal(int _val_smooth)
{
	m_slider->setValue(_val_smooth);
	m_labelMeshSmooth->setText(QString().sprintf("Smooth  %d", _val_smooth));

	m_nSmoothLevel = _val_smooth;
}

void VisualPrintMeshSmoothWidget::setReduceVal(int _val_reduce)
{
	m_sliderReduction->setValue(_val_reduce);
	m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", _val_reduce));

	m_nReductionLevel = _val_reduce;
}

///////////////// For Smooth Control - Start ///////////////////////////////////////////////
void VisualPrintMeshSmoothWidget::slot_OnSliderPress()
{
	if (!m_press)
	{
		m_press = true;

		MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
		if (pMeshInfo)
		{
		}
		else
		{
			QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
		}
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSliderRelease()
{
	if (m_press)
	{
		m_press = false;
	}

	// WIN_MANAGER->setVisualPrintMeshSmoothLevel(m_alpha); 
	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nSmoothLevel = m_nSmoothLevel;

		m_labelMeshSmooth->setText(QString().sprintf("Smooth  %d", m_nSmoothLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSpinChanged(int val)
{
	if (m_nSmoothLevel == val)
		return;

	m_nSmoothLevel = val;

	m_slider->blockSignals(true);
	m_slider->setValue(m_nSmoothLevel);
	m_slider->blockSignals(false);
	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nSmoothLevel = m_nSmoothLevel;

		m_labelMeshSmooth->setText(QString().sprintf("Smooth  %d", m_nSmoothLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSliderMoved(int pos)
{
	if (m_nSmoothLevel == pos)
	{
		return;
	}

	m_nSmoothLevel = (float)pos;

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nSmoothLevel = m_nSmoothLevel;
		m_labelMeshSmooth->setText(QString().sprintf("Smooth  %d", m_nSmoothLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}
///////////////// For Smooth Control - End ///////////////////////////////////////////////

///////////////// For Remesh Control - Start ///////////////////////////////////////////////
void  VisualPrintMeshSmoothWidget::slot_OnSliderMovedReduce(int pos)
{
	m_nReductionLevel = (float)pos;

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nReduceLevel = m_nReductionLevel;

		m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", m_nReductionLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void  VisualPrintMeshSmoothWidget::slot_OnSliderPressReduce()
{
	if (!m_bPressReduce)
	{
		m_bPressReduce = true;

		MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
		if (pMeshInfo)
		{
			pMeshInfo->m_nReduceLevel = m_nReductionLevel;

			m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", m_nReductionLevel));
		}
		else
		{
			QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
		}
	}
}

void  VisualPrintMeshSmoothWidget::slot_OnSliderReleaseReduce()
{
	if (m_press)
	{
		m_bPressReduce = false;
	}

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nReduceLevel = m_nReductionLevel;

		m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", m_nReductionLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void  VisualPrintMeshSmoothWidget::slot_OnSpinChangedReduce(int val)
{
	m_nReductionLevel = val;

	m_sliderReduction->blockSignals(true);
	m_sliderReduction->setValue(m_nReductionLevel);
	m_sliderReduction->blockSignals(false);

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nReduceLevel = m_nReductionLevel;

		m_labelMeshReduce->setText(QString().sprintf("Reduce  %d", m_nReductionLevel));
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}
///////////////// For Remesh Control - End ///////////////////////////////////////////////


///////////////// For Remesh Control - Start ///////////////////////////////////////////////
void VisualPrintMeshSmoothWidget::slot_OnSliderPressRemesh()
{
	if (!m_bPressRemesh)
	{
		m_bPressRemesh = true;

		MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
		if (pMeshInfo)
		{
			pMeshInfo->m_nRemeshLevel = m_nRemeshLevel;
			//   QString qstrRemeshLevel = QStringLiteral("RemeshLevel is %1. That's all!").arg(pMeshInfo->m_nRemeshLevel);
			//   QMessageBox::warning(NULL, "qstrRemeshLevel", qstrRemeshLevel);
		}
		else
		{
			QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
		}
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSliderReleaseRemesh()
{
	if (m_bPressRemesh)
	{
		m_bPressRemesh = false;
	}

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nRemeshLevel = m_nRemeshLevel;
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSpinChangedRemesh(int val)
{
	m_nRemeshLevel = val;

	m_sliderRemesh->blockSignals(true);
	m_sliderRemesh->setValue(m_nRemeshLevel);
	m_sliderRemesh->blockSignals(false);

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nRemeshLevel = m_nRemeshLevel;
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}

void VisualPrintMeshSmoothWidget::slot_OnSliderMovedRemesh(int pos)
{
	m_nRemeshLevel = pos;

	m_pSpinBoxRemesh->blockSignals(true);
	m_pSpinBoxRemesh->setValue(m_nRemeshLevel);
	m_pSpinBoxRemesh->blockSignals(false);

	MeshInfo* pMeshInfo = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(m_muint32MeshInfoUID);
	if (pMeshInfo)
	{
		pMeshInfo->m_nRemeshLevel = m_nRemeshLevel;
	}
	else
	{
		QMessageBox::warning(NULL, "Recommend", "Make sure you choose ROI!");
	}
}
///////////////// For Remesh Control - End ///////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Visual Print Mesh Smooth Widget Class Member Functions - End
///////////////////////////////////////////////////////////////////////////////////