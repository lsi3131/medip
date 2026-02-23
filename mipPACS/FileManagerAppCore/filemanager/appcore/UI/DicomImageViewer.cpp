#include "stdafx.h"
#include "DicomImageViewer.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomType.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/Image/DicomImageToQTConverter.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/data/Image/VolumeImageData.h"
#include "filemanager/render/CPUMPRPlane2DRenderingManager.h"
#include "ui_DicomImageViewer.h"
#include <qlabel>
#include <qlayout>
#include <qdir>
#include <QElapsedTimer>
#include <qpainter>
#include <qmouseevent>

using namespace fm;

namespace fm
{
	DicomImageViewer::DicomImageViewer(QWidget* parent) :
		QWidget(parent),
		m_pSeries(nullptr),
		m_pCurrentDataset(nullptr)
	{
		m_ui = new Ui::DicomImageViewer();
		m_ui->setupUi(this);

		//m_ui->m_labelImageViewer->setScaledContents(true);
		m_ui->m_iconContainerWidget->setVisible(false);
		m_zoomFactor = 1.0f;
		m_zoomStep = 0.04f;

		/*
			반드시m_labelImageViewer의 Size Policy Ignore로 처리할 것.(ui에서 처리)
			QLabel->setPixmap 호출 시 무한히 resizeEvent 발생
		*/
		connect(m_ui->m_scrollBarImage, &QScrollBar::valueChanged, this, &DicomImageViewer::onImageDepthChanged);
		connect(m_ui->m_btnZoomIn, &QPushButton::clicked, this, &DicomImageViewer::onBtnZoomIn);
		connect(m_ui->m_btnZoomOut, &QPushButton::clicked, this, &DicomImageViewer::onBtnZoomOut);
	}

	void DicomImageViewer::SetDicomSeries(DicomInfomationModelSeriesObject* pSeries, bool forceToUpdateImage)
	{
		if (pSeries == nullptr)
		{
			return;
		}

		if (m_pSeries != pSeries)
		{
			m_pSeries = pSeries;
		}

		UpdateScrollBarBySeries();

		/* Image가 없을 경우 */
		const QPixmap* pmap = m_ui->m_labelImageViewer->pixmap();

		if (pmap == nullptr || forceToUpdateImage)
		{
			UpdateWindowingInfo();
			UpdateImage(m_pCurrentDataset);
		}
	}

	void DicomImageViewer::Clear()
	{
		m_pSeries = nullptr;
		m_pCurrentDataset = nullptr;
		m_ui->m_labelImageViewer->clear();
		m_imagePointDelta = QPoint(0, 0);
		m_zoomFactor = 1.0f;
	}

	void DicomImageViewer::UpdateScrollBarBySeries()
	{
		if (m_pSeries == nullptr)
		{
			return;
		}
		int imageCount = m_pSeries->ImageInstanceCount();
		int index = (imageCount - 1) / 2;

		m_ui->m_scrollBarImage->blockSignals(true);
		m_ui->m_scrollBarImage->setMinimum(0);
		m_ui->m_scrollBarImage->setMaximum(imageCount - 1);
		m_ui->m_scrollBarImage->setValue(index);
		m_ui->m_scrollBarImage->blockSignals(false);

		UpdateDatasetByCurrentScrollBarIndex();
	}

	void DicomImageViewer::UpdateDatasetByCurrentScrollBarIndex()
	{
		if (m_pSeries == nullptr)
		{
			return;
		}

		m_pCurrentDataset = nullptr;
		int index = m_ui->m_scrollBarImage->value();
		m_pSeries->Get(&m_pCurrentDataset, index);
	}

	void DicomImageViewer::UpdateImage(DicomDataset* pDataset)
	{
		if (pDataset == nullptr)
		{
			m_ui->m_labelImageViewer->clear();
		}
		else
		{
			QPixmap imagePixmap = DicomImageToQTConverter::GetPixmap(pDataset, &m_windowingInfo);

			/* resize pixmap */
			if (!imagePixmap.isNull())
			{
				QPixmap resultPixmap = GetDrawResultPixmap(imagePixmap);
				m_ui->m_labelImageViewer->setPixmap(resultPixmap);
			}
		}
	}

