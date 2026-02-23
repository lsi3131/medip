#pragma once

#ifndef CMESH_VIEWRENDER_MANAGER_H
#define CMESH_VIEWRENDER_MANAGER_H

#define TEST_OCTREE_BOX 0
#define TEST_LINE_RENDER 0

#include "CMeshFontManager.h"
#include "DataContext.h"
#include "Windows/MEVolumeView.h"

namespace mip
{
	class Renderer;
}
class CMeshWorkManager;
class CMeshModelViewManager;
class CMeshDlgManager;
class CMeshCutManager;
class CPlaneManiplator;
class CMeshHoleFillManager;
class CMeshManipulator;

/*
@brief
*/
class CMeshViewRenderManager
{
public:
	static CMeshViewRenderManager* getInstance();
public:
	CMeshViewRenderManager();
	~CMeshViewRenderManager();

	void Init(DataContext* pDataContext, MEVolumeView* pViewer);
	void Init(
		mip::Renderer* pRenderer,
		DataContext* pDataContext,
		MEVolumeView* pViewer,
		CMeshWorkManager* pMeshWorkManager,
		CMeshModelViewManager* pMeshModelView,
		CMeshDlgManager* pMeshDlgManager,
		CMeshCutManager* pMeshCutManager,
		CPlaneManiplator* pPlaneManipulator,
		CMeshHoleFillManager* pHoleFillManager,
		CMeshManipulator* pMeshManipulator
	);

	void render(QPainter* _p_paint);

	void setWireFrame(bool _b_wire);

	void OnWireFrame();
	void OnBackface();

	void Reset();

	void setHomePosition(HOME_POSITION index);

	mip::VECTOR3 getWorldCenter(bool _b_init = true);

	void initSphere();
	void initFontController();
	float setSphereScale(float scale);
	void setTempMesh(mip::MeshTopology* pMT, mip::MeshTopology* parents = nullptr) { m_pTempMesh = pMT; m_pTempParents = parents; }
	mip::MeshTopology* getTempMesh() { return m_pTempMesh; }
	mip::MeshTopology* getTempParents() { return m_pTempParents; }
	void delTempMesh() {
		SAFE_DELETE(m_pTempMesh);
		m_pTempMesh = nullptr;
		if (m_pTempParents)
		{
			m_pTempParents->initShader(mip::SHADERTYPE::SHADER_PONG);
		}
		m_pTempParents = nullptr;
	}
	FontController* getFontControl() { return &m_pFontCtrl; }

private:

	void renderScene(QPainter* _p_paint, bool _b_skip_mode = false);
	void renderLine(QPainter* _p_paint);
	void drawPckInfo(QPainter* _p_paint);

	void drawBG(QPainter* _p_paint);

	void drawMeshModel(MESH_WORK_MODE _mode);
	void drawManipulator(MESH_WORK_MODE _mode);
	void drawGuidBox(int _width, int _height);
	void drawBackBuffer(QPainter* _p_painter, int _width, int _height);

#if TEST_OCTREE_BOX
	int  getOctreeLine(mip::OcNode* _Node, std::vector<mip::VECTOR3>& Out_ocTreelineList, int& depth, int drawDepth = -1);
	mip::OcNode* rootNode;
	int nTEST;
#endif

	void setClipPlane(mip::MeshTopology* _p_mesh, bool _b_enable = true);

	muint32 createTextureFromQImage(QImage* image);

	mip::MATRIX44 checkWorldMat(mip::MeshTopology* _p_mesh);

private:
	QLabel* m_labelImage;
	std::vector<mip::VECTOR3> m_ocTreelineList_MeVol;

	bool m_wireFrame;
	bool m_backface;

	mip::VECTOR3 m_worldCenter;

	mip::TRANSFORM m_trObject;

	muint32 m_cullingTexture;

	// brush 
	mip::MeshTopology* m_pPickedSphere;
	mip::MeshTopology* m_pPickedPoint;
	// vertPoint
	mip::MeshTopology* m_pPickedVertexPoint;
	// 3D Text Position
	mip::MeshTopology* m_pPicked3dTextPoint;
	//temporary
	mip::MeshTopology* m_pTempMesh;
	mip::MeshTopology* m_pTempParents;
	//FontController
	FontController m_pFontCtrl;

	DataContext* m_pDataContext;
	mip::Renderer* m_pRenderer;
	MEVolumeView* m_pMeshViewer;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pModelViewManager;
	CMeshDlgManager* m_pDlgManager;
	CMeshCutManager* m_pMeshCutManager;
	CPlaneManiplator* m_pPlaneManipulator;
	CMeshHoleFillManager* m_pHoleFillManager;
	CMeshManipulator* m_pMeshManipulator;

public:
	mip::TRANSFORM m_trPlane;
};

#define MESH_RENDER_MANAGER (CMeshViewRenderManager::getInstance())
#endif
