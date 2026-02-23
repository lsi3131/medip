#include "stdafx.h"
#include "MeshData.h"
#include "windowManager.h"
#include "stringManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshManipulator.h"
#include "Renderer/Renderer.h"
#include "Renderer/model.h"
#include "ActionManager.h"

MeshData::MeshData() :
	MeshData(g_Renderer, MESH_MANIPULATOR)
{
}

MeshData::MeshData(mip::Renderer* pRenderer, CMeshManipulator* pMeshManipulator)
{
	m_pRenderer = pRenderer;
	m_pMeshManipulator = pMeshManipulator;
	m_currentMeshInfoIndex = 0;
	m_useMeshInfoBit = 0;
}

MeshData::~MeshData()
{

}

void MeshData::SetRenderer(mip::Renderer* pRenderer)
{
	m_pRenderer = pRenderer;
}

void MeshData::SetMeshManipulator(CMeshManipulator* pMeshManipulator)
{
	m_pMeshManipulator = pMeshManipulator;
}

void MeshData::ClearMeshInfo()
{
	m_pRenderer->makeCurrent();
	for (mip::MeshTopology* pMeshData : m_meshUIDToTopologyMap)
	{
		if (pMeshData)
		{
			SAFE_DELETE(pMeshData);
		}
	}
	m_meshUIDToTopologyMap.clear();

	for (MeshInfo* pInfo : m_meshInfoList)
	{
		if (pInfo)
		{
			SAFE_DELETE(pInfo);
		}
	}
	m_meshInfoList.clear();

	m_layerUIDToMeshUIDMap.clear();

	m_useMeshInfoBit = 0;

	m_pMeshManipulator->clear();

	m_pRenderer->doneCurrent();
}

MeshInfo* MeshData::CreateMeshInfo(bool isCopy)
{
	if (m_meshInfoList.size() >= MESH_MAX)
	{
		return nullptr;
	}

	m_currentMeshInfoIndex = m_meshInfoList.size();

	MeshInfo* meshInfo = new MeshInfo;
	int nameIndex = m_meshInfoList.count();
	muint8 UID = generateIndexForMesh();

	// 201109 허 건 대리 주석처리
	// 프로그램 종료현상 발생
	{
		//meshInfoList.insert(UID, meshInfo);
		//meshInfo->uid = -1;
		//meshInfo->color = generateColor(UID);

		//meshInfo->show = true;
		//if (!isCopy)
		//	SetMeshName(QString("Mesh %1").arg(nameIndex++), UID);

		//useMeshInfoBit |= (1 << UID);
	}

	{
		m_meshInfoList.insert(nameIndex, meshInfo);
		meshInfo->uid = -1;
		meshInfo->color = generateColor(nameIndex);

		meshInfo->show = true;
		if (!isCopy)
		{
			SetMeshName(QString("Mesh %1").arg(nameIndex), nameIndex);
		}

		m_useMeshInfoBit |= (1 << nameIndex);
	}

	meshInfo->show = true;
	meshInfo->selected = false;
	meshInfo->upScale = false;
	meshInfo->TrisCount = 0;

	meshInfo->m_nSmoothLevel = 0;
	meshInfo->m_nRemeshLevel = 0;

	return meshInfo;
}

bool MeshData::AddMeshInfo(muint8 index, const MeshInfo& info)
{
	if (m_meshInfoList.size() >= MESH_MAX)
	{
		return false;
	}

	if (index >= MESH_MAX)
	{
		return false;
	}

	MeshInfo* newInfo = new MeshInfo(info);
	m_useMeshInfoBit |= (1 << index);

	m_meshInfoList.insert(index, newInfo);
	m_currentMeshInfoIndex = index;

	return true;
}

bool MeshData::DeleteMeshLayer(muint8 index)
{
	DeleteMeshTopologyData(index);

	m_meshInfoList.remove(index);
	m_useMeshInfoBit &= ~(1 << index);
	m_currentMeshInfoIndex = 0;

	return true;
}

MeshInfo* MeshData::GetMeshInfo(muint8 uid, bool layerUID) const
{
	if (layerUID)
	{
		auto k = m_layerUIDToMeshUIDMap.find(uid);
		if (k != m_layerUIDToMeshUIDMap.end())
		{
			uid = k.value();
		}
	}

	if ((uid >= m_meshInfoList.size()) || uid < 0)
	{
		return nullptr;
	}

	return m_meshInfoList.at(uid);
}

