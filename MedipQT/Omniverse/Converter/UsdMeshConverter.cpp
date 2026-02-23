#include "stdafx.h"
#include "UsdMeshConverter.h"
#include "UsdMeshPureDataConverter.h"
#include "Actions/ActionMesh.h"
#include "WindowManager.h"
#include "ActionManager.h"
#include "ShortcutManager.h"
#include "StringManager.h"
#include "Omniverse/OmniverseUtils.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

UsdMeshConverter::UsdMeshConverter(
	mip::Renderer* pRenderer,
	WindowManager* pWinManager,
	CMeshWorkManager* pMeshWorkManager,
	CMeshDlgManager* pMeshDlgManager,
	CPlaneManiplator* pPlaneManipulator) :
	m_pRenderer(pRenderer),
	m_pWinManager(pWinManager),
	m_pMeshWorkManager(pMeshWorkManager),
	m_pMeshDlgManager(pMeshDlgManager),
	m_pPlaneManipulator(pPlaneManipulator)
{
}

std::vector<MeshLayerData> UsdMeshConverter::AddList(DataContext* pDataContext, MeshData* pOutMeshData, const std::vector<mipUSDMeshPtr>& usdMeshList)
{
	std::vector<MeshLayerData> addMeshInfoList;

	if (pOutMeshData == nullptr)
	{
		return addMeshInfoList;
	}

	std::vector<MeshDataLayerInfo> meshLayerList;
	for (mipUSDMeshPtr pUsdMesh : usdMeshList)
	{
		std::string name = pUsdMesh->GetName();
		if (pOutMeshData->IsMeshNameExist(name) == false)
		{
			addToMeshDataLayerList(&meshLayerList, pUsdMesh);
		}
	}

	if (meshLayerList.empty())
	{
		return addMeshInfoList;
	}

	for (const MeshDataLayerInfo& meshLayer : meshLayerList)
	{
		MeshLayerData newMeshLayer;
		if (pOutMeshData->AddNew(meshLayer.Name, meshLayer.Color, meshLayer.pData, true, &newMeshLayer))
		{
			addMeshInfoList.push_back(newMeshLayer);
		}
	}

	std::vector<ActionAddMeshesData> dataList;
	for (auto& addMeshInfo : addMeshInfoList)
	{
		ActionAddMeshesData data;
		data.LayerUID = -1;
		data.MeshName = addMeshInfo.Info->GetName();
		data.pMeshData = addMeshInfo.Data;

		dataList.push_back(data);
	}

	QString cacheDirectoryPath =  STRING_MANAGER->cacheFilePath;
	ActionAddMeshes action(pDataContext, m_pWinManager, m_pMeshWorkManager, m_pMeshDlgManager, m_pPlaneManipulator, m_pRenderer, dataList, cacheDirectoryPath);
	action.Run();

	return addMeshInfoList;
}

std::vector<MeshLayerData> UsdMeshConverter::UpdateList(DataContext* pDataContext, MeshData* pOutMeshLayerContainer, const std::vector<mipUSDMeshPtr>& usdMeshList, bool updateUI)
{
	std::vector<MeshLayerData> updateMeshInfoList;
	for (auto& pUsdMesh : usdMeshList)
	{
		std::string name = pUsdMesh->GetName();
		MeshLayerData meshLayerData;
		if (pOutMeshLayerContainer->TryGetMeshDataByName(&meshLayerData, name))
		{
			if (meshLayerData.Info->GetOmniverseStatus() == eOmniverseStatus::Unlock)
			{
				updateMeshLayerByUsdMesh(&meshLayerData, pUsdMesh, updateUI);
				updateMeshInfoList.push_back(meshLayerData);
			}
		}
	}

	return updateMeshInfoList;
}

void UsdMeshConverter::addToMeshDataLayerList(std::vector<MeshDataLayerInfo>* pOutMeshDataLayer, const mipUSDMeshPtr pUsdMesh)
{
	if (UsdMeshPureDataConverter::CanConvertable(*pUsdMesh, eSupportMeshTypes::MipMesh) == false)
	{
		return;
	}

	mip::MeshTopology* pNewMesh = new mip::MeshTopology(m_pRenderer);
	std::string meshName = pUsdMesh->GetName();
	COLOR color = pUsdMesh->GetColor();

	if (UsdMeshPureDataConverter::ConvertUsdToMipMesh(pNewMesh, *pUsdMesh) == false)
	{
		SAFE_DELETE(pNewMesh);
		return;
	}

	MeshDataLayerInfo layerInfo;
	layerInfo.pData = pNewMesh;
	layerInfo.Color = color;
	layerInfo.Name = meshName;

	pOutMeshDataLayer->push_back(layerInfo);
}

void UsdMeshConverter::updateMeshLayerByUsdMesh(MeshLayerData* pTargetMeshLayer, const mipUSDMeshPtr pUsdMesh, bool updateUI)
{
	//qInfo() << "update mesh info and SRT : " << pTargetMeshLayer->Info->GetName();
	COLOR prevColor = pTargetMeshLayer->Info->color;

	pUsdMesh->CopyToMeshInfo(pTargetMeshLayer->Info);
	pUsdMesh->CopyToSRT(pTargetMeshLayer->Data);
	if (updateUI)
	{
		COLOR currentColor = pTargetMeshLayer->Info->color;
		if (prevColor != currentColor)
		{
			pTargetMeshLayer->Data->updateColor(To_mipVECTOR4_Color(pTargetMeshLayer->Info->color));
			pTargetMeshLayer->BuildRenderBufferTopology(m_pRenderer);
		}
	}
}
