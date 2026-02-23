#include "stdafx.h"
#include "ActionMarking.h"

#include "System/stringManager.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalMPRPlaneView.h"

#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"

#include "ActionRegionGrowing.h"
#include "Actions/ActionManager.h"


struct sPOINT
{
	int x, y;
	sPOINT(int _x, int _y) { x = _x; y = _y; }
};
struct
{
	bool operator ()(sPOINT & p1, sPOINT & p2) const
	{
		return (p1.x < p2.x);
	}
} CmpareX;

struct
{
	bool operator ()(sPOINT & p1, sPOINT & p2) const
	{
		return (p1.y < p2.y);
	}
} CmpareY;

struct
{
	bool operator ()(const int & p1, const int & p2) const
	{
		return (p1 < p2);
	}
} Cmpare;


ActionMarking::ActionMarking(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, bool drawCutMarking, mask _m, bool merge, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_MARKING;
	m_view = view;
	m_mask = _m;
	m_maskIndex = _mI;
	m_id = s_id;
	m_drawCutMarking = drawCutMarking;
	MaskInfo *info = m_pVolumeData->findMaskInfo(m_maskIndex, m_mask);
	m_maskNumber = m_drawCutMarking ? (m_mask - 1) : info ? info->uid : m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber, m_drawCutMarking);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawCutMarking);
	m_orgbox[1].reset(true);
	if (view != NULL)
	{
		m_depth = view->getDepth();
		m_type = view->getType();
		QPoint pos;
		QPoint posVolume;
		m_list.reserve(list.size() * 3);
		for (int n = 0; n < list.size(); n++)
		{
			const QPoint & b = list[n];
			if (n > 0)
			{
				const QPoint & a = list[n - 1];
				QPointF dir = (b - a);
				int length = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
				dir.setX(dir.x() / length);
				dir.setY(dir.y() / length);
				for (int t = 0; t < length; t++)
				{
					pos = a + QPoint(dir.x() * t, dir.y() * t);
					posVolume = m_view->screenToVolumePosition(&pos);

					switch (m_view->getType())
					{
					case WT_CORONAL:
						if (m_pVolumeData->isMaskBit(posVolume.x(), m_depth, posVolume.y(), m_mask, m_maskIndex) == false)
							m_list.push_back(posVolume);
						break;
					case WT_SAGITTAL:
						if (m_pVolumeData->isMaskBit(m_depth, posVolume.x(), posVolume.y(), m_mask, m_maskIndex) == false)
							m_list.push_back(posVolume);
						break;
					case WT_AXIAL:
					default:
						if (m_pVolumeData->isMaskBit(posVolume.x(), posVolume.y(), m_depth, m_mask, m_maskIndex) == false)
							m_list.push_back(posVolume);
						break;
					}

				}
			}
			else
			{
				posVolume = m_view->screenToVolumePosition(&b);

				switch (m_view->getType())
				{
				case WT_CORONAL:
					if (m_pVolumeData->isMaskBit(posVolume.x(), m_depth, posVolume.y(), m_mask, m_maskIndex) == false)
						m_list.push_back(posVolume);
					break;
				case WT_SAGITTAL:
					if (m_pVolumeData->isMaskBit(m_depth, posVolume.x(), posVolume.y(), m_mask, m_maskIndex) == false)
						m_list.push_back(posVolume);
					break;
				case WT_AXIAL:
				default:
					if (m_pVolumeData->isMaskBit(posVolume.x(), posVolume.y(), m_depth, m_mask, m_maskIndex) == false)
						m_list.push_back(posVolume);
					break;
				}
			}
		}

		if (merge == false)
			s_id++;


	}
}

ActionMarking::ActionMarking(VOLUME_DATA* pVolumeData, QPainterPath screen_region, WindowBase * view, mask _m, bool merge, int _mI, bool drawcut, bool brush, int startHU, int endHU, QUndoCommand * parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_MARKING;
	m_view = view;
	m_mask = _m;
	m_id = s_id;
	m_maskIndex = _mI;
	m_drawCutMarking = drawcut;
	MaskInfo *info = m_pVolumeData->findMaskInfo(m_maskIndex, m_mask);
	m_maskNumber = m_drawCutMarking ? (m_mask - 1) : info ? info->uid : m_pVolumeData->getCurrentMaskInfo()->uid;
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawCutMarking);
	m_orgbox[1].reset(true);
	m_pVolumeData->fillMaskCount = 0;

	if (view != NULL)
	{
		m_type = view->getType();
		m_depth = view->getDepth();
		screen_region.setFillRule(Qt::WindingFill);
		QPolygonF poly = screen_region.toFillPolygon();
		QRegion region(poly.toPolygon(), Qt::WindingFill);
		unsigned int c_x;
		unsigned int c_y;
		unsigned int c_depth;
		m_pVolumeData->getLengthForScreen(view->getType(), c_x, c_y, c_depth);
		QVector3D tpoint;

		QVector3D size = (view->volumeToScreenPositionAuto(1, 1, m_depth) - view->volumeToScreenPositionAuto(0, 0, m_depth));
		QPoint p1, p4;

		m_list.reserve(c_x*c_y);

		for (int y = 0; y < c_y; y++)
		{
			for (int x = 0; x < c_x; x++)
			{
				tpoint = view->volumeToScreenPositionAuto(x, y, m_depth);

				if (view->getFlipY() == false)
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() + size.y()));
					//	p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() + size.y()));
					//	p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}
				else
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() - size.y()));
					//	p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() - size.y()));
					//	p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}
				QRect rect;
				rect.setCoords(p1.x(), p1.y(), p4.x(), p4.y());
				//	if (region.contains(p1) || region.contains(p2) || region.contains(p3) || region.contains(p4))
				if (region.intersects(rect) || region.contains(rect))
				{
					if (m_pVolumeData->isMaskBit(m_type, x, y, m_depth, m_mask, m_maskIndex) == false)
					{
						int m_width = m_pVolumeData->getCX();
						int m_height = m_pVolumeData->getCY();
						int m_HU; 
						if(view->getType() == WT_AXIAL)
							m_HU= m_pVolumeData->getHUDataPoint()[m_depth* m_width*m_height + m_width * y + x];
						else if(view->getType() == WT_CORONAL)
							m_HU = m_pVolumeData->getHUDataPoint()[y* m_width*m_height + m_width *m_depth + x];
						else if (view->getType() == WT_SAGITTAL)
							m_HU = m_pVolumeData->getHUDataPoint()[y* m_width*m_height + m_width * x + m_depth];

						// check xy inside volume
						if(checkInsizeVolume(m_type, x, y))
						{
							if (!brush)
								m_list.push_back(QPoint(x, y));
							else if (brush && (m_HU >= startHU && m_HU <= endHU))
							{
								m_list.push_back(QPoint(x, y));

							}
							else
							{


							}
						}
					}
				}
			}
		}
		if (merge == false)
			s_id++;
	}
}

bool ActionMarking::checkInsizeVolume(WINDOW_TYPE winType, int x, int y)
{
	BoundingBoxI bb = m_pVolumeData->getBoundingBox();

	int volumeMinX;
	int volumeMinY;

	int volumeMaxX;
	int volumeMaxY;

	switch (winType)
	{
	case WT_CORONAL:
		volumeMinX = bb.minX;
		volumeMinY = bb.minZ;
		volumeMaxX = bb.maxX;
		volumeMaxY = bb.maxZ;
		break;
	case WT_SAGITTAL:
		volumeMinX = bb.minY;
		volumeMinY = bb.minZ;
		volumeMaxX = bb.maxY;
		volumeMaxY = bb.maxZ;
		break;
	case WT_AXIAL:
	default:
		volumeMinX = bb.minX;
		volumeMinY = bb.minY;
		volumeMaxX = bb.maxX;
		volumeMaxY = bb.maxY;
		break;
	}

	return ((volumeMinX <= x) && (volumeMaxX >= x) && (volumeMinY <= y) && (volumeMaxY >= y));
}

bool ActionMarking::mergeWith(const QUndoCommand *command)
{
	const ActionMarking *markingCommand = static_cast<const ActionMarking *>(command);

	if (markingCommand == NULL) return false;

	if (markingCommand->getView() != m_view || markingCommand->getDepth() != m_depth || markingCommand->getMask() != m_mask)
		return false;

	for (auto a = markingCommand->getList().begin(); a != markingCommand->getList().end(); ++a)
	{
		m_list.push_back(*a);
	}

	return true;
}

void ActionMarking::undo()
{
	m_pVolumeData->delMaskBitList(m_type, &m_list, m_depth, m_mask, m_maskIndex);
	//	if (false == m_drawCutMarking)
	m_pVolumeData->setVoxelCount(m_maskNumber, m_pVolumeData->fillMaskCount, false, m_drawCutMarking);
	//	m_pVolumeData->voxelCount[m_maskNumber] += m_pVolumeData->fillMaskCount;
//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
	m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0], false, m_drawCutMarking);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	m_pVolumeData->setTAState(m_maskNumber, m_TAState, m_drawCutMarking);
	WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawCutMarking);
	WIN_MANAGER->setSaveState(false);
}

void ActionMarking::redo()
{
	m_pVolumeData->setMaskBitList(m_type, &m_list, m_depth, m_mask, m_maskIndex);
	//	if (false == m_drawCutMarking)
	m_pVolumeData->setVoxelCount(m_maskNumber, m_pVolumeData->fillMaskCount, false, m_drawCutMarking);
	//	m_pVolumeData->voxelCount[m_maskNumber] += m_pVolumeData->fillMaskCount;
	if (!m_orgbox[1].isFitROI())
	{
		m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true, m_drawCutMarking);
		m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawCutMarking);
		//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	}
	//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
	else
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1], false, m_drawCutMarking);

	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawCutMarking);
	m_pVolumeData->setTAState(m_maskNumber, false, m_drawCutMarking);
	WIN_MANAGER->setSaveState(false);
}




