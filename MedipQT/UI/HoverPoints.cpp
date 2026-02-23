#include "stdafx.h"
#include "HoverPoints.h"
#include "Windows/windowManager.h"
#include "UI/CustomHistogram.h"

static QAlphaStops VolumeAlpha[SP_COUNT];
static QVector<QAlphaStops> custom3DAlpha;


void HoverPoints::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(QPen(QColor(48, 48, 48)));
	p.setBrush(QBrush(QColor(255, 255, 255)));

	path = QPainterPath();
	pathPoly = QPainterPath();

	path.moveTo(m_points.at(0));
	pathPoly.moveTo(m_points.at(0));
	for (int i = 1; i < m_points.size(); i++)
	{
		path.lineTo(m_points.at(i));
		pathPoly.lineTo(m_points.at(i));
	}

	QPainterPathStroker stroker;

	stroker.setCapStyle(Qt::FlatCap);
	stroker.setWidth(5.5);
	pathPoly = stroker.createStroke(pathPoly);
	stroker.setWidth(2.7);
	path = stroker.createStroke(path);

	

	p.drawPath(path);

	for (int i = 0; i < m_points.size(); i++)
		p.drawEllipse(m_points.at(i), 4, 4);

	p.end();
}

HoverPoints::HoverPoints(int type, SLICE_PRESET preset, QWidget *parent, int cusIndex)
	:QWidget(parent)
{
	static bool initial = false;

	setMouseTracking(true);
	this->m_type = type;
	this->m_preset = preset;
	m_custom = cusIndex;
	m_points = GetPresetAlpha(m_preset, m_type, false, m_custom);
	m_move = false;
	m_change = false;
	
	if (!initial)
	{
		initial = true;
		custom3DAlpha.reserve(PRESET_MAX);
	}
}

void HoverPoints::mousePressEvent(QMouseEvent * e)
{
	m_move = false;
	m_index = -1;
	startPos = e->pos();
	if ((e->buttons() & Qt::LeftButton))
	{
		for (int i = 0; i < m_points.size(); i++)//기존 포인트 클릭
		{
			QPoint center = m_points.at(i);
			QRect point(center.x() - 4, center.y() - 4, 2 * 4, 2 * 4);

			if (point.contains(e->pos()))
			{
				m_index = i;
				return;
			}
		}
		if ((-1 == m_index) && (pathPoly.contains(e->pos())))//새로운 포인트 지정
		{
			for (int i = 0; i < m_points.size(); i++)
			{
				if (m_points.at(i).x() > startPos.x())
				{
					m_index = i;
					m_points.insert(m_index, QAlphaStop(startPos));
					m_change = true;
					this->repaint();
					emit ChangePoint();
					return;
				}
			}
		}
		else//다른 곳 클릭(colormap color 지정)
		{
			e->ignore();
			return;
		}

	}
	else if ((e->buttons() & Qt::RightButton))//포인트 삭제
	{
		for (int i = 1; i < m_points.size() - 1; i++)
		{
			QPoint center = m_points.at(i);
			QRect point(center.x() - 4, center.y() - 4, 2 * 4, 2 * 4);

			if (point.contains(e->pos()))
			{
				m_index = i;
				m_change = true;
				emit ChangePoint();
				break;
			}
		}
	}
}

void HoverPoints::mouseMoveEvent(QMouseEvent * e)
{
	const int interval = 1;
	m_move = true;

	if ((e->buttons() & Qt::LeftButton)) //포인트 추가 후 움직임
	{
		if (-1 == m_index)//지정된 포인트가 없을 때 무시
		{
			e->ignore();
			return ;
		}

		movePos = e->pos();

		int xGap = abs(startPos.x() - movePos.x());
		int yGap = abs(startPos.y() - movePos.y());

		if (xGap >= yGap) //포인트 위치 변경
		{
			if ((0 == m_index) || ((m_points.size() - 1) == m_index))
			{
				startPos = e->pos();
				return;
			}

			int pos = -1;

			if (((m_points.at(m_index - 1).x() + interval) < movePos.x())
				&& ((m_points.at(m_index + 1).x() - interval) > movePos.x())) //이전,이후 point 사이에 존재
				pos = e->pos().x();
			else
			{
				startPos = e->pos();
				return;
			}

			m_points.replace(m_index, QPoint(pos, m_points.at(m_index).y()));
		}
		else //기존(포인트 값 변경)
		{
			int pos = movePos.y();

			if (pos >= 70)
				pos = 69;
			else if (pos < 0)
				pos = 0;
			m_points.replace(m_index, QPoint(m_points.at(m_index).x(), pos));
		}

		m_change = true;
		this->repaint();
		emit ChangePoint();
		startPos = e->pos();
	}

	else if ((e->buttons() & Qt::RightButton))//포인트삭제와는 관련없음
		return ;
}

void HoverPoints::mouseReleaseEvent(QMouseEvent * e)
{
	if ((e->button() == Qt::RightButton))
	{
		if (m_move || (-1 == m_index))
		{
			e->ignore();
			return ;
		}
		m_points.remove(m_index);

		this->repaint();
	}
}

