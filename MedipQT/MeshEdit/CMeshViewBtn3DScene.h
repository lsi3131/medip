/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-01-13
@brief			CMeshViewBtn3DScene 헤더파일
*/

#pragma once

#ifndef CMESH_VIEWBTN_MANAGER_H
#define CMESH_VIEWBTN_MANAGER_H

#include <qobject.h>

#include "DataContext.h"

class MEVolumeView;
class WindowManager;
class CMeshCutManager;
class CMeshDlgManager;
class CMeshWorkManager;
class CPlaneManiplator;
class CMeshModelViewManager;
class CMeshDistancMananager;
class CMeshHoleFillManager;

class CMeshViewBtn3DScene : public QObject
{
	Q_OBJECT

public:
	CMeshViewBtn3DScene();
	~CMeshViewBtn3DScene();

	static CMeshViewBtn3DScene* getInstance();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		DataContext* pDataContext,
		WindowManager* pWinManager,
		MEVolumeView* pViewer,
		CMeshCutManager* pMeshCutManager,
		CMeshDlgManager* pDlgManager,
		CMeshWorkManager* pWorkManager,
		CPlaneManiplator* pPlaneManipulator,
		CMeshModelViewManager* pModelViewManager,
		CMeshDistancMananager* pDistanceManager,
		CMeshHoleFillManager* pHoleFillManager
	);

	void initInScreenRightMenu();
	void updateScreenLeftMenu(int _width, int _height);

	void enableOpcityCtrl(bool _b_enable);

	bool eventFilter(QObject* target, QEvent* e);


public slots:
	void OnCreateSphere();
	void OnCreateCube();
	void OnCreateCylinder();

	void OnPlanecut();
	void OnPolyInnercut();
	void OnPolyLinecut();
	void OnPolyFreecut();
	void OnFillHoleSelected();

	void OnDistance();
	void OnSculpt();
	void OnSelect();
	void OnStamp3D();
	void OnShader();

	void OnReCalcZero();
	void OnOpacityChange(int val);

public:
	QAbstractButton* HoverWidget;
	QIcon	 LeaveIcon;

	QPushButton* m_BtnZero;
	QPushButton* m_BtnSphere;
	QPushButton* m_BtnCube;
	QPushButton* m_BtnCylin;

	QToolButton* m_BtnCut;
	QPushButton* m_BtnPlane;
	QPushButton* m_BtnPoly;
	QPushButton* m_BtnLine;
	QPushButton* m_BtnPolyFree;

	QPushButton* m_BtnDistance;

	QPushButton* m_BtnSculpt;
	QPushButton* m_BtnSelect;
	QPushButton* m_BtnStamp3d;
	QPushButton* m_BtnShader;

	QPushButton* m_BtnFillHole;

	QLabel* m_pLabelOpacity;
	QSlider* m_SliderTrans;

	QAction* m_ActZero;

private:
	DataContext* m_pDataContext;
	WindowManager* m_pWinManager;
	MEVolumeView* m_pViewer;

	CMeshCutManager* m_pMeshCutManager;
	CMeshDlgManager* m_pDlgManager;
	CMeshWorkManager* m_pWorkManager;
	CPlaneManiplator* m_pPlaneManipulator;
	CMeshModelViewManager* m_pModelViewManager;
	CMeshDistancMananager* m_pDistanceManager;
	CMeshHoleFillManager* m_pHoleFillManager;
	
};

#define MESH_BTN_SCENE_MANAGER (CMeshViewBtn3DScene::getInstance())
#endif