MeshInfo* MeshData::GetCurrentMeshInfo()
{
	if (m_meshInfoList.count() <= m_currentMeshInfoIndex)
	{
		return nullptr;
	}

	return m_meshInfoList.at(m_currentMeshInfoIndex);
}

void MeshData::SetCurrentMeshIndex(muint8 index)
{
	if ((index < 0) || (index >= m_meshUIDToTopologyMap.count()))
	{
		m_currentMeshInfoIndex = 0;
	}

	m_currentMeshInfoIndex = index;
}

QString MeshData::GetCurrentMeshName()
{
	return GetMeshName(m_currentMeshInfoIndex);
}

QColor MeshData::GetCurrentMeshColor()
{
	return GetMeshColor(m_currentMeshInfoIndex);
}

bool MeshData::TryGetMeshLayerIndexByInfo(int* pOutIndex, const MeshInfo* pInfo)
{
	for (int i = 0; i < m_meshInfoList.size(); ++i)
	{
		if (m_meshInfoList[i] == pInfo)
		{
			*pOutIndex = i;
			return true;
		}
	}

	return false;
}

mip::MeshTopology* MeshData::GetCurrentMesh()
{
	return GetMesh(m_currentMeshInfoIndex);
}

int MeshData::GetCurrentMeshLUID()
{
	MeshInfo* info = GetCurrentMeshInfo();

	if (info)
	{
		return info->uid;
	}

	return -1;
}

void MeshData::SetConnectMesh(muint8 mUID, int lUID)
{
	SetDisconnectMesh(lUID);

	m_layerUIDToMeshUIDMap.insert(lUID, mUID);

	MeshInfo* info = GetMeshInfo(mUID);

	if (info)
	{
		info->uid = lUID;
	}
}

void MeshData::SetDisconnectMesh(muint8 lUID)
{
	auto it = m_layerUIDToMeshUIDMap.find(lUID);
	if (it != m_layerUIDToMeshUIDMap.end())
	{
		MeshInfo* info = GetMeshInfo(it.value());

		if (info)
		{
			info->uid = -1;
		}

		m_layerUIDToMeshUIDMap.remove(lUID);
	}
}

void MeshData::SetMeshName(QString meshName, int uid, bool layerUID)
{
	if (layerUID)
	{
		auto k = m_layerUIDToMeshUIDMap.find(uid);

		if (k != m_layerUIDToMeshUIDMap.end())
		{
			uid = k.value();
		}
		else
		{
			return;
		}
	}

	if (uid >= m_meshInfoList.size())
	{
		return;
	}

	MeshInfo* info = m_meshInfoList.at(uid);
	if (info)
	{
		::StringCbPrintf(info->MeshName, MESHINFO_TEXT_LENGTH_MAX * sizeof(WCHAR),
			meshName.toStdWString().c_str());

		info->NameLength = meshName.length();
	}

}

QString MeshData::GetMeshName(int uid, bool layerUID)
{
	if (layerUID)
	{
		auto k = m_layerUIDToMeshUIDMap.find(uid);

		if (k != m_layerUIDToMeshUIDMap.end())
		{
			uid = k.value();
		}
		else
		{
			return QString();
		}
	}

	if (uid >= m_meshInfoList.size())
	{
		return QString();
	}
	MeshInfo* info = m_meshInfoList.at(uid);

	if (info)
	{
		return QString((QChar*)info->MeshName);
	}

	return QString();
}

QColor MeshData::GetMeshColor(int uid, bool layerUID /*= false*/)
{
	if (layerUID)
	{
		auto k = m_layerUIDToMeshUIDMap.find(uid);

		if (k != m_layerUIDToMeshUIDMap.end())
		{
			uid = k.value();
		}
		else
		{
			return QColor(255, 255, 255);
		}
	}

	if (uid >= m_meshInfoList.size())
	{
		return QColor(255, 255, 255);
	}

	MeshInfo* pInfo = m_meshInfoList.at(uid);

	if (pInfo)
	{
		QColor color(pInfo->color.r, pInfo->color.g, pInfo->color.b);

		return color;
	}

	return QColor(255, 255, 255);
}

