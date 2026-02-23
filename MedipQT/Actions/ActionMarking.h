#pragma once

#ifndef ACTION_MARKING_H
#define ACTION_MARKING_H

#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include "graphics/BoundingBox.h"
#include <qpoint.h>
#include "Renderer/Slice.h"
#include "volumedata.h"

class WindowBase;
class OpenGLWidget;
class AnalMPRPlaneView;

class ActionMarking : public QUndoCommand
{
public:
	ActionMarking(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, bool drawCutMarking, mask _m = VM_MASK1, bool merge = false,int _mI=0, QUndoCommand *parent = 0);
	ActionMarking(VOLUME_DATA* pVolumeData, QPainterPath screen_region, WindowBase* view, mask _m = VM_MASK1, bool merge = false, int _mI = 0, bool drawcut=false, bool brush=false, int startHU=0, int endHU=0, QUndoCommand *parent = 0);
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WindowBase * getView() const { return m_view; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }
	const std::vector<QPoint> & getList() const { return m_list; }
private:
	bool checkInsizeVolume(WINDOW_TYPE winType, int x, int y);

	int						m_id;
	std::vector<QPoint>		m_list;
	int						m_maskIndex;
	int						m_maskNumber;
	WindowBase*				m_view;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	uint					m_depth;
	bool					m_TAState;
	WINDOW_TYPE				m_type;
	bool					m_drawCutMarking;

	VOLUME_DATA* m_pVolumeData;
};

class ActionUnMarking : public QUndoCommand
{
public:
	ActionUnMarking(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m = VM_MASK1, bool merge = false,int _mI=0, QUndoCommand *parent = 0);
	ActionUnMarking(VOLUME_DATA* pVolumeData, QPainterPath screen_region, WindowBase* view, mask _m = VM_MASK1, bool merge = false, int _mI = 0, QUndoCommand *parent = 0);
	ActionUnMarking(VOLUME_DATA* pVolumeData, QVector<QPoint> screen_point, const QPolygon & list, WindowBase* view, mask _m = VM_MASK1, bool merge = false, int _mI = 0,QUndoCommand *parent = 0);
    ActionUnMarking(VOLUME_DATA* pVolumeData, const QPoint screen_point, const QPolygon & list, WindowBase* view, mask _m = VM_MASK1, bool merge = false, int _mI = 0, QUndoCommand *parent = 0);
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WindowBase * getView() const { return m_view; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }
	const std::vector<QPoint> & getList() const { return m_list; }
private:
	int						m_fillCount;
	int						m_id;
	int						m_maskIndex;
	int						m_maskNumber;
	bool					m_TAState;
	std::vector<QPoint>		m_list;
	WindowBase*				m_view;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	uint					m_depth;
	WINDOW_TYPE				m_type;

	VOLUME_DATA*			m_pVolumeData;
};



