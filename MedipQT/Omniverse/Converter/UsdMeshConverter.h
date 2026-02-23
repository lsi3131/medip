#pragma once

#include <QObject>
#include "Omniverse/Usd/mipUSDMesh.h"
#include "graphics/MeshData.h"

class DataContext;
class WindowManager;
class CMeshWorkManager;
class CMeshDlgManager;
class CPlaneManiplator;

namespace mip
{
	class Renderer;
}

struct MeshDataLayerInfo
{
	mip::MeshTopology* pData;
	std::string Name;
	COLOR Color;
};

class UsdMeshConverter : public QObject
{
public:
	Q_OBJECT
public:
	UsdMeshConverter(
		mip::Renderer* pRenderer, 
		WindowManager* pWinManager,
		CMeshWorkManager* pMeshWorkManager,
		CMeshDlgManager* pMeshDlgManager,
		CPlaneManiplator* pPlaneManipulator
		);

public:
	std::vector<MeshLayerData> AddList(DataContext* pDataContext, MeshData* pOutContainer, const std::vector<mipUSDMeshPtr>& usdMeshList);
	std::vector<MeshLayerData> UpdateList(DataContext* pDataContext, MeshData* pOutMeshLayerContainer, const std::vector<mipUSDMeshPtr>& usdMeshList, bool updateUI);

private:
	void addToMeshDataLayerList(std::vector<MeshDataLayerInfo>* pOutMeshDataLayer, const mipUSDMeshPtr pUsdMesh);

	void updateMeshLayerByUsdMesh(MeshLayerData* pTargetMeshLayer, const mipUSDMeshPtr pUsdMesh, bool updateUI);

private:
	mip::Renderer* m_pRenderer;
	WindowManager* m_pWinManager;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshDlgManager* m_pMeshDlgManager;
	CPlaneManiplator* m_pPlaneManipulator;
};
