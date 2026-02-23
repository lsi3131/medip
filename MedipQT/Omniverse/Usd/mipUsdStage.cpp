#include "stdafx.h"
#include "mipUsdStage.h"
#include "mipUsdStage_p.h"
#include "Omniverse/OmniverseHeader.h"
#include "Omniverse/xformUtils.h"
#include "Omniverse/Usd/mipUsdUtil.h"
#include "graphics/MeshLayerDataOmniverse.h"

mipUsdStage::mipUsdStage()
{
	m_p = std::make_shared<mipUsdStagePrivate>();
}

mipUsdStage::~mipUsdStage()
{
}

bool mipUsdStage::CreateNew(const std::string& filepath)
{
	m_p->pUsdStage = pxr::UsdStage::CreateNew(filepath);

	if (m_p->pUsdStage == nullptr)
	{
		return false;
	}

	//Always a good idea to declare your up-ness
	pxr::UsdGeomSetStageUpAxis(m_p->pUsdStage, UsdGeomTokens->y);

	// For physics its important to set units!
	pxr::UsdGeomSetStageMetersPerUnit(m_p->pUsdStage, 0.01);

	return true;
}

bool mipUsdStage::Open(const std::string& filepath)
{
	m_p->pUsdStage = pxr::UsdStage::Open(filepath);

	if (m_p->pUsdStage == nullptr)
	{
		return false;
	}

	return true;
}

bool mipUsdStage::IsOpen() const
{
	return m_p->pUsdStage != nullptr;
}

bool mipUsdStage::Save()
{
	if (m_p->pUsdStage == nullptr)
	{
		return false;
	}
	m_p->pUsdStage->Save();
	return true;
}

bool mipUsdStage::Close()
{
	m_p->pUsdStage.Reset();
	m_p->pUsdStage = nullptr;
	return true;
}

bool mipUsdStage::DeleteAllPrimAsType(eOmniverseUsdType type)
{
	std::vector<pxr::UsdPrim> meshList = m_p->GetPrimList(type);

	for (auto& m : meshList)
	{
		pxr::SdfPath path = m.GetPath();
		m_p->pUsdStage->RemovePrim(path);
	}

	return true;
}

bool mipUsdStage::DeletePrimByNameAsType(const std::string& name, eOmniverseUsdType type)
{
	std::vector<UsdPrim> meshList = m_p->GetPrimList(type);

	for (auto& m : meshList)
	{
		if (m.GetName() == name)
		{
			SdfPath path = m.GetPath();
			m_p->pUsdStage->RemovePrim(path);
			return true;
		}
	}

	return false;
}

bool mipUsdStage::DeleteAllMesh()
{
	return DeleteAllPrimAsType(eOmniverseUsdType::Mesh);
}

bool mipUsdStage::DeleteMeshByName(const std::string& name)
{
	return DeletePrimByNameAsType(name, eOmniverseUsdType::Mesh);
}

bool mipUsdStage::DeleteAllMaterials()
{
	return DeleteAllPrimAsType(eOmniverseUsdType::Material);
}

bool mipUsdStage::DeleteAllLights()
{
	return DeleteAllPrimAsType(eOmniverseUsdType::Light);
}

bool mipUsdStage::Export(const std::string& exportFilePath)
{
	return m_p->pUsdStage->Export(exportFilePath);
}

bool mipUsdStage::CopyUsdFile_CustomLayerRenderSetting(const std::string& filepath)
{
	mipUsdStage stage_src;
	if (stage_src.Open(filepath) == false)
	{
		qWarning() << "file to open custom layer render setting path : " << filepath.c_str();
		return false;
	}

	mipUsdCustomLayerData customLayerData_src = stage_src.GetCustomLayerData();
	mipUsdRenderSetting renderSetting_src;
	if (customLayerData_src.TryGetRenderSetting(&renderSetting_src))
	{
		qWarning() << "source render setting : " << renderSetting_src.GetRenderMode();
		mipUsdCustomLayerData customLayerData_dst = GetCustomLayerData();
		customLayerData_dst.SetRenderSetting(renderSetting_src);
		SetCustomLayerData(customLayerData_dst);
	}

	return true;
}

