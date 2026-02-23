#include "stdafx.h"
#include "SubView.h"
#include "windowManager.h"
#include "System/resourceManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "DataContext.h"

SubView::SubView(int depth_offset, QWidget* parent)
	:WindowBase(parent)
{
	m_windowType = WT_SUB;
	m_depth_offset = depth_offset;
	m_flipY = false;
}

void SubView::init()
{

}

SubView::~SubView()
{
}

void SubView::render(QPainter *p)
{
	WindowBase::render(p);

	if (DATA_CONTEXT->volume_data.isValidate())
	{
		QPoint temp_point = m_SlicePosition;
		WINDOW_TYPE temp_type = m_windowType;

		m_windowType = WIN_MANAGER->getLatestActiveViewType();
		m_depth = WIN_MANAGER->getLatestActiveViewDepth();

		unsigned int c_x;
		unsigned int c_y;
		unsigned int c_depth;
		DATA_CONTEXT->volume_data.getLengthForScreen(m_windowType, c_x, c_y, c_depth);

		int new_depth = m_depth + m_depth_offset;
		if (m_windowType == WT_AXIAL) // axial reversing
			new_depth = m_depth - m_depth_offset;

		if (new_depth >= c_depth || new_depth < 0 || m_windowType == WT_VOLUME)
		{
			QFontMetrics fm(p->font());
			QRect rt = fm.boundingRect("Out of Area");

			p->drawText(QPoint((this->width() - rt.width())/2, this->height() / 2), "Out of Area");
			return;
		}

		uint temp_depth = m_depth;
		m_depth = new_depth;

		if (WIN_MANAGER->mainSegmentWidget)
		{
			WindowBase * win = WIN_MANAGER->mainSegmentWidget->getWindow(m_windowType);
			if (win)
			{
				m_flipX = win->getFlipX();
				m_flipY = win->getFlipY();
				QPoint zero = QPoint(c_x/2, c_y/2);
				setZoomFactor(WIN_MANAGER->getLatestActiveViewZoomX(), WIN_MANAGER->getLatestActiveViewZoomY());
				QVector3D screenZeroPos = volumeToScreenPositionAuto(zero.x(), zero.y(), m_depth);
				QVector3D clickPos = volumeToScreenPosition(win->getLatestClickPos().x(), win->getLatestClickPos().y(), win->getLatestClickPos().z());
				m_SlicePosition.setX(screenZeroPos.x() - clickPos.x());
				m_SlicePosition.setY(screenZeroPos.y() - clickPos.y());
			}
		}
//		if (temp_type != m_windowType)
		{
			resetResource();
		}
		drawSlice(p);

		m_SlicePosition = temp_point;
		m_windowType = temp_type;
		m_depth = temp_depth;
	}
	else
	{
		QColor background = RESOURCE_MANAGER->getUnloadBackGroundColor();
		p->fillRect(0, 0, width(), height(), background);
	}
}