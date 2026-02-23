#pragma once

#include "defineMEDIP.h"
#include "Omniverse/OmniverseHeader.h"
#include "Omniverse/OmniverseUtils.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Usd/mipUsdCustomLayerData.h"
#include "graphics/MeshInfo.h"

class mipUsdStagePrivate
{
public:
	mipUsdStagePrivate();

public:
	void EnablePhysics(const pxr::UsdPrim& prim, bool dynamic);

	std::vector<pxr::UsdPrim> GetPrimList(eOmniverseUsdType type) const;
	std::vector<pxr::UsdPrim> GetMeshPrimList() const;
	std::vector<pxr::UsdPrim> GetMaterialPrimList() const;

	std::vector<pxr::UsdGeomMesh> GetGeomMeshList() const;
	std::vector<pxr::UsdShadeMaterial> CreateMaterialList() const;

	std::vector<pxr::UsdLuxLight> CreateLightList() const;

	bool TryReceiveGeomMeshByName(pxr::UsdGeomMesh* pOutMesh, const std::string& name) const;
	bool TryReceiveShadeMaterialByName(pxr::UsdShadeMaterial* pOut, const std::string& name) const;

	bool TryGetTransformSRT(TransformSRT* pOut, const pxr::UsdGeomMesh& usdMesh);
	bool TrasformMesh(const pxr::UsdGeomMesh& usdMesh, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale);

	bool SetVisible(const pxr::UsdGeomMesh& usdMesh, bool value);

public:
	pxr::UsdStageRefPtr pUsdStage;
	GfVec3i RotationOrder;
};