ActionUnMarking::ActionUnMarking(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m, bool merge, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_mask = _m;
	m_maskIndex = _mI;
	m_id = s_id;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	if (view != NULL)
	{
		m_depth = view->getDepth();
		m_type = view->getType();
		QPoint pos;
		QPoint posVolume;
		m_list.reserve(list.size() * 3);
		for (int n = 0; n < list.size(); n++)
		{
			const QPoint & b = list[n];
			if (n > 0)
			{
				const QPoint & a = list[n - 1];
				QPointF dir = (b - a);
				int length = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
				dir.setX(dir.x() / length);
				dir.setY(dir.y() / length);
				for (int t = 0; t < length; t++)
				{
					pos = a + QPoint(dir.x() * t, dir.y() * t);
					posVolume = m_view->screenToVolumePosition(&pos);

					switch (m_view->getType())
					{
					case WT_CORONAL:
						if (m_pVolumeData->isMaskBit(posVolume.x(), m_depth, posVolume.y(), m_mask, m_maskIndex) == true)
							m_list.push_back(posVolume);
						break;
					case WT_SAGITTAL:
						if (m_pVolumeData->isMaskBit(m_depth, posVolume.x(), posVolume.y(), m_mask, m_maskIndex) == true)
							m_list.push_back(posVolume);
						break;
					case WT_AXIAL:
					default:
						if (m_pVolumeData->isMaskBit(posVolume.x(), posVolume.y(), m_depth, m_mask, m_maskIndex) == true)
							m_list.push_back(posVolume);
						break;
					}
				}
			}
			else
			{
				posVolume = m_view->screenToVolumePosition(&b);
				switch (m_view->getType())
				{
				case WT_CORONAL:
					if (m_pVolumeData->isMaskBit(posVolume.x(), m_depth, posVolume.y(), m_mask, m_maskIndex) == true)
						m_list.push_back(posVolume);
					break;
				case WT_SAGITTAL:
					if (m_pVolumeData->isMaskBit(m_depth, posVolume.x(), posVolume.y(), m_mask, m_maskIndex) == true)
						m_list.push_back(posVolume);
					break;
				case WT_AXIAL:
				default:
					if (m_pVolumeData->isMaskBit(posVolume.x(), posVolume.y(), m_depth, m_mask, m_maskIndex) == true)
						m_list.push_back(posVolume);
					break;
				}
			}
		}

		if (merge == false)
			s_id++;
	}
}

ActionUnMarking::ActionUnMarking(VOLUME_DATA* pVolumeData, QPainterPath screen_region, WindowBase * view, mask _m, bool merge, int _mI, QUndoCommand *parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_mask = _m;
	m_id = s_id;
	m_maskIndex = _mI;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	m_pVolumeData->fillMaskCount = 0;

	if (view != NULL)
	{
		m_type = view->getType();
		m_depth = view->getDepth();
		screen_region.setFillRule(Qt::WindingFill);
		QPolygonF poly = screen_region.toFillPolygon();
		QRegion region(poly.toPolygon(), Qt::WindingFill);
		unsigned int c_x;
		unsigned int c_y;
		unsigned int c_depth;
		m_pVolumeData->getLengthForScreen(view->getType(), c_x, c_y, c_depth);
		QVector3D tpoint;

		QVector3D size = (view->volumeToScreenPositionAuto(1, 1, m_depth) - view->volumeToScreenPositionAuto(0, 0, m_depth));
		QPoint p1, p4;

		m_list.reserve(c_x*c_y);

		for (int y = 0; y < c_y; y++)
		{
			for (int x = 0; x < c_x; x++)
			{
				tpoint = view->volumeToScreenPositionAuto(x, y, m_depth);

				if (view->getFlipY() == false)
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() + size.y()));
					//	p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() + size.y()));
					//	p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}
				else
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() - size.y()));
					//	p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() - size.y()));
					//	p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}
				QRect rect;
				rect.setCoords(p1.x(), p1.y(), p4.x(), p4.y());
				//	if (region.contains(p1) || region.contains(p2) || region.contains(p3) || region.contains(p4))
				if (region.intersects(rect) || region.contains(rect))
				{
					if (m_pVolumeData->isMaskBit(m_type, x, y, m_depth, m_mask, m_maskIndex) == true)
					{
						m_list.push_back(QPoint(x, y));
					}
				}
			}
		}
		if (merge == false)
			s_id++;
	}
}

ActionUnMarking::ActionUnMarking(VOLUME_DATA* pVolumeData, const QPoint screen_point, const QPolygon & list, WindowBase* view, mask _m, bool merge, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_mask = _m;
	m_maskIndex = _mI;
	m_id = s_id;
	m_type = m_view->getType();
	m_depth = m_view->getDepth();
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	if (view != NULL)
	{
		m_list.reserve(list.size());
		QPoint centerP = m_view->screenToVolumePosition(&screen_point);
		QPoint p;
		for (auto a = list.begin(); a != list.end(); ++a)
		{
			p = QPoint(centerP.x() + a->x(), centerP.y() + a->y());
			if (m_pVolumeData->isMaskBit(m_type, p.x(), p.y(), m_depth, m_mask, m_maskIndex) == true)
			{
				m_list.push_back(p);
			}
		}

		if (merge == false)
			s_id++;
	}
}

ActionUnMarking::ActionUnMarking(VOLUME_DATA* pVolumeData, QVector<QPoint> screen_point, const QPolygon & list, WindowBase* view, mask _m, bool merge, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_mask = _m;
	m_id = s_id;
	m_maskIndex = _mI;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	m_pVolumeData->fillMaskCount = 0;

	if (view != NULL)
	{
		m_type = view->getType();
		m_depth = view->getDepth();

		QPoint res;
		m_list.reserve(list.size());
		for (int i = 0; i < screen_point.size(); i++)
		{
			QPoint &centerP = screen_point[i];
			QPoint p;
			QPoint pV;
			if (i > 0)
			{
				const QPoint & a = screen_point[i - 1];
				QPointF dir = (centerP - a);
				int length = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
				dir.setX(dir.x() / length);
				dir.setY(dir.y() / length);
				for (int j = 0; j < length; j++)
				{
					p = a + QPoint(dir.x() * j, dir.y() * j);
					pV = m_view->screenToVolumePosition(&p);

					for (auto k = list.begin(); k != list.end(); ++k)
					{
						res = QPoint(pV.x() + k->x(), pV.y() + k->y());
						switch (m_view->getType())
						{
						case WT_CORONAL:
							if (m_pVolumeData->isMaskBit(res.x(), m_depth, res.y(), m_mask, m_maskIndex) == true)
								m_list.push_back(res);
							break;
						case WT_SAGITTAL:
							if (m_pVolumeData->isMaskBit(m_depth, res.x(), res.y(), m_mask, m_maskIndex) == true)
								m_list.push_back(res);
							break;
						case WT_AXIAL:
						default:
							if (m_pVolumeData->isMaskBit(res.x(), res.y(), m_depth, m_mask, m_maskIndex) == true)
								m_list.push_back(res);
							break;
						}
					}
				}
			}
			else
			{
				pV = m_view->screenToVolumePosition(&centerP);
				for (auto k = list.begin(); k != list.end(); ++k)
				{
					res = QPoint(pV.x() + k->x(), pV.y() + k->y());
					switch (m_view->getType())
					{
					case WT_CORONAL:
						if (m_pVolumeData->isMaskBit(res.x(), m_depth, res.y(), m_mask, m_maskIndex) == true)
							m_list.push_back(res);
						break;
					case WT_SAGITTAL:
						if (m_pVolumeData->isMaskBit(m_depth, res.x(), res.y(), m_mask, m_maskIndex) == true)
							m_list.push_back(res);
						break;
					case WT_AXIAL:
					default:
						if (m_pVolumeData->isMaskBit(res.x(), res.y(), m_depth, m_mask, m_maskIndex) == true)
							m_list.push_back(res);
						break;
					}
				}
			}
		}
	}

	if (merge == false)
		s_id++;

}

bool ActionUnMarking::mergeWith(const QUndoCommand *command)
{
	const ActionUnMarking *markingCommand = static_cast<const ActionUnMarking *>(command);

	if (markingCommand == NULL) return false;

	if (m_id != markingCommand->id()) return false;

	for (auto a = markingCommand->getList().begin(); a != markingCommand->getList().end(); ++a)
	{
		m_list.push_back(*a);
	}

	return true;
}

void ActionUnMarking::undo()
{
	m_pVolumeData->setMaskBitList(m_type, &m_list, m_depth, m_mask, m_maskIndex);
	//	m_pVolumeData->voxelCount[m_maskNumber] += m_pVolumeData->fillMaskCount;
	//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
	m_pVolumeData->setVoxelCount(m_maskNumber, m_pVolumeData->fillMaskCount, false);
	m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	m_pVolumeData->setTAState(m_maskNumber, m_TAState);
	WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	WIN_MANAGER->setSaveState(false);
}

