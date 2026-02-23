#include "stdafx.h"
#include "MeshEditScreenSelector.h"
#include "Renderer/Renderer.h"
#include "graphics/MeshData.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "mipEngine/geometry.h"

MeshEditScreenSelector::MeshEditScreenSelector(
	mip::Renderer* pRenderer,
	MeshData* pMeshData,
	VOLUME_DATA* pVolumeData,
	CMeshWorkManager* pMeshWorkManager,
	CMeshModelViewManager* pMeshModelViewManager) :
	m_pRenderer(pRenderer),
	m_pMeshData(pMeshData),
	m_pVolumeData(pVolumeData),
	m_pMeshWorkManager(pMeshWorkManager),
	m_pMeshModelViewManager(pMeshModelViewManager)
{
}

bool MeshEditScreenSelector::TryGetIntersectedMesh(MeshLayerData* pOut, const QPoint& screenPoint, const QSize& screenSize)
{
	int selectedMeshIndex = -1;
	int selectedMeshCount = 0;
	int meshCount = m_pMeshData->GetMeshCount();
	for (int i = 0; i < meshCount; i++)
	{
		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(i);
		if (pMeshInfo && pMeshInfo->selected)
		{
			selectedMeshCount++;
			selectedMeshIndex = i;
		}
	}

	if (selectedMeshIndex < 0 || selectedMeshCount != 1)
	{
		m_pMeshWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		return false;
	}

	MeshLayerData meshLayer;
	for (int i = 0; i < meshCount; ++i)
	{
		meshLayer = m_pMeshData->Get(i);

		MeshInfo* pMeshInfo = meshLayer.Info;
		if (pMeshInfo == nullptr || !pMeshInfo->selected)
		{
			continue;
		}

		mip::MATRIX44 matWorld;
		int vertexIdx = -1;
		mip::VECTOR3 tracePoint(mip::VECTOR3(0, 0, 0));

		mip::MATRIX44 wvp = m_pRenderer->getWorld() * m_pRenderer->getView() * m_pRenderer->getProj();

		mip::MATRIX44 matOffset;
		mip::VECTOR3 offsetCenter = m_pVolumeData->GetOffsetCenter();

		matOffset.identity();
		matOffset.translation(offsetCenter);

		mip::MATRIX44 view_world = m_pMeshModelViewManager->GetMainTransform();

		mip::RAY ray_pre = mip::geom::ScreenToRay(
			mip::VECTOR2((float)screenPoint.x(), (float)screenPoint.y()),
			screenSize.width(), screenSize.height(),
			m_pMeshModelViewManager->GetCameraPtr()->getView(),
			m_pMeshModelViewManager->GetCameraPtr()->getProj(),
			&(meshLayer.Data->getMatrix() * matOffset * view_world));

		if (!meshLayer.Data->intersectRay(ray_pre.org, ray_pre.dir, tracePoint, vertexIdx, false))
		{
			//printf_s("no pick");
			meshLayer.Data = nullptr;
			continue;
		}
	}

	if (meshLayer.Data == nullptr)
	{
		return false;
	}

	*pOut = meshLayer;

	return true;
}