	void DicomImageViewer::UpdateWindowingInfo()
	{
		if (m_pCurrentDataset)
		{
			DicomType dcmType(m_pCurrentDataset);
			if (dcmType.Is_8Bit_Or_16Bit_Image())
			{
				DicomDataImage_sint16 image(m_pCurrentDataset);
				m_windowingInfo = image.GetWindowingInfo();
			}
		}
	}

	QRect DicomImageViewer::GetDrawRect(QPixmap& imagePixmap)
	{
		float rate = GetViewerWidgetPerPixmapRate(imagePixmap);

		QSize scaledImagePixmapSize = imagePixmap.size() * rate;

		QSize viewerWidgetSize = m_ui->m_labelImageViewer->size();

		int posX = (viewerWidgetSize.width() - scaledImagePixmapSize.width()) / 2.0;
		int posY = (viewerWidgetSize.height() - scaledImagePixmapSize.height()) / 2.0;

		QPoint pt = QPoint(posX, posY) + m_imagePointDelta;

		return QRect(pt.x(), pt.y(), scaledImagePixmapSize.width(), scaledImagePixmapSize.height());
	}

	float DicomImageViewer::GetViewerWidgetPerPixmapRate(QPixmap& imagePixmap)
	{
		float viewerWidgetWidth = m_ui->m_labelImageViewer->width();
		float viewerWidgetHeight = m_ui->m_labelImageViewer->height();
		float imagePixmapWidth = imagePixmap.width();
		float imagePixmapHeight = imagePixmap.height();

		return (viewerWidgetHeight / imagePixmapHeight) * m_zoomFactor;
	}

	QPixmap DicomImageViewer::GetDrawResultPixmap(QPixmap& imagePixmap)
	{
		QRect drawRect = GetDrawRect(imagePixmap);
		QSize viewerWidgetSize = m_ui->m_labelImageViewer->size();

		QPixmap resultPixmap(viewerWidgetSize);
		resultPixmap.fill(Qt::black);

		QPainter p(&resultPixmap);
		p.drawPixmap(drawRect, imagePixmap);
		return resultPixmap;
	}

	void DicomImageViewer::mousePressEvent(QMouseEvent* event)
	{
		m_mouseButton = event->button();

		m_previosMousePoint = m_currentMousePoint;
		m_currentMousePoint = event->pos();

		QWidget::mousePressEvent(event);
	}

	void DicomImageViewer::mouseReleaseEvent(QMouseEvent* event)
	{
		m_mouseButton = event->button();

		QWidget::mouseReleaseEvent(event);
	}

	void DicomImageViewer::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QWidget::mouseDoubleClickEvent(event);
	}

	void DicomImageViewer::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mouseMoveEvent(event);

		m_previosMousePoint = m_currentMousePoint;
		m_currentMousePoint = event->pos();
		QPoint deltaMouse = m_currentMousePoint - m_previosMousePoint;
		if (m_mouseButton & Qt::MiddleButton)
		{
			m_imagePointDelta += deltaMouse;

			UpdateImage(m_pCurrentDataset);
		}
		else if (m_mouseButton & Qt::LeftButton)
		{
		}
		else if (m_mouseButton & Qt::RightButton)
		{
			m_windowingInfo.Level -= deltaMouse.y();
			m_windowingInfo.Width += deltaMouse.x();
			UpdateImage(m_pCurrentDataset);
		}
	}

	void DicomImageViewer::wheelEvent(QWheelEvent* event)
	{
		QWidget::wheelEvent(event);

		if (event->delta() > 0)
		{
			onBtnZoomIn();
		}
		else
		{
			onBtnZoomOut();
		}
	}

	void DicomImageViewer::resizeEvent(QResizeEvent* event)
	{
		QWidget::resizeEvent(event);
	}

	void DicomImageViewer::onBtnZoomIn()
	{
		m_zoomFactor += m_zoomStep;
		UpdateImage(m_pCurrentDataset);
	}

	void DicomImageViewer::onBtnZoomOut()
	{
		m_zoomFactor -= m_zoomStep;
		UpdateImage(m_pCurrentDataset);
	}

	void DicomImageViewer::onImageDepthChanged(int value)
	{
		UpdateDatasetByCurrentScrollBarIndex();
		UpdateImage(m_pCurrentDataset);
	}
}
