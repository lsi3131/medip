#pragma once

/*
	모든 리소스 접근 시 Thread에 안전한 API 제공
*/

#include <mutex>
#include "graphics/MeshInfo.h"
#include "graphics/MeshLayerData.h"

class OmniverseSharedResource
{
public:
	void Clear();
	void UpdateMeshData(const std::vector<MeshLayerData>& meshDataList);
	const std::vector<MeshLayerData> GetMeshDataList();

private:
	std::mutex m_mutex;
	std::vector<MeshLayerData> m_meshDataList;
};





