#pragma once

#include "mipEngine\Camera.h"
#include "CManipulator.h"
#include "DataContext.h"
#include <vector>

/*
@brief
*/
class CMeshModelViewManager;
namespace mip
{
	class Renderer;
}

class CPlaneManiplator : public CManipulator
{
public:
	static CPlaneManiplator* GetInstance();

private:
	static CPlaneManiplator* m_pInstance;

public:
	CPlaneManiplator(mip::Renderer* pRenderer);
	~CPlaneManiplator();

	void Init(DataContext* pDataContext);
	void Init(
		DataContext* pDataContext, 
		CMeshModelViewManager* pModelViewManager,
		CMeshDlgManager* pMeshDlgManager);

public:
	virtual void render(
		DataContext* pDataContext,
		mip::Renderer* _p_renderer,
		mip::MATRIX44& _mat_view_world,
		mip::MATRIX44& _mat_offset,
		mip::SCAMERA* _p_camera) override;

	virtual void DrawSnapping(QPainter* _p_paint) override;

	virtual void Reset() override;

public:
	mip::MeshTopology* getPlaneMeshPtr() { return m_pPlaneMesh; }
	mip::MATRIX44 getWorldMat();

private:
	void DrawPlaneOutline(QPainter* _p_paint);
	void DrawPlaneMesh();

private:
	mip::MeshTopology* m_pPlaneMesh;
	DataContext* m_pDataContext;
};

#define PLANE_MANIPULATOR (CPlaneManiplator::GetInstance())

