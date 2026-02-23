#pragma once

#ifndef MESHINFO_H
#define MESHINFO_H

#include "color.h"
#include "MeshInfo.h"
#include "graphics/Mesh/MeshTopologyBuilder.h"
#include "graphics/MeshLayerData.h"

#define MESHINFO_TEXT_LENGTH_MAX	256

namespace mip
{
	class Renderer;
}

class MeshLayerData;
class CMeshManipulator;

class MeshData
{
public:
	MeshData();
	MeshData(mip::Renderer* pRenderer, CMeshManipulator* pMeshManipulator);
	~MeshData();

public:
	void SetRenderer(mip::Renderer* pRenderer);
	void SetMeshManipulator(CMeshManipulator* pMeshManipulator);

	void ClearMeshInfo();
	MeshInfo* CreateMeshInfo(bool isCopy = false);

	bool AddMeshInfo(muint8 index, const MeshInfo&);
	bool DeleteMeshLayer(muint8 index);

	MeshInfo* GetMeshInfo(muint8 uid, bool layerUID = false) const;

	MeshInfo* GetCurrentMeshInfo();

	void SetCurrentMeshIndex(muint8 index);

	muint8 GetCurrentMeshIndex();
	QString GetCurrentMeshName();
	QColor GetCurrentMeshColor();

	bool TryGetMeshLayerIndexByInfo(int* pOutIndex, const MeshInfo* pInfo);

	mip::MeshTopology* GetCurrentMesh();

	int GetCurrentMeshLUID();//layer uid
	void SetConnectMesh(muint8 mUID, int lUID);
	void SetDisconnectMesh(muint8 lUID);
	void SetMeshName(QString meshName, int uid, bool layerUID = false);
	QString GetMeshName(int uid, bool layerUID = false);
	QColor GetMeshColor(int uid, bool layerUID = false);
	mint8 GetMeshUID(int layerUID) const;
	int GetMeshCount() const;
	int GetSelectMeshCount();
	mip::MeshTopology* GetMesh(int uid, bool layerUID = false) const;
	mip::MeshTopology* GetTempMesh(int uid, bool layerUID = false);
	bool InsertMesh(muint8 meshUID, mip::MeshTopology* pMesh, bool doBuildRenderBufferTopology = true);
	bool InsertMeshWithoutBuildBuffer(muint8 meshUID, mip::MeshTopology* pMesh);
	bool InsertMeshToMeshMap(muint8 meshUID, mip::MeshTopology* pMesh);	

	bool DeleteMeshTopologyData(muint8 meshUID);
	void ChangeMeshUID(muint8 prevUID, muint8 newUID, int layerUID = -1);
	void ChangeLUID(int preUID, int chUID);
	void MeshRenderUpdate(muint8 meshUID);
	void MoveMeshInfo(muint8 preUID, muint8 chUID);

	bool CopyMeshMap(muint8 meshUID, mip::MeshTopology* mesh);

	const QMap<mint32, muint8> getConnectList();

	bool visibleCheckAll();
	void SetMeshInfoModeSelectMode(muint8 meshUID, bool bSelect);
	void ClearMeshInfoSelectMode();

	bool IsVisibleLayerExist();
	bool IsLockLayerExist();

	bool InsertMeshTopology(muint8 meshUID, mip::MeshTopology* mesh, bool updateUI);
	std::vector<MeshLayerData> GetMeshDataList() const;
	MeshLayerData GetMeshDataByLayerIndex(muint8 layerIndex) const;

	COLOR GetNextMeshColor() const;

	bool AddNew(const MeshInfo& info, mip::MeshTopology* pMeshData, bool updateUI, MeshLayerData* pOut = nullptr);
	bool AddNew(const std::string& name, COLOR color, mip::MeshTopology* pMeshData, bool updateUI, MeshLayerData* pOut = nullptr);

	bool LoadSTL(const std::string& filepath, bool updateUI, MeshInfo** ppOutMeshInfo = nullptr, mip::MeshTopology** ppOutMeshData = nullptr);
	bool TryGetMeshDataByName(MeshLayerData* pOut, const std::string& name) const;
	bool IsMeshNameExist(const std::string& name) const;
	bool TryGetIndexByName(int* pOut, const std::string& name) const;

	MeshLayerData Get(int index);
	bool TryGet(MeshLayerData* pOut, int index) const;
	std::vector<MeshLayerData> GetList() const;
	std::vector<MeshLayerData> GetListByStatus(eOmniverseStatus status) const;

	bool Update(const MeshInfo& info, const mip::MeshTopology& data);

	void SetPick(int index, bool value);
	void SetPick(const MeshLayerData& data, bool value);

	bool IsPicked(int index) const;
	bool IsPicked(const MeshLayerData& data) const;

private:
	COLOR generateColor(muint32 index) const;
	mint32 generateIndexForMesh() const;

private:
	mip::Renderer* m_pRenderer;
	CMeshManipulator* m_pMeshManipulator;
	QMap<muint8, mip::MeshTopology*> m_meshUIDToTopologyMap;
	QVector<MeshInfo*> m_meshInfoList;
	QMap<mint32, muint8> m_layerUIDToMeshUIDMap;
	muint64 m_useMeshInfoBit;
	muint8 m_currentMeshInfoIndex;
	MeshTopologyBuilder m_meshTopologyBuilder;
};
#endif