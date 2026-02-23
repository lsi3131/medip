#pragma once

#include <string>
#include "Omniverse/OmniverseHeader.h"
#include "Omniverse/Usd/mipUSDMesh.h"
#include "Omniverse/Usd/mipUSDMaterial.h"
#include "Omniverse/Usd/mipUSDLight.h"
#include "Omniverse/Usd/mipUsdCustomLayerData.h"

class mipUsdStagePrivate;

namespace mip
{
	class Renderer;
}

enum class eOmniverseUsdType
{
	Mesh,
	Material,
	Light,
};

class mipUsdStage
{
public:
	mipUsdStage();
	~mipUsdStage();

public:
	bool CreateNew(const std::string& filepath);
	bool Open(const std::string& filepath);
	bool IsOpen() const;

	bool Save();

	bool Close();
	
	bool DeleteAllPrimAsType(eOmniverseUsdType type);
	bool DeletePrimByNameAsType(const std::string& name, eOmniverseUsdType type);

	bool DeleteAllMesh();
	bool DeleteMeshByName(const std::string& name);

	bool DeleteAllMaterials();

	bool DeleteAllLights();

	bool Export(const std::string& exportFilePath);

	bool CopyUsdFile_CustomLayerRenderSetting(const std::string& filepath);
	bool CopyUsdFile_OnlyMaterialAndLight(const std::string& filepath);

	std::vector<mipUSDMeshPtr> CreateMeshList() const;
	mipUSDMeshPtr CreateMeshByName(const std::string& name) const;
	mipUSDMeshPtr CreateMeshByPath(const std::string& path) const;

	std::vector<mipUSDMaterialPtr> CreateMaterialList() const;
	mipUSDMaterialPtr CreateMeterialByName(const std::string& name) const;
	mipUSDMaterialPtr CreateMeterialByPath(const std::string& path) const;

	std::vector<mipUsdLightPtr> CreateLightList() const;

	bool AddMesh(const mip::MeshTopology& data, const MeshInfo& info, mipUSDMeshPtr* ppOutUsdMeshLayer = nullptr);
	bool AddMeshByStlFile(const std::string& inputFilePath, const std::string& meshLayerName, mip::Renderer* pRenderer, COLOR color = COLOR(255, 0, 0));

	bool TrasformMesh(const std::string& meshName, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale);
	bool TranslateMesh_cm(const std::string& meshName, mip::VECTOR3 v_translate_cm);

	bool AddMaterialByMDLFile(const std::string& filepath, const std::string& name, const std::string& subIdentifier);
	bool AddMaterial(const mipUSDMaterialPtr& material, const std::string& path);

	bool BindMeshMaterial(const std::string& meshName, const std::string& materialName);

	bool SetVisible(const std::string& meshName, bool value);
	bool IsMeshExist(const std::string& meshName) const;
	std::vector<std::string> GetMeshNameList() const;

	mipUsdCustomLayerData GetCustomLayerData() const;
	bool SetCustomLayerData(const mipUsdCustomLayerData& data);

	bool InsertSessionSubLayer(const mipUsdStage& stage);
	bool ClearSessionSubLayer();

	bool SetEditTarget(const mipUsdStage& stage);
	bool ClearEditTarget();

public:
	pxr::UsdStageRefPtr GetData() const;


private:
	std::shared_ptr<mipUsdStagePrivate> m_p;
};