bool mipUsdStage::CopyUsdFile_OnlyMaterialAndLight(const std::string& filepath)
{
	pxr::UsdStageRefPtr pStageSrc = UsdStage::Open(filepath);
	if (pStageSrc == nullptr)
	{
		qWarning() << "fail to open file path : " << filepath.c_str();
		return false;
	}

	pxr::SdfLayerHandle layerSrc = pStageSrc->GetRootLayer();
	pxr::SdfLayerHandle layerDst = m_p->pUsdStage->GetRootLayer();

	UsdPrimRange primSrcRange = pStageSrc->Traverse();
	for (auto& primSrc : primSrcRange)
	{
		SdfPath srcPath = primSrc.GetPath();
		UsdPrim primDest = m_p->pUsdStage->DefinePrim(srcPath);
		SdfPath dstPath = primDest.GetPath();

		bool canCopy = primSrc.IsA<pxr::UsdShadeMaterial>() || primSrc.IsA<pxr::UsdLuxLight>();

		if (canCopy)
		{
			// /Root/Resource/
			qInfo() << "copy source : " << srcPath.GetString().c_str() << ", to dest : "
				<< dstPath.GetString().c_str()
				<< ", type : " << primSrc.GetTypeName().data();
			if (pxr::SdfCopySpec(layerSrc, srcPath, layerDst, dstPath) == false)
			{
				qWarning() << "fail to copy path : " << srcPath.GetString().c_str();
			}
		}
	}

	return true;
}

std::vector<mipUSDMeshPtr> mipUsdStage::CreateMeshList() const
{
	std::vector<mipUSDMeshPtr> list;

	std::vector<pxr::UsdGeomMesh> meshList = m_p->GetGeomMeshList();
	for (auto& usdMesh : meshList)
	{
		mipUSDMeshPtr pMipUsdMeshLayer = std::make_shared<mipUSDMesh>(usdMesh);
		pMipUsdMeshLayer->Refresh();
		list.push_back(pMipUsdMeshLayer);
	}

	return list;
}

mipUSDMeshPtr mipUsdStage::CreateMeshByName(const std::string& name) const
{
	std::vector<mipUSDMeshPtr> mipMeshList = CreateMeshList();

	for (auto& mesh : mipMeshList)
	{
		if (mesh->GetName() == name)
		{
			return mesh;
		}
	}

	return nullptr;
}

mipUSDMeshPtr mipUsdStage::CreateMeshByPath(const std::string& path) const
{
	std::vector<mipUSDMeshPtr> mipMeshList = CreateMeshList();

	for (auto& mesh : mipMeshList)
	{
		if (mesh->GetPath() == path)
		{
			return mesh;
		}
	}

	return nullptr;
}

std::vector<mipUSDMaterialPtr> mipUsdStage::CreateMaterialList() const
{
	std::vector<mipUSDMaterialPtr> mipMaterialList;

	std::vector<pxr::UsdShadeMaterial> usdShadeMaterialList = m_p->CreateMaterialList();
	for (auto& m : usdShadeMaterialList)
	{
		mipUSDMaterialPtr pMaterial = std::make_shared<mipUSDMaterial>(m);

		mipMaterialList.push_back(pMaterial);
	}

	return mipMaterialList;
}

mipUSDMaterialPtr mipUsdStage::CreateMeterialByName(const std::string& name) const
{
	std::vector<mipUSDMaterialPtr> list = CreateMaterialList();

	for (auto& p : list)
	{
		if (p->GetName() == name)
		{
			return p;
		}
	}

	return nullptr;
}

mipUSDMaterialPtr mipUsdStage::CreateMeterialByPath(const std::string& path) const
{
	std::vector<mipUSDMaterialPtr> list = CreateMaterialList();

	for (auto& p : list)
	{
		if (p->GetPath() == path)
		{
			return p;
		}
	}

	return nullptr;
}

std::vector<mipUsdLightPtr> mipUsdStage::CreateLightList() const
{
	std::vector<mipUsdLightPtr> list;

	std::vector<pxr::UsdLuxLight> usdLight = m_p->CreateLightList();
	for (auto& light : usdLight)
	{
		mipUsdLightPtr pMaterial = std::make_shared<mipUsdLight>(light);
		list.push_back(pMaterial);
	}

	return list;
}