class ActionUnMarking2 : public QUndoCommand
{
public:
	ActionUnMarking2(const QPolygon & list, WindowBase* view, std::vector<mask>	& maskList, bool merge = false, QUndoCommand *parent = 0);
	ActionUnMarking2(const QPoint screen_point, const QPolygon & list, WindowBase* view, std::vector<mask>	& maskList, bool merge = false, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WindowBase * getView() const { return m_view; }
	uint getDepth() const { return m_depth; }
	const std::vector<QPoint> * getList(int n) const { return m_list.at(n); }

	std::vector<mask>		m_maskList;
private:
	int						m_id;
	int						m_maskIndex;
	std::vector<std::vector<QPoint>*>		m_list;
	WindowBase*				m_view;
	uint					m_depth;
};



class ActionPolyFill : public QUndoCommand
{
public:
	ActionPolyFill(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m = VM_MASK1, bool del = false,int _mI=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WINDOW_TYPE getType() const { return m_type; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }
private:
	int						m_id;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	bool					m_del;
	WINDOW_TYPE				m_type;
	mask					m_mask;
	uint					m_depth;
	muint32					m_bufferCount;

	VOLUME_DATA*			m_pVolumeData;
};

class ActionDrawFill : public QUndoCommand
{
public:
	ActionDrawFill(VOLUME_DATA* pVolumeData, const QPolygon & list, WindowBase* view, mask _m = VM_MASK1, bool del = false, int _mI=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WINDOW_TYPE getType() const { return m_type; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }
private:
	int						m_id;
	bool					m_del;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	WINDOW_TYPE				m_type;
	mask					m_mask;
	uint					m_depth;
	bool					m_first;
	muint32					m_bufferCount;
	VOLUME_DATA*			m_pVolumeData;
};


class ActionDrawSeedFromMask : public QUndoCommand
{
public:
	ActionDrawSeedFromMask(VOLUME_DATA* pVolumeData, mask _m = VM_MASK1, bool foreseed = true,int _mI=0, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	int						m_maskIndex;
	int						m_voxel[2];
	BoundingBoxI			m_box[2];
	bool					m_foreseed;
	mask					m_mask;
	VOLUME_DATA*				m_pVolumeData;
};



class ActionDrawFill3D : public QUndoCommand
{
public:
	ActionDrawFill3D(VOLUME_DATA* pVolumeData, mask _m, bool del = false,int _mI=0,QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	OpenGLWidget * getView() const { return m_view; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }
private:
	int						m_id;
	bool					m_del;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	OpenGLWidget*			m_view;
	mask					m_mask;
	uint					m_depth;
	bool					m_first;

	VOLUME_DATA*			m_pVolumeData;
};

class WorkDrawFill3D : public QObject
{
	Q_OBJECT

public:
	WorkDrawFill3D(VOLUME_DATA* pVolumeData, const QPolygon & list, OpenGLWidget * view, mask m = VM_MASK1, bool del = false, int mI=0, int uid=0)
	{
		_list.append(list);
		_mask = m;
		_maskIndex = mI;
		_view = view;
		_del = del;
		_uid = uid; 

		m_pVolumeData = pVolumeData;
	}
	virtual ~WorkDrawFill3D() {};
public:
	QPolygon _list;
	mask _mask;
	int _maskIndex;
	int _uid;
	bool _del;
	OpenGLWidget * _view;

	VOLUME_DATA* m_pVolumeData;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};

class ActionInterDraw : public QUndoCommand
{
public:
	ActionInterDraw(VOLUME_DATA* pVolumeData, mask _m = VM_MASK2, int _mI = 0, bool _del = false, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	bool					m_del;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	bool					m_drawcut;
	bool					m_first;

	VOLUME_DATA*			m_pVolumeData;
};

class WorkInterSelect : public QObject
{
	Q_OBJECT

public:
	WorkInterSelect(VOLUME_DATA* pVolumeData, const QPolygon & pckPoints, AnalMPRPlaneView * view, mask m,
		bool del = false, int mI = 0, bool oval=false, bool _line = false, bool _circle=false):
		_view(view),
		_mask(m),
		_del(del),
		_maskIndex(mI),
		_oval(oval),
		_line(_line),
		_circle(_circle),
		m_pVolumeData(pVolumeData)
	{
		_list.append(pckPoints);
	}
	virtual ~WorkInterSelect() {};
public:
	QPolygon _list;
	mask _mask;
	int _maskIndex;
	bool _del;
	bool _oval;
	bool _line;
	bool _circle;
	AnalMPRPlaneView * _view;

	VOLUME_DATA* m_pVolumeData;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};

class ActionOvalFill : public QUndoCommand
{
public:
	
	ActionOvalFill(VOLUME_DATA* pVolumeData, const QPolygon &list, WindowBase *view, mask _m, int _mI, bool del, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	WINDOW_TYPE getType() const { return m_type; }
	mask getMask() const { return m_mask; }
	uint getDepth() const { return m_depth; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	bool					m_del;
	WINDOW_TYPE				m_type;
	mask					m_mask;
	uint					m_depth;
	muint32					m_bufferCount;

	VOLUME_DATA*			m_pVolumeData;
};



class ActionInterRect : public QUndoCommand
{
public:
	ActionInterRect(VOLUME_DATA* pVolumeData, mask _m = VM_MASK2, int _mI = 0, bool _del = false, QUndoCommand *parent = 0);

	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }
	mask getMask() const { return m_mask; }
private:
	int						m_id;
	bool					m_del;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;
	bool					m_first;
	bool					m_drawcut;

	VOLUME_DATA*			m_pVolumeData;
};


class WorkInterRect : public QObject
{
	Q_OBJECT

public:
	WorkInterRect(VOLUME_DATA* pVolumeData, QPainterPath screen_region, AnalMPRPlaneView* view, mask _m = VM_MASK2, int _mI = 0)
	{
		m_region = screen_region;
		m_view = view;
		m_mask = _m;
		m_maskIndex = _mI;

		m_pVolumeData = pVolumeData;
	}

private:
//	std::vector<QPoint>		m_list;
	QPainterPath	m_region;
	AnalMPRPlaneView*	m_view;
	mask			m_mask;
	int				m_maskIndex;

