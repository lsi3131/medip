#pragma once

#include <QPoint>
#include <QSize>
#include "graphics/MeshLayerData.h"

namespace mip
{
	class Renderer;
}

class MeshData;
class VOLUME_DATA;
class CMeshWorkManager;
class CMeshModelViewManager;

class MeshEditScreenSelector
{
public:
	MeshEditScreenSelector(
		mip::Renderer* pRenderer,
		MeshData* pMeshData,
		VOLUME_DATA* pVolumeData,
		CMeshWorkManager* pMeshWorkManager,
		CMeshModelViewManager* pMeshModelViewManager
	);

public:
	bool TryGetIntersectedMesh(MeshLayerData* pOut, const QPoint& screenPoint, const QSize& screenSize);

private:
	mip::Renderer* m_pRenderer;
	MeshData* m_pMeshData;
	VOLUME_DATA* m_pVolumeData;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pMeshModelViewManager;
};
