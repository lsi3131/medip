#include "stdafx.h"
#include "mipUsdStage_p.h"
#include "Omniverse/xformUtils.h"
#include "Omniverse/primUtils.h"

mipUsdStagePrivate::mipUsdStagePrivate()
{
	RotationOrder = GfVec3i(0, 1, 2);
}

//pxr::UsdStageRefPtr mipUsdStagePrivate::GetStage() const
//{
//	return pUsdStage;
//}

void mipUsdStagePrivate::EnablePhysics(const pxr::UsdPrim& prim, bool dynamic)
{
	if (dynamic)
	{
		// Make the cube a physics rigid body dynamic
		pxr::UsdPhysicsRigidBodyAPI::Apply(prim);
	}

	// Add collision
	pxr::UsdPhysicsCollisionAPI::Apply(prim);

	if (prim.IsA<pxr::UsdGeomMesh>())
	{
		pxr::UsdPhysicsMeshCollisionAPI meshCollisionAPI = pxr::UsdPhysicsMeshCollisionAPI::Apply(prim);
		if (dynamic)
		{
			// set mesh approximation to convexHull for dynamic meshes
			meshCollisionAPI.GetApproximationAttr().Set(UsdPhysicsTokens->convexHull);
		}
		else
		{
			// set mesh approximation to none - triangle mesh as is will be used
			meshCollisionAPI.GetApproximationAttr().Set(UsdPhysicsTokens->none);
		}
	}
}

std::vector<pxr::UsdPrim> mipUsdStagePrivate::GetPrimList(eOmniverseUsdType type) const
{
	std::vector<pxr::UsdPrim> primList;

	if (pUsdStage == nullptr)
	{
		return primList;
	}

	pxr::UsdPrimRange range = pUsdStage->Traverse();
	for (const pxr::UsdPrim& node : range)
	{
		if (type == eOmniverseUsdType::Mesh)
		{
			if (node.IsA<pxr::UsdGeomMesh>())
			{
				primList.push_back(node);
			}
		}
		else if (type == eOmniverseUsdType::Material)
		{
			if (node.IsA<pxr::UsdShadeMaterial>())
			{
				primList.push_back(node);
			}
		}
		else if (type == eOmniverseUsdType::Light)
		{
			if (node.IsA<pxr::UsdLuxLight>())
			{
				primList.push_back(node);
			}
		}
		else
		{
			qCritical() << "invalid type : " << (int)type;
			Q_ASSERT(false);
		}
	}

	return primList;
}

std::vector<pxr::UsdPrim> mipUsdStagePrivate::GetMeshPrimList() const
{
	return GetPrimList(eOmniverseUsdType::Mesh);
}

std::vector<pxr::UsdPrim> mipUsdStagePrivate::GetMaterialPrimList() const
{
	return GetPrimList(eOmniverseUsdType::Material);
}

std::vector<pxr::UsdGeomMesh> mipUsdStagePrivate::GetGeomMeshList() const
{
	pxr::UsdGeomMesh m;
	std::vector<pxr::UsdGeomMesh> list;
	std::vector<pxr::UsdPrim> primList = GetPrimList(eOmniverseUsdType::Mesh);
	for (auto& prim : primList)
	{
		list.push_back(pxr::UsdGeomMesh(prim));
	}
	return list;
}

std::vector<pxr::UsdShadeMaterial> mipUsdStagePrivate::CreateMaterialList() const
{
	std::vector<pxr::UsdShadeMaterial> list;
	std::vector<pxr::UsdPrim> primList = GetPrimList(eOmniverseUsdType::Material);
	for (auto& prim : primList)
	{
		list.push_back(pxr::UsdShadeMaterial(prim));
	}
	return list;
}

std::vector<pxr::UsdLuxLight> mipUsdStagePrivate::CreateLightList() const
{
	std::vector<pxr::UsdLuxLight> luxList;
	std::vector<pxr::UsdPrim> primList = GetPrimList(eOmniverseUsdType::Light);
	for (auto& prim : primList)
	{
		luxList.push_back(pxr::UsdLuxLight(prim));
	}
	return luxList;
}

bool mipUsdStagePrivate::TryReceiveGeomMeshByName(pxr::UsdGeomMesh* pOutMesh, const std::string& name) const
{
	std::vector<pxr::UsdGeomMesh> meshList = GetGeomMeshList();
	for (auto& m : meshList)
	{
		if (name == m.GetPrim().GetName())
		{
			*pOutMesh = m;
			return true;
		}
	}
	return false;
}

bool mipUsdStagePrivate::TryReceiveShadeMaterialByName(pxr::UsdShadeMaterial* pOut, const std::string& name) const
{
	std::vector<pxr::UsdShadeMaterial> materialList = CreateMaterialList();
	for (auto& m : materialList)
	{
		if (name == m.GetPrim().GetName())
		{
			*pOut = m;
			return true;
		}
	}
	return false;
}

bool mipUsdStagePrivate::TryGetTransformSRT(TransformSRT* pOut, const pxr::UsdGeomMesh& usdMesh)
{
	GfVec3d translate_mm(0);
	GfVec3d rotXYZ(0);
	GfVec3d scale(1);
	if (!xformUtils::getLocalTransformSRT(usdMesh.GetPrim(), translate_mm, rotXYZ, RotationOrder, scale))
	{
		//std::cerr << "WARNING: Unable to read transformation on \"" << usdMesh.GetPath() << "\"" << std::endl;
		return false;
	}

	pOut->Translate_mm = translate_mm;
	pOut->Rotate = rotXYZ;
	pOut->Scale = scale;

	return true;
}

bool mipUsdStagePrivate::TrasformMesh(const pxr::UsdGeomMesh& usdMesh, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale)
{
	GfVec3d translate_mm = To_GfVec3d(v_translate_cm * 10.0f);
	GfVec3d rotXYZ = To_GfVec3d(v_rotate);
	GfVec3d scale = To_GfVec3d(v_scale);

	xformUtils::setLocalTransformSRT(usdMesh.GetPrim(), translate_mm, rotXYZ, RotationOrder, scale);

	omniClientLiveProcess();

	return true;
}

bool mipUsdStagePrivate::SetVisible(const pxr::UsdGeomMesh& usdMesh, bool value)
{
	std::string visibilityText;
	if (value)
	{
		visibilityText = "inherited";
	}
	else
	{
		visibilityText = "invisible";
	}

	TfToken token(visibilityText);
	VtValue tokenValue(token);
	usdMesh.CreateVisibilityAttr(tokenValue);

	return true;
}
