#pragma once

#include <qdialog>
#include "filemanager/export.h"

namespace Ui
{
	class DicomVolumeViewerDialog;
}

namespace fm
{
	class VolumeImageData;
	struct WindowingInfo;

	class FM_CORE_EXPORT DicomVolumeViewerDialog : public QDialog
	{
		Q_OBJECT

	public:
		DicomVolumeViewerDialog(VolumeImageData* pVolumeImage, QWidget* parent);
		virtual ~DicomVolumeViewerDialog();

	protected:
		virtual void mousePressEvent(QMouseEvent* e) override;
		virtual void mouseMoveEvent(QMouseEvent* e) override;
		virtual void mouseReleaseEvent(QMouseEvent* e) override;
		virtual void resizeEvent(QResizeEvent* e) override;

	private:
		void updateViewer();
		void updateControls();

	private slots:
		void onSliderDepthChanged(int value);
		void onChkApplyOffset();

	private:
		Ui::DicomVolumeViewerDialog* m_ui;
		VolumeImageData* m_pVolumeImageCurrent;
		VolumeImageData* m_pVolumeImageOrigin;
		VolumeImageData* m_pVolumeImageApplyOffset;
		WindowingInfo* m_windowingInfo;
	};
}
