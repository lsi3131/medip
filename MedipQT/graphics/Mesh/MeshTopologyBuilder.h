#pragma once

namespace mip
{
	class Renderer;
	class MeshTopology;
	class SRenderBufferParams;
}

class MeshTopologyBuilder
{
public:
	void BuildRenderBufferTopology(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::SRenderBufferParams* pParams = nullptr);
};