bool mipUsdStage::AddMesh(const mip::MeshTopology& data, const MeshInfo& info, mipUSDMeshPtr* ppOutUsdMeshLayer)
{
	if (mipUsdUtil::IsCompatiblePrimName(info.MeshName) == false)
	{
		return false;
	}

	std::string newPrimName = info.GetName().toStdString();
	pxr::SdfPath rootPrimPath = pxr::SdfPath::AbsoluteRootPath().AppendChild(_tokens->Root);
	pxr::UsdGeomXform rootPrim = pxr::UsdGeomXform::Define(m_p->pUsdStage, rootPrimPath);

	// Define the defaultPrim as the /Root primSrc
	m_p->pUsdStage->SetDefaultPrim(rootPrim.GetPrim());

	pxr::SdfPath meshPrimPath = rootPrimPath.AppendChild(pxr::TfToken(newPrimName.c_str()));
	pxr::UsdGeomMesh usdMesh = pxr::UsdGeomMesh::Define(m_p->pUsdStage, meshPrimPath);

	mipUSDMeshPtr pMipUsdMeshLayer = std::make_shared<mipUSDMesh>(usdMesh);
	if (pMipUsdMeshLayer->SetDataByMeshDataAndInfo(data, info) == false)
	{
		return false;
	}
	m_p->EnablePhysics(usdMesh.GetPrim(), false);
	m_p->SetVisible(usdMesh, info.show);

	// Commit the changes to the USD
	//Save();

	if (ppOutUsdMeshLayer)
	{
		*ppOutUsdMeshLayer = pMipUsdMeshLayer;
	}

	return true;
}

bool mipUsdStage::AddMeshByStlFile(const std::string& inputFilePath, const std::string& meshLayerName, mip::Renderer* pRenderer, COLOR color)
{
	if (IsOpen() == false)
	{
		qWarning() << "stage not online";
		return false;
	}

	MeshLayerDataOmniverse meshLayerData(pRenderer);
	if (meshLayerData.LoadSTL(inputFilePath, meshLayerName) == false)
	{
		return false;
	}

	return AddMesh(*meshLayerData.GetData(), *meshLayerData.GetInfo());
}

bool mipUsdStage::TrasformMesh(const std::string& meshName, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale)
{
	pxr::UsdGeomMesh usdMesh;
	if (m_p->TryReceiveGeomMeshByName(&usdMesh, meshName) == false)
	{
		return false;
	}

	return m_p->TrasformMesh(usdMesh, v_translate_cm, v_rotate, v_scale);
}

bool mipUsdStage::TranslateMesh_cm(const std::string& meshName, mip::VECTOR3 v_translate_cm)
{
	pxr::UsdGeomMesh usdMesh;
	if (m_p->TryReceiveGeomMeshByName(&usdMesh, meshName) == false)
	{
		return false;
	}

	TransformSRT SRT;
	if (m_p->TryGetTransformSRT(&SRT, usdMesh) == false)
	{
		return false;
	}

	return m_p->TrasformMesh(usdMesh, v_translate_cm, To_mipVECTOR3(SRT.Rotate), To_mipVECTOR3(SRT.Scale));
}

bool mipUsdStage::AddMaterialByMDLFile(const std::string& filepath, const std::string& name, const std::string& subIdentifier)
{
	qInfo() << "AddMaterialByMDLFile filepath : " << filepath.c_str() << ",name : " << name.c_str() << ", id : " << subIdentifier.c_str();
	pxr::SdfPath rootPrimPath = pxr::SdfPath::AbsoluteRootPath().AppendChild(_tokens->Root);
	pxr::UsdGeomXform rootPrim = pxr::UsdGeomXform::Define(m_p->pUsdStage, rootPrimPath);

	// Define the defaultPrim as the /Root primSrc
	m_p->pUsdStage->SetDefaultPrim(rootPrim.GetPrim());

	pxr::TfToken materialNameToken(name);
	pxr::TfToken materialSubIdentifier(subIdentifier);

	pxr::SdfPath matPath = rootPrimPath.AppendChild(materialNameToken);
	pxr::UsdShadeMaterial newMat = pxr::UsdShadeMaterial::Define(m_p->pUsdStage, matPath);

	pxr::SdfAssetPath mdlShaderModule = pxr::SdfAssetPath(filepath);
	pxr::SdfPath shaderPath = matPath.AppendChild(materialNameToken);
	pxr::UsdShadeShader mdlShader = pxr::UsdShadeShader::Define(m_p->pUsdStage, shaderPath);
	mdlShader.CreateIdAttr(pxr::VtValue(_tokens->shaderId));

	mdlShader.SetSourceAsset(mdlShaderModule, _tokens->mdl);
	mdlShader.GetPrim().CreateAttribute(pxr::TfToken("info:mdl:sourceAsset:subIdentifier"), SdfValueTypeNames->Token, false, SdfVariabilityUniform).Set(materialSubIdentifier);

	UsdShadeOutput mdlOutput = newMat.CreateSurfaceOutput(_tokens->mdl);
	mdlOutput.ConnectToSource(mdlShader, _tokens->out);

	// Commit the changes to the USD
	Save();

	return true;
}