mint8 MeshData::GetMeshUID(int layerUID) const
{
	auto it = m_layerUIDToMeshUIDMap.find(layerUID);

	if (it != m_layerUIDToMeshUIDMap.end())
	{
		return it.value();
	}

	return -1;
}

int MeshData::GetMeshCount() const
{
	return m_meshInfoList.count();
}

int	MeshData::GetSelectMeshCount()
{
	int nMesh = GetMeshCount();

	int cntSelect = 0;
	for (int idx = 0; idx < nMesh; ++idx)
	{
		MeshInfo* pMeshInfo = GetMeshInfo(idx);

		if (pMeshInfo && pMeshInfo->selected)
		{
			cntSelect++;
		}
	}

	return cntSelect;
}

mip::MeshTopology* MeshData::GetMesh(int uid, bool layerUID) const
{
	if (layerUID)
	{
		uid = GetMeshUID(uid);
	}

	if (uid >= 0)
	{
		auto k = m_meshUIDToTopologyMap.find(uid);

		if (k != m_meshUIDToTopologyMap.end())
		{
			return k.value();
		}
	}

	return nullptr;
}

mip::MeshTopology* MeshData::GetTempMesh(int uid, bool layerUID /*= false*/)
{
	if (layerUID)
	{
		uid = GetMeshUID(uid);
	}

	if (uid >= 0)
	{
		auto k = m_meshUIDToTopologyMap.find(uid);

		if (k != m_meshUIDToTopologyMap.end())
		{
			mip::MeshTopology* orgMesh = k.value();
			if (NULL != orgMesh)
			{
				mip::MeshTopology* mesh = new mip::MeshTopology(g_Renderer);

				int _count = orgMesh->m_verts.size();
				mesh->m_verts.reserve(_count);
				mesh->m_verts.assign(orgMesh->m_verts.begin(), orgMesh->m_verts.end());

				_count = orgMesh->m_tris.size();
				mesh->m_tris.reserve(_count);
				mesh->m_tris.assign(orgMesh->m_tris.begin(), orgMesh->m_tris.end());

				_count = orgMesh->m_normals.size();
				mesh->m_normals.reserve(_count);
				mesh->m_normals.assign(orgMesh->m_normals.begin(), orgMesh->m_normals.end());

				_count = orgMesh->m_vertColors.size();
				mesh->m_vertColors.reserve(_count);
				mesh->m_vertColors.assign(orgMesh->m_vertColors.begin(), orgMesh->m_vertColors.end());

				_count = orgMesh->m_tverts.size();
				mesh->m_tverts.reserve(_count);
				mesh->m_tverts.assign(orgMesh->m_tverts.begin(), orgMesh->m_tverts.end());

				_count = orgMesh->m_ttris.size();
				mesh->m_ttris.reserve(_count);
				mesh->m_ttris.assign(orgMesh->m_ttris.begin(), orgMesh->m_ttris.end());

				_count = orgMesh->m_tVHedges.size();
				mesh->m_tVHedges.reserve(_count);
				mesh->m_tVHedges.assign(orgMesh->m_tVHedges.begin(), orgMesh->m_tVHedges.end());

				mesh->m_boundingBox.max = orgMesh->m_boundingBox.max;
				mesh->m_boundingBox.min = orgMesh->m_boundingBox.min;

				mesh->m_treeTris = orgMesh->m_treeTris;
				mesh->m_baseColor = orgMesh->m_baseColor;
				//orgMesh->m_treeTris = nullptr;

				mesh->setTopologyed(orgMesh->isTopologyed());

				//m_pWinManager->makeCurrent();
				//mesh->buildRenderBufferTopology();
				//m_pWinManager->doneCurrent();

				return mesh;
			}
		}
	}

	return nullptr;
}

bool MeshData::InsertMeshToMeshMap(muint8 meshUID, mip::MeshTopology* mesh)
{
	if (!mesh)
	{
		return false;
	}

	if (!mesh->isTopologyed())
	{
		mesh->mergingVertex();
		mesh->buildTopologyHEdge();
		mesh->buildTree();
		mesh->updateVertex();

		MeshInfo* mInfo = GetMeshInfo(meshUID);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, mesh->getAlphaVal() / 255.0f));
		mesh->updateColor(color);
		mesh->enableAlpha(true);
	}
	else
	{
		mesh->enableAlpha(true);
		mesh->buildTree();
	}

	m_pRenderer->makeCurrent();
	mesh->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();

	m_meshUIDToTopologyMap.insert(meshUID, mesh);

	m_pMeshManipulator->InsertManipulator(meshUID);

	return true;
}

