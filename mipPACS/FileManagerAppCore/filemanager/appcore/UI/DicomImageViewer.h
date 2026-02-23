#pragma once

#include <qwidget>
#include <qcolor>
#include <QScrollBar>
#include <QListWidgetItem>
#include "filemanager/appcore/appcore_defines.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/DicomDatasetIO.h"
#include "filemanager/data/Image/WindowingInfo.h"
#include "filemanager/export.h"

class QLabel;
class QVBoxLayout;

namespace Ui
{
	class DicomImageViewer;
}

namespace fm
{
	class DicomDataImage_sint16;

	class FM_CORE_EXPORT DicomImageViewer : public QWidget
	{
		Q_OBJECT

	public:
		DicomImageViewer(QWidget* parent = nullptr);

	public:
		void SetDicomSeries(DicomInfomationModelSeriesObject* pSeries, bool forceToUpdateImage = false);
		void Clear();

	protected:

	private:
		void UpdateScrollBarBySeries();
		void UpdateDatasetByCurrentScrollBarIndex();
		void UpdateImage(DicomDataset* pDataset);
		void UpdateWindowingInfo();

		QRect GetDrawRect(QPixmap& imagePixmap);
		float GetViewerWidgetPerPixmapRate(QPixmap& imagePixmap);
		QPixmap GetDrawResultPixmap(QPixmap& imagePixmap);

	protected:
		virtual void mousePressEvent(QMouseEvent* event) override;
		virtual void mouseReleaseEvent(QMouseEvent* event) override;
		virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
		virtual void mouseMoveEvent(QMouseEvent* event) override;
		virtual void wheelEvent(QWheelEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;

	private slots:
		void onImageDepthChanged(int value);
		void onBtnZoomIn();
		void onBtnZoomOut();

	private:
		Ui::DicomImageViewer* m_ui;
		WindowingInfo m_windowingInfo;

		DicomInfomationModelSeriesObject* m_pSeries;
		DicomDataset* m_pCurrentDataset;

		float m_zoomFactor;
		float m_zoomStep;
		QPoint m_imagePointDelta;

		Qt::MouseButton m_mouseButton;
		QPoint m_previosMousePoint;
		QPoint m_currentMousePoint;

	};
}
