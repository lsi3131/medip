#pragma once

#ifndef MEVOLUMEVIEW_H
#define MEVOLUMEVIEW_H

#include "define.h"
#include "glWidget.h"

#include "math/math.h"
#include "math/Transform.h"

#include "mipEngine/Camera.h"

#include "Renderer/Mesh.h"
#include "Renderer/color.h"
#include "MeshControl.h"
#include "DataContext.h"

#include "MeshEdit/MeshEditSculpt.h"

#include <QMap>
#include <QWidget>
#include <QRect>
#include <QIcon>

namespace mip
{
	class MeshTopology;
};

class QLabel;
class QImage;
class QMenu;
class QAction;
class QPushButton;
class QAbstractButton;
class QCheckBox;
class QSlider;
class QToolButton;

class CMeshWorkManager;
class CMeshModelViewManager;
class CMeshViewRenderManager;
class CMeshDlgManager;
class CMeshViewBtn3DScene;
class WindowManager;
class ActionManager;

namespace mip
{
	class Renderer;
}

class MEVolumeView : public OpenGLWidget
{
	Q_OBJECT

public:
	friend MeshEditSculpt;

public:
	MEVolumeView(DataContext* pDataContext, QWidget* parent = 0);
	MEVolumeView(
		mip::Renderer* pRenderer,
		WindowManager* pWinManager,
		ActionManager* pActionManager,
		DataContext* pDataContext,
		QWidget* parent = 0);
	virtual ~MEVolumeView();

public:
	void SetModel(CMeshWorkManager* pMeshWorkManager);

	void reserveInit(HWND hwnd);
	void resetUI();

	// 20200916 byPHS - Brush 包访
	bool clearSelectFlag(bool force = false);
	bool clearSelectFlag(mip::MeshTopology* pMT);
	bool clearAllSelectFlag();
	bool readyBrush(int _MeshWorkMode, int initial = 0);
	bool updateMeshVertex();
	bool setSphereScale(float scale);
	bool deleteSelectFlag();
	bool allSelectFlag();
	bool createMatchingData(int Mode = 0);
	bool clearVertFlag();

	// 20210818 byPHS - Selection 包访
	void updateSelectionUI(mip::MeshTopology* pMT = nullptr);

	//
	bool getGeometryCount(int& _vCnt, int& _tCnt);
	bool getGeometryCount(mip::MeshTopology* pMT, int& _vCnt, int& _tCnt);
	bool updateGeometryCount(mip::MeshTopology* pMT, int& _vCnt, int& _tCnt);
	bool updateGeometryCount(int& _vCnt, int& _tCnt);
	bool updateGeometryCount(int pick = -1);
	bool getMEViewGeometryCount(int& _vCnt, int& _tCnt);

	void updateGeometryCnt(bool bUpdate);
	bool enableCtrlToggleKey();

	float GetSphereScale() const;
	mip::VECTOR3 GetPickPoint() const;
	mip::VECTOR3 GetPickVertPoint() const;

	void SetUpdateGeometryCnt(bool value);
	bool IsUpdateGeometryCnt() const;

	void SetOldIdx(int value);
	int GetOldIdx() const;

	void SetTotalSize(int value);
	int GetTotalSize() const;

	int GetVertCnt() const;
	int GetTriCnt() const;

	void SetSphereRadius(float value);
	float GetSphereRadius() const;

	std::vector<mip::VECTOR3> GetDrawLineList() const;

	void SetCtrl(bool value);
	void SetAlt(bool value);
	void SetShift(bool value);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual bool eventFilter(QObject* target, QEvent* e) override;

	virtual void render(QPainter* painter) override;

private:
	void processMouseMove();
	void processMousePress(QMouseEvent* e);
	void processMouseRelease(QMouseEvent* e);

private:
	bool process_BrushSculpt(bool bShift = false);
	bool process_Select(eMeshSelectMode _mode = eMeshSelectMode::Select); //0 - select, 1 - unselect
	bool process_Sculpt_Move(eMouseMode mode); //_press = 0, _Move = 1, _release = 2
	bool process_UpdateDisplay();

	//Stemp3D
	bool process_Stamp3D();
	bool process_Stamp3D_Controller(int mode); //_press = 0, _Move = 1

	// 20200910 byPHS - Brush 包访 
	bool moveSphere();

	void updateCurrentMeshBoundingBox();

	//Stemp3D
	mip::MeshTopology* getIntersectedMeshByScreenMousePoint();

private:
	HWND m_hwnd;

	QPoint m_MousePos;
	QPoint m_preMousePos;
	QRect m_clickBox;  //for context menu

private:
	bool m_mouseMove;
	bool m_RbuttonDown;
	bool m_LbuttonDown;
	bool m_MbuttonDown;
	bool m_CtrlToggle;

	// 20200910 byPHS - Brush 包访 
	mip::mesh_control::Brush_Data m_BrushData;

	//Brush Sphere
	float m_sphereScale;

	bool m_bUpdateGeometryCnt;
	int m_vertCnt;
	int m_TriCnt;
	int m_oldIdx;
	int m_TotalSize;

	//picking Position
	//mip::VECTOR3 m_pickPoint;
	//mip::VECTOR3 m_pickVertPoint;
	mip::VECTOR3 m_pick3dTextPoint;

#if 0
	std::vector<int> m_octreeTestlist;
#endif

	//20201217_byPHS - Sculpt 包访
	//mip::VECTOR3 m_realPickPT;
	//mip::VECTOR3 m_1stDirPickFromScreen = mip::VECTOR3(0.0f, 0.0f, 0.0f);
	//mip::VECTOR3 m_pickPtOld = mip::VECTOR3(0.0f, 0.0f, 0.0f);

	//test
	std::vector<mip::VECTOR3> m_drawLineList;

private:
	mip::Renderer* m_pRenderer;
	WindowManager* m_pWinManager;
	ActionManager* m_pActionManager;
	DataContext* m_pDataContext;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pMeshModelViewManager;
	CMeshViewRenderManager* m_pMeshRenderManager;
	CMeshDlgManager* m_pMeshDialogManager;
	CMeshViewBtn3DScene* m_pMeshBtn3DScene;

	std::shared_ptr<MeshEditSculpt> m_pMeshEditSculpt;
};
#endif