bool MeshData::InsertMesh(muint8 meshUID, mip::MeshTopology* pMesh, bool doBuildRenderBufferTopology)
{
	if (pMesh == nullptr)
	{
		return false;
	}

	MeshInfo* pInfo = GetMeshInfo(meshUID);
	if (pInfo == nullptr)
	{
		return false;
	}

	if (!pMesh->isTopologyed())
	{
		pMesh->mergingVertex();
		pMesh->buildTopologyHEdge();
		pMesh->buildTree();
		pMesh->updateVertex();

		mip::VECTOR4 color(mip::VECTOR4(pInfo->color.r / 255.0f, pInfo->color.g / 255.0f, pInfo->color.b / 255.0f, pMesh->getAlphaVal() / 255.0f));
		pMesh->updateColor(color);
		pMesh->enableAlpha(true);
	}
	else
	{
		pMesh->enableAlpha(true);
	}

	if (doBuildRenderBufferTopology)
	{
		m_meshTopologyBuilder.BuildRenderBufferTopology(m_pRenderer, pMesh);
	}

	m_meshUIDToTopologyMap.insert(meshUID, pMesh);

	m_pMeshManipulator->InsertManipulator(meshUID);

	return true;
}

bool MeshData::InsertMeshWithoutBuildBuffer(muint8 meshUID, mip::MeshTopology* pMesh)
{
	return InsertMesh(meshUID, pMesh, false);
}

bool MeshData::DeleteMeshTopologyData(muint8 meshUID)
{
	auto it = m_meshUIDToTopologyMap.find(meshUID);
	if (it != m_meshUIDToTopologyMap.end())
	{
		SAFE_DELETE(it.value());
		m_meshUIDToTopologyMap.erase(it);

		m_pMeshManipulator->deleteManipulator(meshUID);

		return true;
	}
	return false;
}

void MeshData::ChangeMeshUID(muint8 prevUID, muint8 newUID, int layerUID)
{
	if (prevUID == newUID)
	{
		return;
	}

	auto it = m_meshUIDToTopologyMap.find(prevUID);

	if (it != m_meshUIDToTopologyMap.end())
	{
		mip::MeshTopology* m = it.value();
		m_meshUIDToTopologyMap.erase(it);
		m_meshUIDToTopologyMap.insert(newUID, m);
	}

	if (layerUID != -1)
	{
		auto l = m_layerUIDToMeshUIDMap.find(layerUID);

		if (l != m_layerUIDToMeshUIDMap.end())
		{
			m_layerUIDToMeshUIDMap[layerUID] = newUID;
		}
	}

	if (prevUID < MESH_MAX)
	{
		m_useMeshInfoBit &= ~(1 << prevUID);
	}

	if (newUID < MESH_MAX)
	{
		m_useMeshInfoBit |= 1 << newUID;
	}
}

void MeshData::ChangeLUID(int preUID, int chUID)
{
	mint8 meshUID = GetMeshUID(preUID);

	if (meshUID == -1)
	{
		return;
	}

	MeshInfo* pInfo = GetMeshInfo(meshUID);

	if (pInfo)
	{
		pInfo->uid = chUID;
	}

	auto it = m_layerUIDToMeshUIDMap.find(preUID);

	if (it != m_layerUIDToMeshUIDMap.end())
	{
		m_layerUIDToMeshUIDMap.erase(it);
	}

	if (chUID != -1)
	{
		m_layerUIDToMeshUIDMap.insert(chUID, meshUID);
	}
}

void MeshData::MeshRenderUpdate(muint8 meshUID)
{
	mip::MeshTopology* m = GetMesh(meshUID);

	if (NULL == m)
	{
		return;
	}

	m_pRenderer->makeCurrent();
	m->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();
}

void MeshData::MoveMeshInfo(muint8 preUID, muint8 chUID)
{
	/*if (preUID < chUID)
		chUID--;*/

	MeshInfo* pInfo = m_meshInfoList.takeAt(preUID);

	if (pInfo)
	{
		m_meshInfoList.insert(chUID, pInfo);
	}
}

const QMap<mint32, muint8> MeshData::getConnectList()
{
	return QMap<mint32, muint8>(m_layerUIDToMeshUIDMap);
}

