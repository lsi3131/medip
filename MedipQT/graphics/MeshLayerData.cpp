#include "stdafx.h"
#include "MeshLayerData.h"
#include "Renderer/Mesh.h"
#include "Renderer/model.h"
#include "Renderer/Renderer.h"
#include "windowManager.h"

MeshLayerData::MeshLayerData() :
	Info(nullptr), Data(nullptr)
{
}

MeshLayerData::MeshLayerData(MeshInfo* info, mip::MeshTopology* data) :
	Info(info), Data(data)
{

}

bool MeshLayerData::BuildRenderBufferTopology(mip::Renderer* pRenderer)
{
	if (Data == nullptr)
	{
		return false;
	}

    pRenderer->makeCurrent();
	Data->buildRenderBufferTopology();
	pRenderer->doneCurrent();

    return true;
}