void ActionUnMarking::redo()
{
	m_pVolumeData->delMaskBitList(m_type, &m_list, m_depth, m_mask, m_maskIndex);
	//	m_pVolumeData->voxelCount[m_maskNumber] += m_pVolumeData->fillMaskCount;
	m_pVolumeData->setVoxelCount(m_maskNumber, m_pVolumeData->fillMaskCount, false);
	if (!m_orgbox[1].isFitROI())
	{
		m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
		//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
		m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);
	}
	//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
	else
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	m_pVolumeData->setTAState(m_maskNumber, false);
	WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	WIN_MANAGER->setSaveState(false);
}




#if 0
ActionUnMarking2::ActionUnMarking2(const QPolygon & list, WindowBase* view, std::vector<mask>& maskList, bool merge, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_maskList.assign(maskList.begin(), maskList.end());
	m_id = s_id;

	if (view != NULL)
	{
		mSlice = view->getDepth();

		QPoint vP;
		m_list.reserve(list.size() * 2);
		for (auto m = m_maskList.begin(); m != m_maskList.end(); ++m)
		{
			std::vector<QPoint> * pList = new std::vector<QPoint>;
			pList->reserve(list.size());
			for (auto a = list.begin(); a != list.end(); ++a)
			{
				vP = m_view->screenToVolumePosition(a);

				if (m_pVolumeData->isMaskBit(m_view->getType(), vP.x(), vP.y(), m_view->getDepth(), (*m)) == true)
				{
					pList->push_back(vP);
				}
			}

			m_list.push_back(pList);
		}

		if (merge == false)
			s_id++;
	}
}

ActionUnMarking2::ActionUnMarking2(const QPoint screen_point, const QPolygon & list, WindowBase* view, std::vector<mask>& maskList, bool merge, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_UNMARKING;
	m_view = view;
	m_maskList.assign(maskList.begin(), maskList.end());
	m_id = s_id;

	if (view != NULL)
	{
		m_list.reserve(list.size() * 2);
		QPoint centerP = m_view->screenToVolumePosition(&screen_point);
		QPoint p;
		m_list.reserve(list.size() * 2);
		for (auto m = m_maskList.begin(); m != m_maskList.end(); ++m)
		{
			std::vector<QPoint> * pList = new std::vector<QPoint>;
			pList->reserve(list.size());
			for (auto a = list.begin(); a != list.end(); ++a)
			{
				p = QPoint(centerP.x() + a->x(), centerP.y() + a->y());
				if (m_pVolumeData->isMaskBit(m_view->getType(), p.x(), p.y(), m_view->getDepth(), (*m)) == true)
				{
					pList->push_back(p);
				}
			}

			m_list.push_back(pList);
		}

		if (merge == false)
			s_id++;
	}
}

bool ActionUnMarking2::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionUnMarking2::undo()
{
	for (int m = 0; m < m_maskList.size(); ++m)
	{
		std::vector<QPoint> * pList = m_list[m];
		m_pVolumeData->setMaskBitList(m_view->getType(), pList, m_view->getDepth(), m_maskList[m]);
	}
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionUnMarking2::redo()
{
	for (int m = 0; m < m_maskList.size(); ++m)
	{
		std::vector<QPoint> * pList = m_list[m];
		m_pVolumeData->delMaskBitList(m_view->getType(), pList, m_view->getDepth(), m_maskList[m]);
	}
	WIN_MANAGER->renderLater_GridView(false);
}


#endif


bool checkInRegion(QRegion & region, QPoint point)
{
	return region.contains(point);
	//int rectCount = region.rectCount();

	//for ( auto rect = region.begin() ; rect != region.end() ; ++rect )
	//{
	//	const QRect & r = (*rect);

	//	//if (r.contains(_point)) return true;
	//	if (_point.x() >= r.left() && _point.x() <= r.right() &&
	//		_point.y() >= r.top() && _point.y() <= r.bottom()) return true;
	//}

	//return false;
}


ActionPolyFill::ActionPolyFill(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m, bool del, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_POLYFILL;
	m_mask = _m;
	m_maskIndex = _mI;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_id = s_id++;
	m_del = del;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	std::vector<sPOINT> buff;
	m_pVolumeData->fillMaskCount = 0;
	if (view != NULL)
	{
		m_type = view->getType();

		m_bufferCount = m_pVolumeData->createTempMaskSlice(m_type);
		m_depth = view->getDepth();

		if (m_bufferCount > 0)
		{
			QRegion region(list, Qt::WindingFill);

			unsigned int c_x;
			unsigned int c_y;
			unsigned int c_depth;
			m_pVolumeData->getLengthForScreen(view->getType(), c_x, c_y, c_depth);
			mask * mask_data = m_pVolumeData->getMaskDataPoint(m_maskIndex);

			QVector3D tpoint;
			muint32 * slice = m_pVolumeData->pData2D_Mask_Temp_SliceIndex;
			mask * slice_data = m_pVolumeData->pData2D_Mask_Temp_SliceData;
			muint32 index;

			QVector3D size = (view->volumeToScreenPositionAuto(1, 1, m_depth) - view->volumeToScreenPositionAuto(0, 0, m_depth));
			QPoint p1, p2, p3, p4;
			for (int y = 0; y < c_y; y++)
			{
				for (int x = 0; x < c_x; x++)
				{
					tpoint = view->volumeToScreenPositionAuto(x, y, m_depth);

					if (view->getFlipY() == false)
					{
						p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() + size.y()));
						p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() + size.y()));
						p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
						p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
					}
					else
					{
						p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() - size.y()));
						p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() - size.y()));
						p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
						p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
					}

					if (region.contains(p1) || region.contains(p2) | region.contains(p3) || region.contains(p4))
					{
						del ? (m_pVolumeData->fillMaskCount)-- : (m_pVolumeData->fillMaskCount)++;
						slice[c_x*y + x] = m_pVolumeData->getVolumeIndex(m_type, x, y, m_depth);
						slice_data[c_x*y + x] = mask_data[slice[c_x*y + x]];
					}
				}
			}
		}
	}
}

bool ActionPolyFill::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionPolyFill::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(false);
		}
		//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
		m_pVolumeData->setTAState(m_maskNumber, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}

void ActionPolyFill::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
				file.close();
			}

			if (m_del)
				m_pVolumeData->applyTempMaskSliceDel(m_type, m_mask, m_maskIndex, m_bufferCount);
			else
				m_pVolumeData->applyTempMaskSliceAdd(m_type, m_mask, m_maskIndex, m_bufferCount);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
			//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
				file.close();
			}
		}
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}



ActionDrawFill::ActionDrawFill(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m, bool del, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{ 
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_DRAWFILL;
	m_bufferCount = 0;
	m_mask = _m;
	m_maskIndex = _mI;
	m_id = s_id++;
	m_del = del;
	m_pVolumeData->fillMaskCount = 0;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
	if (view != NULL)
	{
		m_type = view->getType();

		m_bufferCount = m_pVolumeData->createTempMaskSlice(m_type);
		m_depth = view->getDepth();

		if (m_bufferCount > 0)
		{
			QRegion region(list, Qt::WindingFill);

			unsigned int c_x;
			unsigned int c_y;
			unsigned int c_depth;
			m_pVolumeData->getLengthForScreen(view->getType(), c_x, c_y, c_depth);
			mask * mask_data = m_pVolumeData->getMaskDataPoint(m_maskIndex);

			QVector3D tpoint;
			muint32 * slice = m_pVolumeData->pData2D_Mask_Temp_SliceIndex;
			mask * slice_data = m_pVolumeData->pData2D_Mask_Temp_SliceData;
			muint32 index;
			QVector3D size = (view->volumeToScreenPositionAuto(1, 1, m_depth) - view->volumeToScreenPositionAuto(0, 0, m_depth));
			QPoint p1, p2, p3, p4;
			for (int y = 0; y < c_y; y++)
			{
				for (int x = 0; x < c_x; x++)
				{
					tpoint = view->volumeToScreenPositionAuto(x, y, m_depth);

					if (view->getFlipY() == false)
					{
						p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() + size.y()));
						p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() + size.y()));
						p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
						p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
					}
					else
					{
						p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() - size.y()));
						p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() - size.y()));
						p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
						p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
					}

					if (region.contains(p1) || region.contains(p2) | region.contains(p3) || region.contains(p4))
					{
						del ? (m_pVolumeData->fillMaskCount)-- : (m_pVolumeData->fillMaskCount)++;
						slice[c_x*y + x] = m_pVolumeData->getVolumeIndex(m_type, x, y, m_depth);
						slice_data[c_x*y + x] = mask_data[slice[c_x*y + x]];
					}
				}
			}
		}
	}
	m_fillCount = (m_pVolumeData->fillMaskCount);
}

bool ActionDrawFill::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionDrawFill::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(false);
		}
		//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
		m_pVolumeData->setTAState(m_maskNumber, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}

void ActionDrawFill::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32)); //바뀔 데이터 인덱스 집합
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32)); //바뀔 데이터 인덱스 집합
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);//바뀔 데이터의 현재 값 기록
				file.close();
			}

			//바뀔 데이터 값 획득 (m_mask or zero)
			if (m_del)
				m_pVolumeData->applyTempMaskSliceDel(m_type, m_mask, m_maskIndex, m_bufferCount);
			else
				m_pVolumeData->applyTempMaskSliceAdd(m_type, m_mask, m_maskIndex, m_bufferCount);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
			//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
				file.close();
			}
		}

		m_pVolumeData->forceUpdateMaskVolume();
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}