mint32 MeshData::generateIndexForMesh() const
{
	muint64 index = 0;
	for (; index < MESH_MAX; index++)
	{
		if (!(m_useMeshInfoBit & (1 << index)))
		{
			break;
		}
	}
	return index;
}

bool MeshData::visibleCheckAll()
{
	bool res = true;

	for (int i = 0; i < m_meshInfoList.count(); i++)
	{
		MeshInfo* pInfo = m_meshInfoList.at(i);

		if (pInfo)
		{
			res = pInfo->show;
		}

		if (!res)
		{
			break;
		}
	}

	return res;
}

void MeshData::SetMeshInfoModeSelectMode(muint8 meshUID, bool bSelect)
{
	MeshInfo* pMeshInfo = GetMeshInfo(meshUID);

	if (pMeshInfo)
	{
		pMeshInfo->selected = bSelect;
	}
}

void MeshData::ClearMeshInfoSelectMode()
{
	int nMesh = GetMeshCount();

	for (int mUID = 0; mUID < m_meshInfoList.count(); mUID++)
	{
		MeshInfo* pMeshInfo = GetMeshInfo(mUID);

		if (pMeshInfo)
		{
			pMeshInfo->selected = false;
		}
	}
}

bool MeshData::IsVisibleLayerExist()
{
	for (int index = 0; index < m_meshInfoList.count(); index++)
	{
		MeshInfo* pMeshInfo = GetMeshInfo(index);

		if (pMeshInfo)
		{
			if (pMeshInfo->show)
			{
				return true;
			}
		}
	}

	return false;
}

bool MeshData::IsLockLayerExist()
{
	for (int index = 0; index < m_meshInfoList.count(); index++)
	{
		MeshInfo* pMeshInfo = GetMeshInfo(index);

		if (pMeshInfo)
		{
			if (pMeshInfo->GetOmniverseStatus() == eOmniverseStatus::Lock)
			{
				return true;
			}
		}
	}

	return false;
}

COLOR MeshData::GetNextMeshColor() const
{
	int lastIndex = m_meshInfoList.size() - 1;
	return generateColor(lastIndex + 1);
}

COLOR MeshData::generateColor(muint32 index) const
{
	static COLOR colorTable[] =
	{
		COLOR(170,0,0,255), COLOR(66,255,255,255), COLOR(59,255,56,255),
		COLOR(255,33,244,255), COLOR(165,127,231,255), COLOR(214,117,130,255),
		COLOR(51,135,214,255), COLOR(214,168,127,255)
	};

	if (index >= 8)
	{
		index = index % 8;
	}

	return colorTable[index];
}

bool MeshData::InsertMeshTopology(muint8 meshUID, mip::MeshTopology* pMesh, bool updateUI)
{
	if (pMesh == nullptr)
	{
		return false;
	}

	if (!pMesh->isTopologyed())
	{
		pMesh->mergingVertex();
		pMesh->buildTopologyHEdge();
		pMesh->buildTree();
		pMesh->updateVertex();

		//mesh->m_tverts.push_back(mip::TVert());
		//mesh->m_ttris.push_back(mip::TTri());

		MeshInfo* mInfo = GetMeshInfo(meshUID);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, pMesh->getAlphaVal() / 255.0f));
		pMesh->updateColor(color);
		pMesh->enableAlpha(true);
	}
	else
	{
		pMesh->m_tverts.clear();
		pMesh->m_ttris.clear();

		pMesh->m_tverts.push_back(mip::TVert());
		pMesh->m_ttris.push_back(mip::TTri());

		pMesh->enableAlpha(true);
	}

	if (updateUI)
	{
		m_meshTopologyBuilder.BuildRenderBufferTopology(m_pRenderer, pMesh);
	}

	m_meshUIDToTopologyMap.insert(meshUID, pMesh);

	if (updateUI)
	{
		m_pMeshManipulator->InsertManipulator(meshUID);
	}

	return true;
}

bool MeshData::TryGetMeshDataByName(MeshLayerData* pOut, const std::string& name) const
{
	std::vector<MeshLayerData> list = GetMeshDataList();
	for (auto& m : list)
	{
		std::string curName = QString::fromWCharArray(m.Info->MeshName).toStdString();
		if (curName == name)
		{
			*pOut = m;
			return true;
		}
	}
	return false;
}