	VOLUME_DATA* m_pVolumeData;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};

class WorkSplitting3D : public QObject
{
	Q_OBJECT

public:
	WorkSplitting3D(VOLUME_DATA* pVolumeData, OpenGLWidget * view, QPolygon list=QPolygon(), mask m = VM_MASK1, int mI = 0, int uid = 0, WORK_MODE eType = WORK_MODE::WORK_3D_CURVE_SPLIT, mip::PLANE p1 = mip::PLANE(), mip::PLANE p2 = mip::PLANE())
	{
		if (eType == WORK_MODE::WORK_3D_CURVE_SPLIT)
		{
			_list.append(list);
		}
		else if (eType == WORK_MODE::WORK_3D_PLANE_SPLIT)
		{
			_plane_1 = p1;
		}
		else if (eType == WORK_MODE::WORK_3D_ANGLE_SPLIT)
		{
			_plane_1 = p1;
			_plane_2 = p2;
		}

		_mask = m;
		_maskIndex = mI;
		_view = view;
		_uid = uid;
		_type = eType;

		m_pVolumeData = pVolumeData;
	}
	virtual ~WorkSplitting3D() {};
public:
	QPolygon _list;
	mask _mask;
	int _maskIndex;
	int _uid;
	mip::PLANE _plane_1;
	mip::PLANE _plane_2;
	OpenGLWidget * _view;
	WORK_MODE _type; //split type 0:(default)curve, 1:plane

	VOLUME_DATA* m_pVolumeData;

private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

};

class MaskInfo;

class ActionSplitting3D : public QUndoCommand
{
public:
	ActionSplitting3D(VOLUME_DATA* pVolumeData, int uid,int splitCount, QUndoCommand *parent = nullptr);
	void undo() override;
	void redo() override;
	bool mergeWith(const QUndoCommand *command) override;
	int id() const override { return m_id; }

private:
	int						m_id;
	int						m_maskIndex;
	int						m_maskNumber;
	int						m_fillCount;
	bool					m_TAState;
	int						m_Split;
	int						m_SplitIndex[2];
	std::vector<MaskInfo>	m_Splitinfo;
	std::vector<int>		m_Splitlist;
	std::vector<muint32>	m_Splituid;
	std::vector<int>		m_SplitFill;
	std::vector<BoundingBoxI> m_SplitBox;
	BoundingBoxI			m_orgbox[2];
	mask					m_mask;

	VOLUME_DATA*			m_pVolumeData;
};



class WorkRandomPoint : public QObject
{
	Q_OBJECT

public:
	WorkRandomPoint(VOLUME_DATA* pVolumeData, int _count, bool _pointunit, int _x_radius_min, int _x_radius_max,
		int _y_radius_min, int _y_radius_max,
		int _z_radius_min, int _z_radius_max,
		int _mean_min_HU, int _mean_max_HU,
		int _boundary_min_HU, int _boundary_max_HU,
		bool _cavitymode,	int _cavityboundary, int _uid)
		:_count(_count), _pointunit(_pointunit),
		_x_radius_min(_x_radius_min),
		_x_radius_max(_x_radius_max),
		_y_radius_min(_y_radius_min),
		_y_radius_max(_y_radius_max),
		_z_radius_min(_z_radius_min),
		_z_radius_max(_z_radius_max),
		_mean_min_HU(_mean_min_HU),
		_mean_max_HU(_mean_max_HU),
		_boundary_min_HU(_boundary_min_HU),
		_boundary_max_HU(_boundary_max_HU),		
		_cavitymode(_cavitymode),
		_cavityboundary(_cavityboundary),
		_uid(_uid),
		m_pVolumeData(pVolumeData)
	{
	};

private:
	bool _pointunit;
	int _count;
	int _x_radius_min;
	int _x_radius_max;
	int _y_radius_min;
	int _y_radius_max;
	int _z_radius_min;
	int _z_radius_max;
	
	int _mean_min_HU;
	int _mean_max_HU;
	int _boundary_min_HU;
	int _boundary_max_HU;
	int _cavityboundary;
	bool _cavitymode;
	int		_uid;

	VOLUME_DATA* m_pVolumeData;
private:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();
};
#endif