ActionDrawSeedFromMask::ActionDrawSeedFromMask(VOLUME_DATA* pVolumeData, mask _m, bool foreseed, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_DRAWSEED;
	m_mask = _m;
	m_id = s_id++;
	m_maskIndex = _mI;
	m_foreseed = foreseed;
	m_voxel[0] = m_pVolumeData->getVoxelCount(m_foreseed, true);
	m_box[0] = m_pVolumeData->getBoundingBox(m_foreseed, true);
}

bool ActionDrawSeedFromMask::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionDrawSeedFromMask::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask();
			m_pVolumeData->setVoxelCount(m_foreseed, m_voxel[0], true, true);
			m_pVolumeData->setBoundingBox(m_foreseed, m_box[0], false, true);
			WIN_MANAGER->applyVoxelToUI(m_mask - 1, true);
			WIN_MANAGER->forceUpdate2DViewData(false, true);
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView(false);
			WIN_MANAGER->setSaveState(false);

		}
	}
}

void ActionDrawSeedFromMask::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask();

			m_pVolumeData->setVoxelCount(m_foreseed, m_voxel[1], true, true);
			m_pVolumeData->setBoundingBox(m_foreseed, m_box[1], false, true);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			m_pVolumeData->applyMaskAdd(m_mask, m_foreseed ? VM_MASK1 : VM_MASK0, m_maskIndex, 0);
			m_pVolumeData->setVoxelCount(m_foreseed, m_pVolumeData->fillMaskCount, false, true);
			m_pVolumeData->updateUIDBoundingBox(m_foreseed, true, true);
			m_box[1] = m_pVolumeData->getBoundingBox(m_foreseed, true);
			m_voxel[1] = m_pVolumeData->getVoxelCount(m_foreseed, true);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		WIN_MANAGER->applyVoxelToUI(m_mask - 1, true);
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}






ActionDrawFill3D::ActionDrawFill3D(VOLUME_DATA* pVolumeData, mask _m, bool del, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_DRAWFILL3D;
	m_mask = _m;
	m_maskIndex = _mI;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_id = s_id++;
	m_del = del;
	m_fillCount = m_pVolumeData->fillMaskCount;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);
}

bool ActionDrawFill3D::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionDrawFill3D::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(true);
		}
		//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
		m_pVolumeData->setTAState(m_maskNumber, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionDrawFill3D::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			if (m_del)
				m_pVolumeData->applyTempMaskBitDel(m_mask, m_maskIndex);
			else
				m_pVolumeData->applyTempMaskBitAdd(m_mask, m_maskIndex);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
			//		m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];

			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		m_pVolumeData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
		WIN_MANAGER->setSaveState(false);
	}
}

void WorkDrawFill3D::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkDrawFill3D::threadRun()
{
	mip::VECTOR3 min, max;
	min.x = MAX_float;
	min.y = MAX_float;
	max.x = MIN_float;
	max.y = MIN_float;
	float X, Y;
	m_pVolumeData->fillMaskCount = 0;
	setProgressValue(0, true);
	for (auto a = _list.begin(); a != _list.end(); ++a)
	{
		X = a->x();
		Y = a->y();

		if (X < min.x) min.x = X;
		if (X > max.x) max.x = X;
		if (Y < min.y) min.y = Y;
		if (Y > max.y) max.y = Y;
	}


	if (1)
	{
		min = _view->getScreenToProj(min.x, min.y);
		max = _view->getScreenToProj(max.x, max.y);

		mip::OBB_CONV obb;
		obb.setProjMinMax(min, max);
		mip::MATRIX44 mat = _view->getWVP();
		mat.inverse();
		obb.transform(mat);
		obb.buildPlanes();

		QRegion region(_list, Qt::WindingFill);
		//view->m_guidebox = box;

		mip::VECTOR3 size;

		BoundingBoxI box = m_pVolumeData->getBoundingBox(_uid);


		size.x = m_pVolumeData->getSizeX();
		size.y = m_pVolumeData->getSizeY();
		size.z = m_pVolumeData->getSizeZ();

		muint32 xlen = m_pVolumeData->getCX();
		muint32 ylen = m_pVolumeData->getCY();
		muint32 zlen = m_pVolumeData->getCZ();

		{
			bool check = false;
			int _total = box.maxZ - box.minZ;
			_total++;

			for (int z = box.minZ; z <= box.maxZ; z++)
			{
				if (m_pVolumeData->threadStop == true)
				{
					emit finished();
					return;
				}

				setProgressValue(z / float(_total) * 100);

				for (int y = box.minY; y <= box.maxY; y++)
				{
					for (int x = box.minX; x <= box.maxX; x++)
					{
						check = !_del;

						if (m_pVolumeData->isMaskBit(x, y, z, _mask, _maskIndex)) check = _del; // fast filtering

						if (check == true)
						{
							float fx = ((x / float(xlen - 1)) - 0.5f) * size.x;
							float fy = ((y / float(ylen - 1)) - 0.5f) * size.y;
							float fz = ((z / float(zlen - 1)) - 0.5f) * size.z;

							mip::VECTOR3 p(fx, fy, fz);
							if (obb.planes[0].dot(p) > 0) continue;
							if (obb.planes[1].dot(p) > 0) continue;
							if (obb.planes[2].dot(p) > 0) continue;
							if (obb.planes[3].dot(p) > 0) continue;

							mip::VECTOR3 r = _view->getLocalToScreen(fx, fy, fz);
							if (region.contains(QPoint(r.x, r.y)))
							{
								_del ? (m_pVolumeData->fillMaskCount)-- : (m_pVolumeData->fillMaskCount)++;
								m_pVolumeData->setTempMaskBit(x, y, z, _mask);
							}
						}

						//m_pVolumeData->setTempMaskBitAuto(WT_AXIAL, x, y, m_depth, m_mask);
					}
				}
			}
		}
	}

	emit finished();
}


ActionInterDraw::ActionInterDraw(VOLUME_DATA* pVolumeData, mask _m /*= VM_MASK2*/, int _mI /*= 0*/, bool _del /*= false*/, QUndoCommand *parent /*=0*/)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_INTER_SELECTION;

	m_id = s_id++;
	m_mask = _m;
	m_maskIndex = _mI;
	m_del = _del;
	m_drawcut = (m_mask & (VM_MASK0 | VM_MASK1)) && (m_maskIndex == 0);
	m_maskNumber = m_drawcut ? (m_mask - 1) : m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber, m_drawcut);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawcut);
	m_orgbox[1].reset(true);

	m_fillCount = 0;
}

void ActionInterDraw::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();

			file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			m_pVolumeData->forceUpdateMaskVolume();


			//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
			//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
			m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false, m_drawcut);
			m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0], false, m_drawcut);
			m_pVolumeData->setTAState(m_maskNumber, m_TAState, m_drawcut);
			WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawcut);
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView();

			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionInterDraw::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
			file.close();

			if (m_del)
				m_pVolumeData->applyTempMaskBitDel(m_mask, m_maskIndex);
			else
				m_pVolumeData->applyTempMaskBitAdd(m_mask, m_maskIndex);

		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			if (m_del)
				m_pVolumeData->applyTempMaskBitDel(m_mask, m_maskIndex);
			else
				m_pVolumeData->applyTempMaskBitAdd(m_mask, m_maskIndex);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true, m_drawcut);
			//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawcut);
		}

		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false, m_drawcut);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1], m_drawcut);

		m_pVolumeData->forceUpdateMaskVolume();
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		m_pVolumeData->setTAState(m_maskNumber, false, m_drawcut);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawcut);
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView();

	}

	WIN_MANAGER->setSaveState(false);
}

bool ActionInterDraw::mergeWith(const QUndoCommand * command)
{
	return false;
}