std::vector<MeshLayerData> MeshData::GetMeshDataList() const
{
	std::vector<MeshLayerData> list;
	for (int i = 0; i < GetMeshCount(); ++i)
	{
		list.push_back(GetMeshDataByLayerIndex(i));
	}

	return list;
}

MeshLayerData MeshData::GetMeshDataByLayerIndex(muint8 layerIndex) const
{
	MeshLayerData m;
	if ((layerIndex >= m_meshInfoList.size()) || layerIndex < 0)
	{
		return m;
	}

	mip::MeshTopology* topology = GetMesh(layerIndex);
	if (topology == nullptr)
	{
		return m;
	}

	m.Info = m_meshInfoList.at(layerIndex);
	m.Data = topology;

	return m;
}

muint8 MeshData::GetCurrentMeshIndex()
{
	return m_currentMeshInfoIndex;
}

bool MeshData::CopyMeshMap(muint8 meshUID, mip::MeshTopology* mesh)
{
	if (mesh == 0)
	{
		return false;
	}

	mip::MeshTopology* copy_mesh = new mip::MeshTopology(g_Renderer);

	// copy core
	copy_mesh->m_tris.resize(mesh->m_tris.size());
	std::copy(mesh->m_tris.begin(), mesh->m_tris.end(), copy_mesh->m_tris.begin());

	copy_mesh->m_verts.resize(mesh->m_verts.size());
	std::copy(mesh->m_verts.begin(), mesh->m_verts.end(), copy_mesh->m_verts.begin());

	// copy topology
	// tree 는 제외
	copy_mesh->m_ttris.resize(mesh->m_ttris.size());
	std::copy(mesh->m_ttris.begin(), mesh->m_ttris.end(), copy_mesh->m_ttris.begin());

	copy_mesh->m_tverts.resize(mesh->m_tverts.size());
	std::copy(mesh->m_tverts.begin(), mesh->m_tverts.end(), copy_mesh->m_tverts.begin());

	copy_mesh->m_tVHedges.resize(mesh->m_tVHedges.size());
	std::copy(mesh->m_tVHedges.begin(), mesh->m_tVHedges.end(), copy_mesh->m_tVHedges.begin());

	copy_mesh->updateVertex();

	copy_mesh->m_baseColor = mesh->m_baseColor;
	copy_mesh->updateColor(copy_mesh->m_baseColor);

	copy_mesh->setTopologyed(true);

	copy_mesh->setRotate(mesh->rotation);
	copy_mesh->setTranslate(mesh->translation);
	copy_mesh->setScale(mesh->scale);

	copy_mesh->m_boundingBox = mesh->m_boundingBox;

	m_pRenderer->makeCurrent();
	copy_mesh->buildRenderBufferTopology();
	m_pRenderer->doneCurrent();

	m_meshUIDToTopologyMap.insert(meshUID, copy_mesh);

	return true;
}

bool MeshData::AddNew(const MeshInfo& info, mip::MeshTopology* pMeshData, bool updateUI, MeshLayerData* pOut)
{
	// create mesh info
	if (IsMeshNameExist(info.GetName().toStdString()))
	{
		return false;
	}

	CreateMeshInfo();

	muint32 uid = GetCurrentMeshIndex();
	MeshInfo* pMeshInfo = GetMeshInfo(uid);

	if (pMeshInfo)
	{
		pMeshInfo->upScale = false;
		pMeshInfo->uid = uid;
		pMeshInfo->SetName(info.GetName());
		pMeshInfo->color = info.color;
	}

	// insert mesh
	InsertMeshTopology(uid, pMeshData, updateUI);

	if (updateUI)
	{
		MeshRenderUpdate(uid);
	}

	if (pOut)
	{
		*pOut = Get(uid);
	}

	return true;
}

bool MeshData::AddNew(const std::string& name, COLOR color, mip::MeshTopology* pMeshData, bool updateUI, MeshLayerData* pOut)
{
	MeshInfo info;
	info.SetName(QString::fromStdString(name));
	info.color = color;

	return AddNew(info, pMeshData, updateUI, pOut);
}

