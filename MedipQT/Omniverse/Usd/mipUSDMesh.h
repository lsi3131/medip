#pragma once

#include <string>
#include <memory>
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include "defineMEDIP.h"
#include "Omniverse/Usd/mipUSDMaterial.h"
#include "graphics/MeshLayerData.h"
#include "Omniverse/OmniverseHeader.h"
#include "Math/Vector.h"

//PXR_NAMESPACE_USING_DIRECTIVE

class mipUSDMesh
{
public:
	mipUSDMesh(pxr::UsdGeomMesh raw);
	virtual ~mipUSDMesh();

public:
	void Refresh();

	bool SetDataByMeshDataAndInfo(const mip::MeshTopology& data, const MeshInfo& info);
	bool SetDataByMeshLayer(const MeshLayerData& layer);

	bool CopyToMeshTopology(mip::MeshTopology* pOut) const;
	bool CopyToMeshLayerData(MeshLayerData* pOut) const;
	bool CopyToMeshInfo(MeshInfo* pOut) const;
	bool CopyToSRT(mip::MeshTopology* pOut) const;

	std::string GetName() const;
	std::string GetPath() const;

	void SetStatus(eOmniverseStatus status);
	eOmniverseStatus GetStatus() const;

	pxr::UsdGeomMesh GetData() const;

public:
	mip::VECTOR3 GetTranslation_mm() const;
	mip::VECTOR3 GetTranslation_cm() const;

	std::vector<mip::VECTOR3> GetVertice_mm() const;
	std::vector<mip::VECTOR3> GetVertice_cm() const;

	std::vector<mip::VECTOR3> GetNormals() const;
	std::vector<muint32> GetIndices() const;
	std::vector<muint32> GetFaceVertexCounts() const;

	mip::QUATERNION GetRotation() const;
	mip::VECTOR3 GetScale() const;

	COLOR GetColor() const;

	void SetVisible(bool value);
	bool IsVisible() const;

	bool BindMaterial(mipUSDMaterialPtr pMaterial);
	bool UnbindMaterial();
	mipUSDMaterialPtr GetMaterial() const;

public:
	bool operator==(const mipUSDMesh& rhs) const;
	bool operator!=(const mipUSDMesh& rhs) const;

public:
	pxr::VtArray<pxr::GfVec3f> points_mm;
	pxr::VtArray<pxr::GfVec3f> meshNormals;
	pxr::VtArray<int> vecIndices;
	pxr::VtArray<int> faceVertexCounts; //= { 4 };

	pxr::GfVec3d translation_mm;
	pxr::GfVec3d rotation;
	pxr::GfVec3d scale;
	pxr::GfVec3i rotationOrder;
	pxr::GfVec3f color;
private:
	bool m_isVisible;
	eOmniverseStatus m_status;
	pxr::UsdGeomMesh m_raw;
};

using mipUSDMeshPtr = std::shared_ptr<mipUSDMesh>;