bool mipUsdStage::AddMaterial(const mipUSDMaterialPtr& material, const std::string& path)
{
	SdfPath matPath(path);
	pxr::UsdShadeMaterial newMaterial = pxr::UsdShadeMaterial::Define(m_p->pUsdStage, matPath);

	pxr::UsdPrim primDst = newMaterial.GetPrim();
	pxr::UsdPrim primSrc = material->GetRaw()->GetPrim();

	if (mipUsdUtil::CopyProperties(&primDst, primSrc) == false)
	{
		qWarning() << "fail to copy properties. path : " << matPath.GetString().c_str();
		return false;
	}

	return true;
}

bool mipUsdStage::BindMeshMaterial(const std::string& meshName, const std::string& materialName)
{
	pxr::UsdGeomMesh usdMesh;
	pxr::UsdShadeMaterial usdMaterial;

	if (m_p->TryReceiveGeomMeshByName(&usdMesh, meshName) == false)
	{
		qWarning() << "fail to load shade mesh : " << meshName.c_str();

		return false;
	}

	if (m_p->TryReceiveShadeMaterialByName(&usdMaterial, materialName) == false)
	{
		qWarning() << "fail to load shade material : " << materialName.c_str();
		return false;
	}

	pxr::UsdShadeMaterialBindingAPI usdMaterialBinding(usdMesh);
	if (usdMaterialBinding.Bind(usdMaterial) == false)
	{
		qWarning() << "fail to bind material" << materialName.c_str();
		return false;
	}

	Save();

	return true;
}

bool mipUsdStage::SetVisible(const std::string& meshName, bool value)
{
	UsdGeomMesh usdMesh;
	if (m_p->TryReceiveGeomMeshByName(&usdMesh, meshName) == false)
	{
		return false;
	}

	return m_p->SetVisible(usdMesh, value);
}

bool mipUsdStage::IsMeshExist(const std::string& meshName) const
{
	std::vector<std::string> meshNameList = GetMeshNameList();

	auto it = std::find(meshNameList.begin(), meshNameList.end(), meshName);
	return it != meshNameList.end();
}

std::vector<std::string> mipUsdStage::GetMeshNameList() const
{
	std::vector<std::string> meshNameList;
	if (IsOpen() == false)
	{
		qWarning() << "stage is not online";
		return meshNameList;
	}

	std::vector<UsdPrim> meshList = m_p->GetMeshPrimList();
	for (auto& m : meshList)
	{
		meshNameList.push_back(m.GetName());
	}

	return meshNameList;
}

mipUsdCustomLayerData mipUsdStage::GetCustomLayerData() const
{
	pxr::VtDictionary customLayerData = m_p->pUsdStage->GetRootLayer()->GetCustomLayerData();

	return mipUsdCustomLayerData(customLayerData);
}

bool mipUsdStage::SetCustomLayerData(const mipUsdCustomLayerData& data)
{
	m_p->pUsdStage->GetRootLayer()->SetCustomLayerData(data.GetDictionary());

	return true;
}

bool mipUsdStage::InsertSessionSubLayer(const mipUsdStage& stage)
{
	SdfLayerHandle stageLayer = stage.GetData()->GetRootLayer();
	UsdStageRefPtr pStage = m_p->pUsdStage;

	pStage->GetSessionLayer()->InsertSubLayerPath(stageLayer->GetIdentifier());

	return true;
}

bool mipUsdStage::ClearSessionSubLayer()
{
	UsdStageRefPtr pStage = m_p->pUsdStage;
	pStage->GetEditTarget().GetLayer()->Clear();
	pStage->GetSessionLayer()->GetSubLayerPaths().clear();
	pStage->SetEditTarget(UsdEditTarget(pStage->GetRootLayer()));

	return true;
}

bool mipUsdStage::SetEditTarget(const mipUsdStage& stage)
{
	SdfLayerHandle stageLayer = stage.GetData()->GetRootLayer();
	UsdStageRefPtr pStage = m_p->pUsdStage;

	pStage->SetEditTarget(UsdEditTarget(stageLayer));

	return true;
}

bool mipUsdStage::ClearEditTarget()
{
	UsdStageRefPtr pStage = m_p->pUsdStage;
	pStage->GetEditTarget().GetLayer()->Clear();

	return true;
}

pxr::UsdStageRefPtr mipUsdStage::GetData() const
{
	return m_p->pUsdStage;
}