bool MeshData::LoadSTL(const std::string& filepath, bool updateUI, MeshInfo** ppOutMeshInfo, mip::MeshTopology** ppOutMeshData)
{
	mip::MeshTopology* pMeshData = new mip::MeshTopology(g_Renderer);

	if (!mip::model::LoadSTLFile(filepath, pMeshData, 0.1f))
	{
		qWarning() << "fail to load stl. path : " << filepath.c_str();
		SAFE_DELETE(pMeshData);
		return false;
	}

	// create mesh info
	CreateMeshInfo();

	muint32 uid = GetCurrentMeshIndex();
	MeshInfo* pMeshInfo = GetMeshInfo(uid);

	if (pMeshInfo)
	{
		pMeshInfo->upScale = false;
		pMeshInfo->uid = uid;
	}

	// insert mesh
	InsertMeshTopology(uid, pMeshData, updateUI);
	if (updateUI)
	{
		MeshRenderUpdate(uid);
	}

	if (ppOutMeshInfo)
	{
		*ppOutMeshInfo = pMeshInfo;
	}

	if (ppOutMeshData)
	{
		*ppOutMeshData = pMeshData;
	}

	return true;
}

bool MeshData::IsMeshNameExist(const std::string& name) const
{
	MeshLayerData d;
	return TryGetMeshDataByName(&d, name);
}

bool MeshData::TryGetIndexByName(int* pOut, const std::string& name) const
{
	std::vector<MeshLayerData> list = GetList();
	for (int i = 0; i < list.size(); ++i)
	{
		if (list[i].Info->GetName() == QString::fromStdString(name))
		{
			*pOut = i;
			return true;
		}
	}

	return false;
}

MeshLayerData MeshData::Get(int index)
{
	MeshLayerData d;
	if (TryGet(&d, index) == false)
	{
		Q_ASSERT(false);
	}
	return d;
}

bool MeshData::TryGet(MeshLayerData* pOut, int index) const
{
	std::vector<MeshLayerData> list = GetList();
	int count = list.size();
	if (index < 0 || index >= count)
	{
		return false;
	}

	*pOut = list[index];
	return true;
}

std::vector<MeshLayerData> MeshData::GetList() const
{
	return GetMeshDataList();
}

std::vector<MeshLayerData> MeshData::GetListByStatus(eOmniverseStatus status) const
{
	std::vector<MeshLayerData> meshList_All = GetMeshDataList();
	std::vector<MeshLayerData> meshList_Target;
	for (auto& m : meshList_All)
	{
		if (m.Info->GetOmniverseStatus() == status)
		{
			meshList_Target.push_back(m);
		}
	}

	return meshList_Target;
}

bool MeshData::Update(const MeshInfo& info, const mip::MeshTopology& data)
{
	MeshLayerData layerData;
	if (TryGetMeshDataByName(&layerData, info.GetName().toStdString()) == false)
	{
		return false;
	}

	qInfo() << "update layer data : " << layerData.Info->GetName();

	QString tempStlPath = STRING_MANAGER->m_strAppDataLocalPath + "/temp.stl";

	bool bEnableProgress = false;
	QString qstrProgressTitle = "";
	ACTION_MANAGER->action_FileWork_Export_Mesh_To_STLFile_Single(DATA_CONTEXT, tempStlPath, (mip::MeshTopology*)&data, info.upScale, bEnableProgress, qstrProgressTitle);

	if (mip::model::LoadSTLFile(tempStlPath.toStdString(), layerData.Data, 0.1f) == false)
	{
		qInfo() << "fail to load STL";
		Q_ASSERT(false);
	}


	return true;
}

void MeshData::SetPick(int index, bool value)
{
	MeshInfo* pMeshInfo = GetMeshInfo(index);
	if (pMeshInfo)
	{
		pMeshInfo->selected = value;
	}
}

void MeshData::SetPick(const MeshLayerData& data, bool value)
{
	int index = 0;
	if (TryGetIndexByName(&index, data.Info->GetName().toStdString()) == false)
	{
		return;
	}
	SetPick(index, value);
}

bool MeshData::IsPicked(int index) const
{
	MeshInfo* pMeshInfo = GetMeshInfo(index);
	if (pMeshInfo)
	{
		return pMeshInfo->selected;
	}
	return false;
}

bool MeshData::IsPicked(const MeshLayerData& data) const
{
	int index = 0;
	if (TryGetIndexByName(&index, data.Info->GetName().toStdString()) == false)
	{
		return false;
	}

	return IsPicked(index);
}
