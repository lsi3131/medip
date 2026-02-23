#pragma once

#include "graphics/MeshInfo.h"
#include <string>
#include <memory>

namespace mip
{
	class MeshTopology;
	class Renderer;
}

class MeshLayerData
{
public:
	MeshLayerData();
	MeshLayerData(MeshInfo* info, mip::MeshTopology* data);

public:
	bool BuildRenderBufferTopology(mip::Renderer* pRenderer);

public:
	MeshInfo* Info;
	mip::MeshTopology* Data;
	eOmniverseStatus Status = eOmniverseStatus::Unlock;
};


