#include "stdafx.h"
#include "DicomVolumeViewerDialog.h"
#include "ui_DicomVolumeViewerDialog.h"
#include "filemanager/render/CPUMPRPlane2DRenderingManager.h"
#include "filemanager/data/Image/VolumeImageData.h"
#include "filemanager/data/Image/VolumeUtility.h"
#include "filemanager/data/Image/WindowingInfo.h"
#include <QMouseEvent>

namespace fm
{
	DicomVolumeViewerDialog::DicomVolumeViewerDialog(VolumeImageData* pVolumeImage, QWidget* parent) :
		QDialog(parent),
		m_ui(nullptr),
		m_pVolumeImageCurrent(nullptr),
		m_pVolumeImageOrigin(pVolumeImage)
	{
		m_pVolumeImageApplyOffset = new VolumeImageData();
		*m_pVolumeImageApplyOffset = *pVolumeImage;

		//ApplyHUOffsetToVolume(m_pVolumeImageApplyOffset, DCM_MODAL_TYPE::)

		m_pVolumeImageCurrent = m_pVolumeImageOrigin;

		m_windowingInfo = new WindowingInfo();

		m_ui = new Ui::DicomVolumeViewerDialog();
		m_ui->setupUi(this);

		int depth = m_pVolumeImageOrigin->getCZ() / 2;
		m_pVolumeImageOrigin->UpdateHUMinMax();
		int huMin = m_pVolumeImageOrigin->HUMin();
		int huMax = m_pVolumeImageOrigin->HUMax();

		m_windowingInfo->InitByHU(huMin, huMax);

		m_ui->m_sliderDepth->setRange(0, m_pVolumeImageOrigin->getCZ() - 1);
		m_ui->m_sliderDepth->setValue(depth);

		connect(m_ui->m_sliderDepth, &QSlider::valueChanged, this, &DicomVolumeViewerDialog::onSliderDepthChanged);
		connect(m_ui->m_chkApplyOffset, &QCheckBox::clicked, this, &DicomVolumeViewerDialog::onChkApplyOffset);

		updateViewer();
		updateControls();
	}
	
	DicomVolumeViewerDialog::~DicomVolumeViewerDialog()
	{
		delete m_ui;
		delete m_windowingInfo;
		delete m_pVolumeImageApplyOffset;
	}

	void DicomVolumeViewerDialog::mousePressEvent(QMouseEvent* e)
	{
		QDialog::mousePressEvent(e);
		QPoint pos = m_ui->m_volumeViewer->mapFromParent(e->pos());
		int px = pos.x();
		int py = pos.y();

		int vx = px;
		int vy = py;
		int vz = m_ui->m_sliderDepth->depth();

		m_ui->m_editPos_Viewer->setText(QString("(%1,%2)").arg(px).arg(py));
		m_ui->m_editPos_Volume->setText(QString("(%1,%2,%3)").arg(vx).arg(vy).arg(vz));

		int HU = m_pVolumeImageOrigin->GetData(vx, vy, vz);
		m_ui->m_editHU->setText(QString::number(HU));
	}

	void DicomVolumeViewerDialog::mouseMoveEvent(QMouseEvent* e)
	{
		QDialog::mouseMoveEvent(e);
		QPoint pos = m_ui->m_volumeViewer->mapFromParent(e->pos());
		int px = pos.x();
		int py = pos.y();

		int vx = px;
		int vy = py;
		int vz = m_ui->m_sliderDepth->value();

		m_ui->m_editPos_Viewer->setText(QString("(%1,%2)").arg(px).arg(py));
		m_ui->m_editPos_Volume->setText(QString("(%1,%2,%3)").arg(vx).arg(vy).arg(vz));

		int HU = m_pVolumeImageOrigin->GetData(vx, vy, vz);
		m_ui->m_editHU->setText(QString::number(HU));
	}

	void DicomVolumeViewerDialog::mouseReleaseEvent(QMouseEvent* e)
	{
		QDialog::mouseReleaseEvent(e);
	}

	void DicomVolumeViewerDialog::resizeEvent(QResizeEvent* e)
	{
		QDialog::resizeEvent(e);
		updateControls();
	}

	void DicomVolumeViewerDialog::onSliderDepthChanged(int value)
	{
		updateViewer();
	}

	void DicomVolumeViewerDialog::updateViewer()
	{
		WINDOW_TYPE winType = WT_AXIAL;
		QPixmap pmap;
		int depth = m_ui->m_sliderDepth->value();

		if (CPUMPRPlane2DRenderingManager::DrawCT(pmap, m_pVolumeImageCurrent, winType, depth, m_windowingInfo->Level, m_windowingInfo->Width))
		{
			m_ui->m_volumeViewer->setPixmap(pmap);
		}
	}
	void DicomVolumeViewerDialog::updateControls()
	{
		int width_Viewer = m_ui->m_volumeViewer->width();
		int height_Viewer = m_ui->m_volumeViewer->height();
		m_ui->m_editSize_Viewer->setText(QString("(%1,%2)").arg(width_Viewer).arg(height_Viewer));

		int width_Volume = m_pVolumeImageCurrent->getCX();
		int height_Volume = m_pVolumeImageCurrent->getCY();
		m_ui->m_editSize_Volume->setText(QString("(%1,%2)").arg(width_Volume).arg(height_Volume));
	}

	void DicomVolumeViewerDialog::onChkApplyOffset()
	{
		if (m_ui->m_chkApplyOffset->isChecked())
		{
			m_pVolumeImageCurrent = m_pVolumeImageApplyOffset;
		}
		else
		{
			m_pVolumeImageCurrent = m_pVolumeImageOrigin;
		}
		updateViewer();
		updateControls();
	}
}
