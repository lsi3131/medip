/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-01-14
@brief			CMeshViewRightClickEvent 헤더파일
*/


#pragma once

#include "DataContext.h"

class MEVolumeView;
class CMeshModelViewManager;
class CMeshViewRenderManager;

/*
@biref
*/
enum MESH_ACTION_TYPE
{
	ACT_EXPORT,
	ACT_COROR,
	ACT_CURRENT,
	ACT_WIREFRAME,
	ACT_BACKFACE
};


/*
@brief
*/
class CMeshViewRightClickEvent : public QObject
{
	Q_OBJECT

public	:
	CMeshViewRightClickEvent();
	~CMeshViewRightClickEvent();

	static CMeshViewRightClickEvent * getInstance();

	void	Init(DataContext* pDataContext, MEVolumeView* pViewer);

	void	setContextMenu();

	QMenu* getContextMenuPtr() { return m_ContextMenu; };

	QAction* getActionPtr(MESH_ACTION_TYPE _type);

public slots :
	void	OnExportMesh();
	void	OnReCalcCurrent();
	void	OnColorChange();

	void	OnWireFrame();
	void	OnBackface();

private	:
	QMenu	*m_ContextMenu;

	QAction *m_ActExport;
	QAction	*m_ActColor;
	QAction *m_ActCurrent;
	QAction *m_ActWire;
	QAction *m_ActBackface;
	
	DataContext*	m_pDataContext;
	MEVolumeView*	m_pViewer;

	CMeshModelViewManager* m_pModelViewManager;
	CMeshViewRenderManager* m_pRenderManager;
};

#define MESH_RClick_EVENT_MANAGER (CMeshViewRightClickEvent::getInstance())