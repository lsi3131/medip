#include "stdafx.h"
#include "OmniverseSharedResource.h"

void OmniverseSharedResource::Clear()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_meshDataList.clear();
}

void OmniverseSharedResource::UpdateMeshData(const std::vector<MeshLayerData>& meshDataList)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	qInfo() << "update mesh data";

	m_meshDataList = meshDataList;
}

const std::vector<MeshLayerData> OmniverseSharedResource::GetMeshDataList()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	qInfo() << "get mesh data";

	return m_meshDataList;
}