void WorkInterSelect::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkInterSelect::threadRun()
{
	int n = 0;
	std::vector<mip::VECTOR3> tempList;
	tempList.clear();
	WINDOW_TYPE _type = _view->getType();
	VOLUME_DATA * _volumData = m_pVolumeData;
	_volumData->getMPRPPlanes(_type, tempList);

	if (tempList.size() < 4) return;

	float xSpace, ySpace;
	uint xMax, yMax;
	mip::VECTOR3 xDir = (tempList[3] - tempList[0]).normalize();
	mip::VECTOR3 yDir = (tempList[1] - tempList[0]).normalize();
	mip::VECTOR3 zDir = (tempList[0] - tempList[2]).normalize();
	mip::VECTOR3 start;

	switch (_type)
	{
	case WT_AXIAL:
		yDir = -yDir;
		xSpace = _volumData->getSpaceX();
		ySpace = _volumData->getSpaceY();
		xMax = _volumData->getCX();
		yMax = _volumData->getCY();
		start = tempList[1] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	case WT_CORONAL:
		xSpace = _volumData->getSpaceX();
		ySpace = _volumData->getSpaceZ();
		xMax = _volumData->getCX();
		yMax = _volumData->getCZ();
		start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	case WT_SAGITTAL:
		xSpace = _volumData->getSpaceY();
		ySpace = _volumData->getSpaceZ();
		xMax = _volumData->getCY();
		yMax = _volumData->getCZ();
		start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	default: return;
		break;
	}

	mip::AABB box3D = _volumData->getBoundingBox3DAABB();
	std::vector<mint16> huPoints;
	std::vector<mip::VECTOR3> volPoints;

	mip::VECTOR3 point;

	mip::Slice * pCam = &(_view->getCamera());
	mip::MATRIX44 matView = pCam->getView();
	mip::MATRIX44 matProj = pCam->getProj();

	int w, h;
	mint16 hu, huMin, huMax;

	huMin = MAX_int16;
	huMax = MIN_int16;

	w = _view->width();
	h = _view->height();

	volPoints.clear();
	int _progress = 0;
	setProgressValue(_progress, true);

	if (!_oval)
	{
		QRegion region(_list, Qt::WindingFill);

		//_list thick
		if (!_line)
		{
			_progress = 40;
			for (int y = 0; y < yMax; y++)
			{
				if (_volumData->threadStop) break;

				point = start;
				for (int x = 0; x < xMax; x++)
				{
					//	if (box3D.checkPoint(point) == true)
					{
						mip::VECTOR3 v1 = mip::geom::WorldToScreen(point, w, h, matView, matProj);

						// 220713 허 건 과장 주석처리
						// zoom 확대 시, coninue로 빠져서 폴리곤 영역 체크가 안됨
						{
							//if (!(0 <= v1.x && v1.x < w))
							//	continue;
							//else if (!(0 <= v1.y && v1.y < h))
							//	continue;
						}


						bool res = false;

						if (region.contains(QPoint(v1.x, v1.y)))
							res = true;

						if (res)
						{
							mip::VECTOR3 value = _view->getWorldToVolume(point.x, point.y, point.z);
							volPoints.push_back(value);
						}
					}
					point += xDir * xSpace;
				}

				start += yDir * ySpace;

				setProgressValue(y / (float)yMax * _progress);
			}//40
		}
		else
			_progress = 40;

		std::vector<mip::VECTOR3> initPlane;

		mip::AABB box3D = m_pVolumeData->getBoundingBox3DAABB();

		const WINDOW_TYPE _type = _view->getType();


		for (int i = 0; i < _list.size(); i++)
		{
			int _x, _y;
			bool res = false;
			_x = _list.at(i).x();
			_y = _list.at(i).y();
			mip::VECTOR3 currntPoint1 = pCam->getWorldPoint(_x, _y, 0);
			mip::VECTOR3 currntPoint2 = pCam->getWorldPoint(_x, _y, 1);

			mip::PLANE	pl(tempList[0], tempList[1], tempList[2]);

			int RayResult = mip::geom::Intersect_LinePlane(pl, currntPoint1, currntPoint2, &point);

			if (RayResult > 0)
			{
				if (box3D.checkPoint(point) == true)
				{
					if (box3D.min.x > point.x)
						continue;
					else if (box3D.max.x < point.x)
						continue;

					if (box3D.min.y > point.y)
						continue;
					else if (box3D.max.y < point.y)
						continue;

					if (box3D.min.z > point.z)
						continue;
					else if (box3D.max.z < point.z)
						continue;

					res = true;

				}
			}

			mip::VECTOR3 value = _view->getWorldToVolume(point.x, point.y, point.z);
			volPoints.push_back(value);

			setProgressValue((float)i / _list.size() * _progress);
		}

	}
	else
	{
		QPainterPath region;
		QRect rect;

		rect.setCoords(_list.at(0).x(), _list.at(0).y(),
			_list.at(1).x(), _list.at(1).y());

		if (_circle)
		{
			QPoint cent = rect.center();
			double radi = rect.width() < rect.height() ? rect.height() : rect.width();

			rect.setWidth(radi);
			rect.setHeight(radi);
		}

		region.addEllipse(rect);
		_progress = 40;
		for (int y = 0; y < yMax; y++)
		{
			if (_volumData->threadStop) break;

			point = start;
			for (int x = 0; x < xMax; x++)
			{
				if (box3D.checkPoint(point) == true)
				{
					mip::VECTOR3 v1 = mip::geom::WorldToScreen(point, w, h, matView, matProj);

					if (region.contains(QPoint(v1.x, v1.y)))
					{
						mip::VECTOR3 value = _view->getWorldToVolume(point.x, point.y, point.z);
						volPoints.push_back(value);
					}
				}
				point += xDir * xSpace;
			}

			start += yDir * ySpace;

			setProgressValue(y / (float)yMax * _progress);
		}//40
	}

	std::vector<mip::VECTOR3>::iterator pos = std::unique(volPoints.begin(), volPoints.end());
	volPoints.erase(pos, volPoints.end());

	int c_x, c_y, c_z, length;

	c_x = _volumData->getCX();
	c_y = _volumData->getCY();
	c_z = _volumData->getCZ();
	length = _volumData->getVolumeDataLength();

	mask *_dt = _volumData->getMaskTempDataPoint();

	bool _sel = false;

	for (int i = 0; i < volPoints.size(); i++)
	{
		mip::VECTOR3 p = volPoints.at(i);
		mip::VECTOR3 p2 = p;

		p2.x = qCeil(p2.x);
		p2.y = qCeil(p2.y);
		p2.z = qCeil(p2.z);

		p.x = qFloor(p.x);
		p.y = qFloor(p.y);
		p.z = qFloor(p.z);

		for (int z = p.z; z <= p2.z; z++)
		{
			if (_volumData->threadStop) break;
			if (z >= c_z || z < 0) continue;

			for (int y = p.y; y <= p2.y; y++)
			{
				if (y >= c_y || y < 0) continue;
				for (int x = p.x; x <= p2.x; x++)
				{
					if (x >= c_x || x < 0) continue;

					int index = (z * c_x * c_y) + (y * c_x) + x;

					if (!(index < 0 ||
						index >= (length)))
					{
						if (_dt[index] != _mask)
						{
							_dt[index] = _mask;

							if (!_sel) _sel = true;
						}
					}
				}
			}
		}

		setProgressValue(_progress + (i / (float)volPoints.size() * 60));
	}

	_volumData->threadStop = !_sel;

	emit finished();
}

ActionOvalFill::ActionOvalFill(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase * view, mask _m, int _mI, bool del, QUndoCommand *parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_OVALFILL;
	m_mask = _m;
	m_maskIndex = _mI;
	m_maskNumber = m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	m_id = s_id++;
	m_del = del;
	
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_orgbox[1].reset(true);

	std::vector<sPOINT> buff;
	m_pVolumeData->fillMaskCount = 0;
	if (view != NULL)
	{
		m_type = view->getType();

		m_bufferCount = m_pVolumeData->createTempMaskSlice(m_type);
		m_depth = view->getDepth();

		QPainterPath region;
		QRect rect;

		rect.setCoords(list.at(0).x(), list.at(0).y(),
			list.at(1).x(), list.at(1).y());

		region.addEllipse(rect);

		unsigned int c_x;
		unsigned int c_y;
		unsigned int c_depth;
		m_pVolumeData->getLengthForScreen(view->getType(), c_x, c_y, c_depth);
		mask * mask_data = m_pVolumeData->getMaskDataPoint(m_maskIndex);

		QVector3D tpoint;
		muint32 * slice = m_pVolumeData->pData2D_Mask_Temp_SliceIndex;
		mask * slice_data = m_pVolumeData->pData2D_Mask_Temp_SliceData;
		muint32 index;

		QVector3D size = (view->volumeToScreenPositionAuto(1, 1, m_depth) - view->volumeToScreenPositionAuto(0, 0, m_depth));
		QPoint p1, p2, p3, p4;
		for (int y = 0; y < c_y; y++)
		{
			for (int x = 0; x < c_x; x++)
			{
				tpoint = view->volumeToScreenPositionAuto(x, y, m_depth);

				if (view->getFlipY() == false)
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() + size.y()));
					p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() + size.y()));
					p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}
				else
				{
					p1 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y() - size.y()));
					p2 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y() - size.y()));
					p3 = QPoint(qFloor(tpoint.x() + size.x()), qFloor(tpoint.y()));
					p4 = QPoint(qFloor(tpoint.x()), qFloor(tpoint.y()));
				}

				if (region.contains(p1) || region.contains(p2) | region.contains(p3) || region.contains(p4) )
				{
					// getVolumeIndex(m_type, x, y, m_depth);
					del ? (m_pVolumeData->fillMaskCount)-- : (m_pVolumeData->fillMaskCount)++;
					slice[c_x*y + x] = m_pVolumeData->getVolumeIndex(m_type, x, y, m_depth);
					slice_data[c_x*y + x] = mask_data[slice[c_x*y + x]];
				}
			}
		}
	}

}

void ActionOvalFill::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(false);
		}
		//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
		m_pVolumeData->setTAState(m_maskNumber, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}

void ActionOvalFill::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskSlice(m_type);
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
			file.read((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
			file.close();

			m_pVolumeData->applyTempMaskSlice(m_maskIndex, m_bufferCount);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
				file.close();
			}

			if (m_del)
				m_pVolumeData->applyTempMaskSliceDel(m_type, m_mask, m_maskIndex, m_bufferCount);
			else
				m_pVolumeData->applyTempMaskSliceAdd(m_type, m_mask, m_maskIndex, m_bufferCount);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
			//		m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceIndex, m_bufferCount * sizeof(muint32));
				file.write((char*)m_pVolumeData->pData2D_Mask_Temp_SliceData, m_bufferCount);
				file.close();
			}
		}
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	}

	m_pVolumeData->clearTempMaskSlice();
	WIN_MANAGER->setSaveState(false);
}

bool ActionOvalFill::mergeWith(const QUndoCommand * command)
{
	return false;
}

