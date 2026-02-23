/*
@company 메디컬아이피
@autor 허 건 과장
@date 2021-01-20
@brief CMeshWorkManager 헤더파일
*/

#pragma once

#ifndef CMESH_WORK_MANAGER_H
#define CMESH_WORK_MANAGER_H

#include "DataContext.h"

#include "mipEngine/Camera.h"

#include "Windows/MEVolumeView.h"

namespace mip
{
	class Renderer;
}

class CMeshModelViewManager;
class CMeshViewRenderManager;
class CMeshViewBtn3DScene;
class CMeshCutManager;
class CMeshHoleFillManager;
class CMeshDlgManager;
class CMeshManipulator;
class CPlaneManiplator;
class CMeshDistancMananager;
class CMeshViewRightClickEvent;

class WindowManager;
class ActionManager;

/*
@brief
*/
class CMeshWorkManager : public QObject
{
	Q_OBJECT

public:
	static CMeshWorkManager* getInstance();
public:
	CMeshWorkManager();
	virtual ~CMeshWorkManager();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		mip::Renderer* pRenderer,
		DataContext* pDataContext,
		WindowManager* pWinManager,
		ActionManager* pActionManager,
		MEVolumeView* pViewer,
		CMeshModelViewManager* pModelViewManager,
		CMeshViewRenderManager* pRenderManager,
		CMeshViewBtn3DScene* pBtn3DScene,
		CMeshCutManager* pMeshCutManager,
		CMeshHoleFillManager* pHoleFillManager,
		CMeshDlgManager* pDlgManager,
		CMeshManipulator* pMeshManipulator,
		CPlaneManiplator* pPlaneManipulator,
		CMeshDistancMananager* pDistanceManager,
		CMeshViewRightClickEvent* pRClickEvent
	);

	void SetMeshManipulator(CMeshManipulator* pMeshManipulator);
	CMeshManipulator* GetMeshManipulator() const;

	void SetPlaneManipulator(CPlaneManiplator* pPlaneManipulator);
	CPlaneManiplator* GetPlaneManipulator() const;

	void SetMeshDlgManager(CMeshDlgManager* pMeshDlgManager);
	CMeshDlgManager* GetMeshDlgManager() const;

	void SetModelView(CMeshModelViewManager* pMeshModelView);
	CMeshModelViewManager* GetModelView() const;

	void SetRenderer(CMeshViewRenderManager* pMeshRenderer);
	CMeshViewRenderManager* GetRenderer() const;

	void SetMeshBtn3DScene(CMeshViewBtn3DScene* pBtn3DScene);
	CMeshViewBtn3DScene* GetMeshBtn3DScene() const;

	void SetMeshHoleFillManager(CMeshHoleFillManager* pMeshHoleFillManager);
	CMeshHoleFillManager* GetMeshHoleFillManager() const;

	void SetMeshCutManager(CMeshCutManager* pMeshCutManager);
	CMeshCutManager* GetMeshCutManager() const;

	void UpdateWorkMode(MESH_WORK_MODE _mode, bool iconRefresh = false);
	void UpdateWorkMode(MESH_REMESH_TYPE _mode, bool iconRefresh = false);

	void UpdateUndoRedo(bool _b_write);

	void UpdateBtn(MESH_WORK_MODE CurMode, MESH_WORK_MODE _mode);

	void resetList();

	void setWorkMode(MESH_WORK_MODE _mode);
	MESH_WORK_MODE	getWorkMode();
	MESH_WORK_MODE	getPrevWorkMode();

	void setKeyBtnPtr(bool* _p_ctrl, bool* _p_alt, bool* _p_shift);

	void processkeyPress(QKeyEvent* e);
	void processkeyRelease(QKeyEvent* e);

	bool processMouseMove(QMouseEvent* e, mip::SCAMERA* pCamera);
	void processMousePress(QMouseEvent* e);
	bool processMouseRelease(QMouseEvent* e);
	bool processMouseDoubleClicked(QMouseEvent* e);
	void ProcessWheel(QWheelEvent* event);

private:
	bool processMouseMove(mip::SCAMERA* pCamera);

private:
	MESH_WORK_MODE m_WorkMode;
	MESH_WORK_MODE m_PrevWorkMode;

	bool* m_p_Ctrl;
	bool* m_p_Alt;
	bool* m_p_Shift;

	bool m_mouseMove;

	bool m_RbuttonDown;
	bool m_LbuttonDown;
	bool m_MbuttonDown;

	QPoint m_MousePos;
	QPoint m_preMousePos;

	QRect m_clickBox; //for context menu

	DataContext* m_pDataContext;

	mip::Renderer* m_pRenderer;
	CMeshModelViewManager* m_pModelViewManager;
	CMeshViewRenderManager* m_pRenderManager;
	CMeshViewBtn3DScene* m_pBtn3DScene;
	CMeshCutManager* m_pMeshCutManager;
	CMeshHoleFillManager* m_pHoleFillManager;
	CMeshDlgManager* m_pDlgManager;
	CMeshManipulator* m_pMeshManipulator;
	CPlaneManiplator* m_pPlaneManipulator;
	CMeshDistancMananager* m_pDistanceManager;
	CMeshViewRightClickEvent* m_pRClickEvent;
};

#define MESH_WORK_MANAGER (CMeshWorkManager::getInstance())
#endif