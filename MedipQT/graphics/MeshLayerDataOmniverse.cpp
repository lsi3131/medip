#include "stdafx.h"
#include "MeshLayerDataOmniverse.h"
#include "Renderer/Mesh.h"
#include "Renderer/model.h"
#include "windowManager.h"

MeshLayerDataOmniverse::MeshLayerDataOmniverse(mip::Renderer* pRenderer) :
	m_status(eOmniverseStatus::Unlock)
{
	m_pData = std::make_shared<mip::MeshTopology>(pRenderer);
	m_pInfo = std::make_shared<MeshInfo>();
}

MeshInfo* MeshLayerDataOmniverse::GetInfo() const
{
	return m_pInfo.get();
}

mip::MeshTopology* MeshLayerDataOmniverse::GetData() const
{
	return m_pData.get();
}

bool MeshLayerDataOmniverse::LoadSTL(const std::string& filepath, const std::string& name)
{
	if (!mip::model::LoadSTLFile(filepath, m_pData.get(), 0.1f))
	{
		qWarning() << "fail to load stl." << filepath.c_str();
		return false;
	}

	m_pData->mergingVertex();
	m_pData->buildTopologyHEdge();
	m_pData->buildTree();
	m_pData->updateVertex();

	m_pData->updateColor(mip::VECTOR4(1.0f, 0, 0, 1.0f));
	m_pData->enableAlpha(true);

	std::shared_ptr<MeshInfo> pInfo = std::make_shared<MeshInfo>();
	m_pInfo->uid = 0;
	m_pInfo->color = COLOR(255, 0, 0);
	m_pInfo->show = true;
	m_pInfo->selected = false;
	m_pInfo->upScale = false;
	m_pInfo->TrisCount = 0;
	m_pInfo->m_nSmoothLevel = 0;
	m_pInfo->m_nRemeshLevel = 0;
	m_pInfo->SetName(QString::fromStdString(name));

	return true;
}

