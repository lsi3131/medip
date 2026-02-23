#include "stdafx.h"
#include "MeshTopologyBuilder.h"
#include "Renderer/Renderer.h"
#include "Renderer/MeshTopology.h"

void MeshTopologyBuilder::BuildRenderBufferTopology(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::SRenderBufferParams* pParams)
{
	pRenderer->makeCurrent();
	if (pParams)
	{
		pMesh->buildRenderBufferTopology(pParams);
	}
	else
	{
		pMesh->buildRenderBufferTopology();
	}
	pRenderer->doneCurrent();
}