QAlphaStops HoverPoints::GetPresetAlpha(SLICE_PRESET preset, int type, bool reset, int cusIndex)
{
	static bool init = false;
	static QAlphaStops resetStops;
	if (!init)
	{
		init = true;

		QString points;
		QStringList point;
		int cusCnt = WIN_MANAGER->getPresetCount();

		resetStops.insert(0, QPoint(0, 69));
		resetStops.insert(1, QPoint(511, 0));
		//volume
		for (int i = 0; i < SP_COUNT; i++)
		{
			WIN_MANAGER->getConfigValue(ELEMENT_PRESET, QString("VOLUME_%1").arg(i), points);

			if (points.length() <= 0)
			{
				VolumeAlpha[i].insert(0, QPoint(0, 69));
				VolumeAlpha[i].insert(1, QPoint(511, 0));
			}
			else
			{
				point = points.split(";");
				for (int j = 0; j < points.split(";").length(); j++)
				{
					QStringList pointxy = point.at(j).split(",");
					VolumeAlpha[i].insert(j, QPoint(pointxy.at(0).toInt(), pointxy.at(1).toInt()));
				}
			}
		}

		//volume
		for (int i = 0; i < cusCnt; i++)
		{
			//getconfigvalue
			WIN_MANAGER->getConfigPreset(i, CL_3D, points, false);
			QAlphaStops stops;

			if (points.length() <= 0)
			{
				stops.insert(0, QPoint(0, 69));
				stops.insert(1, QPoint(511, 0));
			}
			else
			{
				point = points.split(";");
				for (int j = 0; j < points.split(";").length(); j++)
				{
					QStringList pointxy = point.at(j).split(",");
					stops.insert(j, QPoint(pointxy.at(0).toInt(), pointxy.at(1).toInt()));
				}
			}

			if (custom3DAlpha.size() <= i)
				custom3DAlpha.append(stops);
			else
				custom3DAlpha.replace(i, stops);
		}

	}

	
	if (-1 >= cusIndex)
	{
		//return stops
		if (reset && preset == SP_DEFAULT)
			return resetStops;
		if (type == CL_2D)
			return resetStops;
		else
			return VolumeAlpha[preset];
	}
	else
	{
		if (CL_2D == type)
			return resetStops;
		else
			return custom3DAlpha.at(cusIndex);
	}
}

void HoverPoints::SetPresetAlpha(QAlphaStops stops, SLICE_PRESET preset, int type, int cusIndex)
{
	QString points;

	for (int i = 0; i < stops.size(); i++)
	{
		if (0 != i)
			points.append(QString(";%1,%2").arg(stops.at(i).x()).arg(stops.at(i).y()));
		else
			points.append(QString("%1,%2").arg(stops.at(i).x()).arg(stops.at(i).y()));
	}

	if(-1 >= cusIndex)
	{
		if (type == CL_3D)
		{
			VolumeAlpha[preset] = stops;
			WIN_MANAGER->setConfigValue(ELEMENT_PRESET, QString("VOLUME_%1").arg(preset), points);
		}
	}
	else
	{
		if (CL_3D == type)
		{
			if (cusIndex >= custom3DAlpha.size())
				custom3DAlpha.append(stops);
			else
				custom3DAlpha.replace(cusIndex, stops);
		}

		WIN_MANAGER->setConfigPreset(cusIndex, type, points, false);
	}
}

bool HoverPoints::ApplyAlpha()
{
	if (m_change)
		SetPresetAlpha(m_points, m_preset, m_type, m_custom);

	return m_change;
}

QVector<int> HoverPoints::getAlphaGraph(int type, SLICE_PRESET preset, int cusIndex, bool rst)
{
	QAlphaStops stops;
	QVector<int> alphaTable;
	QPointF point;
	QPainterPath alphaPath;
	float index;

	stops = GetPresetAlpha(preset, type, rst, cusIndex);

	alphaPath.moveTo(stops.at(0));

	for (int i = 1; i < stops.size(); i++)
		alphaPath.lineTo(stops.at(i));

	for (int i = 0; i < (512 / 2); i++)
	{
		index = (float)i / ((512 / 2) - 1);
		point = alphaPath.pointAtPercent(index);
		alphaTable.push_back((int)(((69.0f - point.y()) / 69) * 255));
	}

	return alphaTable;
}

void HoverPoints::ResetAlphaPoint()
{
	QAlphaStops stops = GetPresetAlpha(m_preset, m_type, true);
	if (m_points != stops)
	{
		m_points.clear();
		m_points = stops;
//		if(GetPresetAlpha(m_preset, m_type) != stops)
			m_change = true;
		this->repaint();
		emit ChangePoint();
	}
}

void HoverPoints::AddPresetAlpha(int index, bool isImport)
{
	QAlphaStops stops;

	if (!isImport)
	{
		QString points;

		stops.clear();
		stops = GetPresetAlpha(SP_CUSTOM, CL_3D);
		custom3DAlpha.append(stops);
		points = "";

		for (int i = 0; i < stops.size(); i++)
		{
			if (0 != i)
				points.append(QString(";%1,%2").arg(stops.at(i).x()).arg(stops.at(i).y()));
			else
				points.append(QString("%1,%2").arg(stops.at(i).x()).arg(stops.at(i).y()));
		}

		WIN_MANAGER->setConfigPreset(index, CL_3D, points, false);

		stops.clear();
	}
	else
	{
		QString points;
		QStringList point;
		QAlphaStops stops;

		stops.clear();

		WIN_MANAGER->getConfigPreset(index, CL_3D, points, false);

		if (points.length() <= 0)
		{
			stops.insert(0, QPoint(0, 69));
			stops.insert(1, QPoint(511, 0));
		}
		else
		{
			point = points.split(";");
			for (int j = 0; j < point.size(); j++)
			{
				QStringList pointxy = point.at(j).split(",");
				stops.insert(j, QPoint(pointxy.at(0).toInt(), pointxy.at(1).toInt()));
			}
		}


		if (custom3DAlpha.size() <= index)
			custom3DAlpha.append(stops);
		else
			custom3DAlpha.replace(index, stops);

		stops.clear();
	}
}

bool HoverPoints::DelPresetAlpha(int index)
{
	if(index < 0 || index >= custom3DAlpha.size())
		return false;

	custom3DAlpha.remove(index);

	return true;
}