void WorkInterRect::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkInterRect::threadRun()
{
	int n = 0;
	std::vector<mip::VECTOR3> tempList;
	tempList.clear();
	WINDOW_TYPE _type = m_view->getType();
	VOLUME_DATA * _volumData = m_pVolumeData;
	_volumData->getMPRPPlanes(_type, tempList);

	if (tempList.size() < 4) return;

	float xSpace, ySpace;
	uint xMax, yMax;
	mip::VECTOR3 xDir = (tempList[3] - tempList[0]).normalize();
	mip::VECTOR3 yDir = (tempList[1] - tempList[0]).normalize();
	mint16 * planeData = 0;
	mip::VECTOR3 start;

	switch (_type)
	{
	case WT_AXIAL:
		yDir = -yDir;
		xSpace = _volumData->getSpaceX();
		ySpace = _volumData->getSpaceY();
		xMax = _volumData->getCX();
		yMax = _volumData->getCY();
		planeData = _volumData->axialPlaneData[1];
		start = tempList[1] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	case WT_CORONAL:
		xSpace = _volumData->getSpaceX();
		ySpace = _volumData->getSpaceZ();
		xMax = _volumData->getCX();
		yMax = _volumData->getCZ();
		planeData = _volumData->coronalPlaneData[1];
		start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	case WT_SAGITTAL:
		xSpace = _volumData->getSpaceY();
		ySpace = _volumData->getSpaceZ();
		xMax = _volumData->getCY();
		yMax = _volumData->getCZ();
		planeData = _volumData->sagittalPlaneData[1];
		start = tempList[0] + (xDir * 0.5f * xSpace) + (yDir * 0.5f * ySpace);
		break;
	default: return;
		break;
	}

	mip::AABB box3D = _volumData->getBoundingBox3DAABB();
	std::vector<mint16> huPoints;
	std::vector<mip::VECTOR3> volPoints;

	mip::VECTOR3 point;

	mip::Slice * pCam = &(m_view->getCamera());
	mip::MATRIX44 matView = pCam->getView();
	mip::MATRIX44 matProj = pCam->getProj();

	int w, h;
	mint16 hu, huMin, huMax;

	huMin = MAX_int16;
	huMax = MIN_int16;

	w = m_view->width();
	h = m_view->height();

	volPoints.clear();
	setProgressValue(0, true);

	m_region.setFillRule(Qt::WindingFill);
	QPolygonF poly = m_region.toFillPolygon();
	QRegion region(poly.toPolygon(), Qt::WindingFill);

	for (int y = 0; y < yMax; y++)
	{
		if (_volumData->threadStop) break;

		point = start;
		for (int x = 0; x < xMax; x++)
		{
			if (box3D.checkPoint(point) == true)
			{
				mip::VECTOR3 v1 = mip::geom::WorldToScreen(point, w, h, matView, matProj);

				if (region.contains(QPoint(v1.x, v1.y)))
				{
					mip::VECTOR3 value = m_view->getWorldToVolume(point.x, point.y, point.z);
					volPoints.push_back(value);
				}
			}
			point += xDir * xSpace;
		}

		start += yDir * ySpace;

		setProgressValue(y / (float)yMax * 40);
	}//40

	std::vector<mip::VECTOR3>::iterator pos = std::unique(volPoints.begin(), volPoints.end());
	volPoints.erase(pos, volPoints.end());

	int c_x, c_y, c_z, length;

	c_x = _volumData->getCX();
	c_y = _volumData->getCY();
	c_z = _volumData->getCZ();
	length = _volumData->getVolumeDataLength();

	mask *_dt = _volumData->getMaskTempDataPoint();

	bool _sel = false;

	for (int i = 0; i < volPoints.size(); i++)
	{
		mip::VECTOR3 p = volPoints.at(i);
		mip::VECTOR3 p2 = p;

		p2.x = qCeil(p2.x);
		p2.y = qCeil(p2.y);
		p2.z = qCeil(p2.z);

		p.x = qFloor(p.x);
		p.y = qFloor(p.y);
		p.z = qFloor(p.z);

		for (int z = p.z; z <= p2.z; z++)
		{
			if (_volumData->threadStop) break;
			if (z >= c_z || z < 0) continue;

			for (int y = p.y; y <= p2.y; y++)
			{
				if (y >= c_y || y < 0) continue;
				for (int x = p.x; x <= p2.x; x++)
				{
					if (x >= c_x || x < 0) continue;

					int index = (z * c_x * c_y) + (y * c_x) + x;

					if (!(index < 0 ||
						index >= (length)))
					{
						if (_dt[index] != m_mask)
						{
							_dt[index] = m_mask;

							if (!_sel) _sel = true;
						}
					}
				}
			}
		}

		setProgressValue(40 + (i / (float)volPoints.size() * 60));
	}

	_volumData->threadStop = !_sel;

	emit finished();
}

ActionInterRect::ActionInterRect(VOLUME_DATA* pVolumeData, mask _m, int _mI, bool _del, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_IP_INTER_RECT_SELECTION;

	m_id = s_id++;
	m_mask = _m;
	m_maskIndex = _mI;
	m_del = _del;
	m_drawcut = (m_maskIndex == 0) && (m_mask <= VM_MASK1) ? true : false;
	m_maskNumber = m_drawcut ? (m_mask - 1) : m_pVolumeData->getCurrentMaskInfo()->uid;
	m_TAState = m_pVolumeData->getTAState(m_mask, m_drawcut);
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawcut);
	m_orgbox[1].reset(true);

	m_fillCount = 0;
}

void ActionInterRect::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();

			file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			m_pVolumeData->forceUpdateMaskVolume();

			m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false, m_drawcut);
			m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0], false, m_drawcut);
			WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawcut);
			m_pVolumeData->setTAState(m_maskNumber, m_TAState, m_drawcut);
			WIN_MANAGER->renderLater_GridView();

			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionInterRect::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
			file.close();

			if (m_del)
				m_pVolumeData->applyTempMaskBitDel(m_mask, m_maskIndex);
			else
				m_pVolumeData->applyTempMaskBitAdd(m_mask, m_maskIndex);

		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			if (m_del)
				m_pVolumeData->applyTempMaskBitDel(m_mask, m_maskIndex);
			else
				m_pVolumeData->applyTempMaskBitAdd(m_mask, m_maskIndex);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true, m_drawcut);
			//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber, m_drawcut);
		}

		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1], false, m_drawcut);
		m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false, m_drawcut);
		m_pVolumeData->forceUpdateMaskVolume();
		//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox[1];
		//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
		m_pVolumeData->setTAState(m_maskNumber, false, m_drawcut);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber, m_drawcut);
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView();

	}

	WIN_MANAGER->setSaveState(false);
}

bool ActionInterRect::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionSplitting3D::ActionSplitting3D(VOLUME_DATA* pVolumeData, int uid, int splitCount, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_SPLIT3D;

	m_id = s_id++;

	m_maskNumber = uid;
	m_Split = (splitCount - 1);

	m_Splitinfo.reserve(m_Split);
	m_Splitlist.reserve(m_Split);
	m_SplitFill.reserve(m_Split);
	m_SplitBox.reserve(m_Split);
	m_Splituid.reserve(m_Split);

	MaskInfo *info = m_pVolumeData->getMaskInfo(m_maskNumber, true);

	m_SplitIndex[0] = m_pVolumeData->GetMaskPointCount();
	m_SplitIndex[1] = m_maskIndex = m_maskNumber < MASK_SECOND_MAX ?
		0 : (m_maskNumber - MASK_SECOND_MAX) / 8 + 1;
	m_mask = m_maskIndex == 0 ? info->mask_id : info->mask_id2;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	m_orgbox[0] = m_pVolumeData->getBoundingBox(uid);

}

void ActionSplitting3D::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		bool res;
		//DEL
		std::vector<UIDMask> vec;
		m_pVolumeData->delMaskInfos(m_Splituid, vec, res);
		m_pVolumeData->setCurrentMaskIndex(0);
		MaskInfo * info = m_pVolumeData->getMaskInfo(0);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_SplitIndex[0]; i++)
			{
				m_pVolumeData->createTempMaskData();
				file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
				m_pVolumeData->applyTempMask(i);
			}
			file.close();

			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(true);
		}
		for (int i = 0; i < m_Split; i++)
		{
			int uid = m_Splitinfo[i].uid;

			m_pVolumeData->setBoundingBox(uid, BoundingBoxI(), true);
			m_pVolumeData->setVoxelCount(uid, 0);
			m_pVolumeData->setTAState(uid, false);
		}

		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[0]);
		m_pVolumeData->setTAState(m_maskNumber, m_TAState);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}

}

