#pragma once

#include "graphics/MeshInfo.h"
#include <string>
#include <memory>

namespace mip
{
	class MeshTopology;
	class Renderer;
}

class MeshLayerDataOmniverse
{
public:
	MeshLayerDataOmniverse(mip::Renderer* pRenderer);
public:
	MeshInfo* GetInfo() const;
	mip::MeshTopology* GetData() const;

	bool LoadSTL(const std::string& filepath, const std::string& name);

private:
	std::shared_ptr<MeshInfo> m_pInfo;
	std::shared_ptr<mip::MeshTopology> m_pData;
	eOmniverseStatus m_status;
};