void ActionSplitting3D::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_SplitIndex[1]; i++)
			{
				m_pVolumeData->createTempMaskData();
				file.read((char*)m_pVolumeData->getMaskTempDataPoint(), m_pVolumeData->getVolumeDataLength());
				m_pVolumeData->applyTempMask(i);
			}

			file.close();

			for (int i = 0; i < m_Split; i++)
				m_pVolumeData->insertMaskInfo(m_Splitlist[i], m_Splitinfo[i]);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_SplitIndex[0]; i++)
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			mask mVal = VM_MASK0;

			m_pVolumeData->applyTempMaskBitChange(m_mask, m_maskIndex, mVal, 0, false);

			m_fillCount = m_pVolumeData->fillMaskCount;

			m_pVolumeData->updateUIDBoundingBox(m_maskNumber, true);
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskNumber);

			mVal <<= 1;

			for (int i = 0; i < m_Split; i++)
			{
				bool r = m_pVolumeData->createMaskInfo();

				if (r)
				{
					m_Splitlist.push_back(m_pVolumeData->getCurrentMaskInfoID());
					m_Splitinfo.push_back(*m_pVolumeData->getCurrentMaskInfo());
					m_Splituid.push_back(m_Splitinfo[i].uid);

					int _mI = m_Splitinfo[i].uid >= MASK_SECOND_MAX ? (m_Splitinfo[i].uid - MASK_SECOND_MAX) / 8 + 1 : 0;
					mask _m = _mI == 0 ? m_Splitinfo[i].mask_id : m_Splitinfo[i].mask_id2;

					m_pVolumeData->applyTempMaskBitChange(_m, _mI, mVal, 0, i == (m_Split - 1));

					m_SplitFill.push_back(m_pVolumeData->fillMaskCount);
					m_pVolumeData->updateUIDBoundingBox(m_Splitinfo[i].uid, true);
					m_SplitBox.push_back(m_pVolumeData->getBoundingBox(m_Splitinfo[i].uid));
					mVal <<= 1;
				}
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				m_SplitIndex[1] = m_pVolumeData->GetMaskPointCount();
				for (int i = 0; i < m_SplitIndex[1]; i++)
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
	}
	for (int i = 0; i < m_Split; i++)
	{
		int uid = m_Splitinfo[i].uid;

		m_pVolumeData->setBoundingBox(uid, m_SplitBox[i]);
		m_pVolumeData->setVoxelCount(uid, m_SplitFill[i], false);
		m_pVolumeData->setTAState(uid, false);
		WIN_MANAGER->applyVoxelToUI(uid);
	}

	m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
	m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox[1]);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(true);
	m_pVolumeData->setTAState(m_maskNumber, false);
	WIN_MANAGER->applyVoxelToUI(m_maskNumber);
	WIN_MANAGER->updateUI();
	WIN_MANAGER->setSaveState(false);

}

bool ActionSplitting3D::mergeWith(const QUndoCommand * command)
{
	return false;
}

void WorkSplitting3D::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkSplitting3D::threadRun()
{
	mip::VECTOR3 size;
	mip::VECTOR3 space;


	BoundingBoxI box = m_pVolumeData->getBoundingBox(_uid);
	std::vector<mip::VECTOR3> volPoints;

	size.x = m_pVolumeData->getSizeX();
	size.y = m_pVolumeData->getSizeY();
	size.z = m_pVolumeData->getSizeZ();

	space.x = m_pVolumeData->getSpaceX();
	space.y = m_pVolumeData->getSpaceY();
	space.z = m_pVolumeData->getSpaceZ();


	int c_x, c_y, c_z, length;

	c_x = m_pVolumeData->getCX();
	c_y = m_pVolumeData->getCY();
	c_z = m_pVolumeData->getCZ();
	length = m_pVolumeData->getVolumeDataLength();

	mask *_dt = m_pVolumeData->getMaskTempDataPoint();

	m_pVolumeData->fillMaskCount = 0;
	setProgressValue(0, true);
	volPoints.clear();

	VolumeView * view = ((VolumeView*)_view);

	int _total = box.maxZ - box.minZ;
	int MaskVoxel = 0;
	int LineVoxel = 0;
	_total++;
	bool _sel = false;

	if (WORK_MODE::WORK_3D_CURVE_SPLIT == _type)
	{
		float _progress = 50;
		QRegion region(_list, Qt::WindingFill);

		QPainterPath path = QPainterPath();
		path.moveTo(_list.at(0));

		QPainterPathStroker stroker;

		stroker.setCapStyle(Qt::FlatCap);

		for (auto a = _list.begin(); a != _list.end(); ++a)
			path.lineTo(QPointF(a->x(), a->y()));

		stroker.setWidth(2.3);
		path = stroker.createStroke(path);

		for (int z = box.minZ; z <= box.maxZ; z++)
		{
			if (m_pVolumeData->threadStop == true)
			{
				emit finished();
				return;
			}

			setProgressValue(((float)(z - box.minZ) / _total) * _progress);

			for (int y = box.minY; y <= box.maxY; y++)
			{
				for (int x = box.minX; x <= box.maxX; x++)
				{
					int index = (z * c_x * c_y) + (y * c_x) + x;

					if (index < 0 || index >= (length)) continue;

					_dt[index] = VM_MASK0;
					MaskVoxel++;

					float fx = ((x == 0 ? x : (x - 1) / float(c_x - 1)) - 0.5f) * size.x;
					float fy = ((y == 0 ? y : (y - 1) / float(c_y - 1)) - 0.5f) * size.y;
					float fz = ((z == 0 ? z : (z - 1) / float(c_z - 1)) - 0.5f) * size.z;

					mip::VECTOR3 p(fx, fy, fz);

					mip::VECTOR3 r = ((VolumeView*)view)->getLocalToScreen(fx, fy, fz);

					if (path.contains(QPointF(r.x, r.y)) || region.contains(QPointF(r.x, r.y).toPoint()))
					{
						r = mip::VECTOR3(x, y, z);
						volPoints.push_back(r);
					}

				}
			}
		}//Line points

		std::vector<mip::VECTOR3>::iterator pos = std::unique(volPoints.begin(), volPoints.end());
		volPoints.erase(pos, volPoints.end());

		for (int i = 0; i < volPoints.size(); i++)
		{
			mip::VECTOR3 p = volPoints.at(i);
			mip::VECTOR3 p2 = p;

			p2.x = qCeil(p2.x + 0.5);
			p2.y = qCeil(p2.y + 0.5);
			p2.z = qCeil(p2.z + 0.5);

			p.x = qFloor(p.x - 0.5);
			p.y = qFloor(p.y - 0.5);
			p.z = qFloor(p.z - 0.5);

			for (int z = p.z; z <= p2.z; z++)
			{
				if (m_pVolumeData->threadStop) break;
				if (z >= c_z || z < 0) continue;

				for (int y = p.y; y <= p2.y; y++)
				{
					if (y >= c_y || y < 0) continue;
					for (int x = p.x; x <= p2.x; x++)
					{
						if (x >= c_x || x < 0) continue;

						int index = (z * c_x * c_y) + (y * c_x) + x;

						if (!(index < 0 ||
							index >= (length)))
						{
							if (_dt[index] == VM_MASK0)
							{
								_dt[index] = VM_MASK1;
								if (!_sel)	_sel = true;
							}
						}
					}
				}
			}

			setProgressValue(_progress + (i / (float)volPoints.size() * 50));
		}//set line points 

		_progress += 50;//80

	}
	else if(WORK_MODE::WORK_3D_PLANE_SPLIT == _type)
	{
		bool _sel2 = false;
		for (int z = box.minZ; z <= box.maxZ; z++)
		{
			if (m_pVolumeData->threadStop == true)
			{
				emit finished();
				return;
			}

			setProgressValue(((float)(z - box.minZ) / _total) * 100);

			for (int y = box.minY; y <= box.maxY; y++)
			{
				for (int x = box.minX; x <= box.maxX; x++)
				{
					int index = (z * c_x * c_y) + (y * c_x) + x;

					if (index < 0 || index >= (length)) continue;
					float fx = space.x*x;
					float fy = space.y*y;
					float fz = space.z*z;
					mip::VECTOR3 p(fx, fy, fz);
					float res = _plane_1.dot(p);
					if (res >= 0.f)
					{
						if (!_sel) _sel = true;
						_dt[index] = VM_MASK0;
					}
					else
					{
						if (!_sel2) _sel2 = true;
						_dt[index] = VM_MASK1;
					}
				}
			}
		}

		if (!(_sel && _sel2)) _sel = false;
	}
	else if (WORK_MODE::WORK_3D_ANGLE_SPLIT == _type)
	{
		bool _sel2 = false;
		for (int z = box.minZ; z <= box.maxZ; z++)
		{
			if (m_pVolumeData->threadStop == true)
			{
				emit finished();
				return;
			}

			setProgressValue(((float)(z - box.minZ) / _total) * 100);

			for (int y = box.minY; y <= box.maxY; y++)
			{
				for (int x = box.minX; x <= box.maxX; x++)
				{
					int index = (z * c_x * c_y) + (y * c_x) + x;

					if (index < 0 || index >= (length)) continue;
					float fx = space.x*x;
					float fy = space.y*y;
					float fz = space.z*z;
					mip::VECTOR3 p(fx, fy, fz);
					float res_1 = _plane_1.dot(p);
					float res_2 = _plane_2.dot(p);
					if (res_1 <= 0.f || res_2 >=0.f)
					{
						if (!_sel) 
							_sel = true;
						_dt[index] = VM_MASK0;
					}
					else
					{
						if (!_sel2) 
							_sel2 = true;
						_dt[index] = VM_MASK1;
					}
				}
			}
		}

		if (!(_sel && _sel2)) _sel = false;
	}


	m_pVolumeData->threadResult = m_pVolumeData->SplitTempMaskBit(_uid, _mask, _maskIndex);

	m_pVolumeData->threadStop = !_sel;

	setProgressValue(100);

	emit finished();
}

void WorkRandomPoint::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkRandomPoint::threadRun()
{
	
	setProgressValue(0, true);

	MaskInfo* info = m_pVolumeData->getMaskInfo(_uid, true);

	int _mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	mask _m = _mI == 0 ? info->mask_id : info->mask_id2;

	mask *_res = m_pVolumeData->getMaskTempDataPoint();
	mask *_org = m_pVolumeData->getMaskDataPoint(_mI);

	muint32 cx, cy, cz;
	m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	BoundingBoxI box = m_pVolumeData->getBoundingBox(_uid);

	const int min_index = box.minZ * cx*cy + box.minY*cx + box.minX;
	const int max_index = box.maxZ * cx*cy + box.maxY*cx + box.maxX;
	const int range_index = abs(min_index - max_index) + 1;

	const int range_x = abs(box.minX - box.maxX) + 1;
	const int range_y = abs(box.minY - box.maxY) + 1;
	const int range_z = abs(box.minZ - box.maxZ) + 1;

	short *smoothing_mask_input = new short[cx*cy*cz];
	short *smoothing_mask_output = new short[cx*cy*cz];
	memset(smoothing_mask_input, 0, cx*cy*cz*sizeof(short));
	memset(smoothing_mask_output, 0, cx*cy*cz * sizeof(short));

	srand(time(NULL));

	int index = 0;
	if (!_pointunit) // 단순 1 pixel 단위로만 포인트를 생성
	{
		for (int i = 0; i < _count; i++)
		{
			for (;;)
			{
				if (m_pVolumeData->threadStop)
					break;

				int _x, _y, _z;
				_z = (rand() % range_z);
				_z += box.minZ;

				_y = (rand() % range_y);
				_y += box.minY;

				_x = (rand() % range_x);
				_x += box.minX;

				index = _z * cx*cy + _y*cx + _x;

				if (index < min_index || index > max_index)
					continue;

				if ((_org[index] & _m) && (_res[index] == 0))
				{
					_res[index] = _m;
					break;
				}
			}

			setProgressValue(100 * ((float)(i + 1) / _count));

			if (m_pVolumeData->threadStop)
				break;
		}
	}

	else // x-y-z range에 따라 타원체를 생성
	{
		for (int i = 0; i < _count; i++)
		{
			for (;;)
			{
				if (m_pVolumeData->threadStop)
					break;
				
				int _x, _y, _z;
				_z = (rand() % range_z);
				_z += box.minZ;

				_y = (rand() % range_y);
				_y += box.minY;

				_x = (rand() % range_x);
				_x += box.minX;

				index = _z * cx*cy + _y*cx + _x;

				if (index < min_index || index > max_index)
					continue;

				if ((_org[index] & _m) && (_res[index] == 0))
				{
					// 이 부분에서 타원체를 생성하는 모듈 호출


//  					int x_diameter = (_x_radius_max ) * 2 + 1;
//  					int y_diameter = (_y_radius_max ) * 2 + 1;
//  					int z_diameter = (_z_radius_max ) * 2 + 1;

					int x_diameter = (_x_radius_min + rand() % (_x_radius_max- _x_radius_min)) * 2 + 1;
					int y_diameter = (_y_radius_min + rand() % (_y_radius_max - _y_radius_min)) * 2 + 1;
					int z_diameter = (_z_radius_min + rand() % (_z_radius_max - _z_radius_min)) * 2 + 1;


 					x_diameter = (int) ((float)x_diameter /  m_pVolumeData->getSpaceX(true));
 					y_diameter = (int) ((float)y_diameter /  m_pVolumeData->getSpaceY(true));
 					z_diameter = (int) ((float)z_diameter /  m_pVolumeData->getSpaceZ(true));

					if (x_diameter % 2 == 0) x_diameter++;
					if (y_diameter % 2 == 0) y_diameter++;
					if (z_diameter % 2 == 0) z_diameter++;

					int max_diameter;
					if (x_diameter >= y_diameter) max_diameter = x_diameter;
					else if (y_diameter >= x_diameter) max_diameter = y_diameter;
					if (z_diameter >= max_diameter) max_diameter = z_diameter;

					
					unsigned char *ellipsoid = new unsigned char[x_diameter * y_diameter * z_diameter];
					mip::TA::MakeEllipsoid(ellipsoid, x_diameter, y_diameter, z_diameter);
//					mip::writeRawFile(ellipsoid, x_diameter * y_diameter * z_diameter, QString("D:\\ellipsoid.raw"));

					unsigned char *cube = new unsigned char[max_diameter * max_diameter * max_diameter];
					memset(cube, 0, max_diameter * max_diameter * max_diameter);
					
					for (int z = 0; z <z_diameter; z++)
					{
						for (int y = 0; y <y_diameter; y++)
						{
							for (int x =0; x <x_diameter; x++)
							{
								cube[(z+ (max_diameter - z_diameter)/2)* max_diameter * max_diameter + (y+ (max_diameter - y_diameter)/2)* max_diameter + x + (max_diameter - x_diameter)/2	]
									=ellipsoid[(z)* x_diameter * y_diameter + (y)* x_diameter + x];
							}
						}
					}
					unsigned char *rotation_cube = new unsigned char[max_diameter * max_diameter * max_diameter];
					memset(rotation_cube, 0, max_diameter * max_diameter * max_diameter);

					

//  					mip::Rotation3D(cube, rotation_cube, max_diameter, max_diameter, max_diameter, rand() % 360, rand() % 360, rand() % 360, 0);
// 
//  					mip::writeRawFile(cube, max_diameter * max_diameter * max_diameter, QString("D:\\cube.raw"));
//   					mip::writeRawFile(rotation_cube, max_diameter * max_diameter * max_diameter, QString("D:\\rotation_cube_x.raw"));
//  					mip::writeRawFile(rotation_cube_y, max_diameter * max_diameter * max_diameter, QString("D:\\rotation_cube_y.raw"));
//					mip::writeRawFile(rotation_cube_z, max_diameter * max_diameter * max_diameter, QString("D:\\rotation_cube_z.raw"));

					
					int max_radius = max_diameter / 2;
				
// 					for (int z = _z - _z_radius_max ; z <= _z + _z_radius_max ; z++)
// 					{
// 						for (int y = _y - _y_radius_max ; y <= _y + _y_radius_max ; y++)
// 						{
// 							for (int x = _x - _x_radius_max ; x <= _x + _x_radius_max ; x++)
					for (int z = _z - max_radius; z <= _z + max_radius; z++)
					{
						for (int y = _y - max_radius; y <= _y + max_radius; y++)
						{
							for (int x = _x - max_radius; x <= _x + max_radius; x++)



							{
								index = z * cx*cy + y*cx + x;
								int ellipsoid_index = (z - (_z - max_radius)) * max_diameter*max_diameter + (y - (_y - max_radius)) * max_diameter + (x - (_x - max_radius));

//								int ellipsoid_index = (z - (_z - ((z_diameter - 1) / 2))) * x_diameter*y_diameter + (y - ((y_diameter - 1) / 2)) * x_diameter + (x - ((x_diameter - 1) / 2));

								if (cube[ellipsoid_index] != 0 &&
									x >= 0 && x < cx &&
									y >= 0 && y < cy &&
									z >= 0 && z < cz )
								{
									if ((_org[index] & _m) && (_res[index] == 0))
									{

// 										int original_HU = m_pVolumeData->getData(index);
// 										int new_HU = original_HU * (255. - (float)rotation_cube[ellipsoid_index]) / 255.+ (_mean_min_HU + rand() % (_mean_max_HU - _mean_min_HU)) * (float)rotation_cube[ellipsoid_index] / 255.;
// 
// 										m_pVolumeData->setData(x, y, z, new_HU);
										_res[index] = _m;
										smoothing_mask_input[index] = 255;

									}

								}
							}
						}
					}
					delete[]ellipsoid;
					delete[]cube;
					delete[]rotation_cube;
					
					break;
				}
			}

			setProgressValue(100 * ((float)(i + 1) / _count));

	

			if (m_pVolumeData->threadStop)
				break;
		}

		int kernel_size;
		if (_cavitymode)
			kernel_size = _cavityboundary * 2 + 1;
		else
			kernel_size = 9;

		int filterWidth = kernel_size;
		int filterHeight = kernel_size;
		int filterSlice = kernel_size;
		double *kernel = (double*)malloc(sizeof(double)*filterHeight*filterSlice*filterWidth);

		mip::VP::getGaussian(kernel_size, kernel_size, kernel_size, 1, kernel, &(m_pVolumeData->threadStop),	NULL, this);
		
//		mip::writeRawFile(kernel, kernel_size*kernel_size*kernel_size * sizeof(double), QString("D:\\kernel.raw"));

		int m_gaussian_max = mip::VP::ConvolutionFiltering(smoothing_mask_output, smoothing_mask_input, kernel_size, kernel, cx, cy, cz,
			0, cx, 0, cy, 0, cz, &(m_pVolumeData->threadStop), NULL, this, true);
		
		mip::writeRawFile(smoothing_mask_output, cx*cy*cz*sizeof(short), QString("D:\\gauss.raw"));
		

		delete[]kernel;


		
	
			

		for (int z = 0; z < cz; z++)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					index = (z)*cy*cx + (y)*cx + (x);
					if (_res[index] == _m)
					{

						int original_HU = m_pVolumeData->getData(index);
						int new_HU;
 						if (!_cavitymode)
 							new_HU = original_HU * ((255. - (float)smoothing_mask_output[index]) / 255.) +
							(_mean_min_HU + rand() % (_mean_max_HU - _mean_min_HU)) * (float)smoothing_mask_output[index] / 255.;
						else
						{
							if (smoothing_mask_output[index] == m_gaussian_max)
								new_HU = (_mean_min_HU + rand() % (_mean_max_HU - _mean_min_HU)) * (float)smoothing_mask_output[index] / 255.;
							else
							{
								float weight = ((255. - (float)smoothing_mask_output[index])  / 255.);
								//if (weight > 1.) weight = 1.0;

								new_HU = (_boundary_min_HU + rand() % (_boundary_max_HU - _boundary_min_HU));
							}
								

						}
							
						m_pVolumeData->setData(x, y, z, new_HU);

					}
				}
			}
		}
	}
	delete[]smoothing_mask_input;
	delete[]smoothing_mask_output;
	
	emit finished();
}
