#include "stdafx.h"
#include "mipUSDMesh.h"
#include "Omniverse/OmniverseUtils.h"
#include "Omniverse/xformUtils.h"
#include "Renderer/MeshTopology.h"

#define STL_LABEL_SIZE_DEFINITIONS  (80)

#define PXR_VISIBLE_TEXT ("inherited")
#define PXR_INVISIBLE_TEXT ("invisible")

mipUSDMesh::mipUSDMesh(pxr::UsdGeomMesh raw) :
	m_isVisible(false),
	m_status(eOmniverseStatus::Unlock),
	m_raw(raw)
{
	translation_mm = pxr::GfVec3d(0, 0, 0);
	rotation = pxr::GfVec3d(0, 0, 0);
	scale = pxr::GfVec3d(1.0, 1.0, 1.0);
	rotationOrder = pxr::GfVec3i(0, 1, 2);
}

mipUSDMesh::~mipUSDMesh()
{
}

void mipUSDMesh::Refresh()
{
	bool result = false;

	result = m_raw.GetPointsAttr().Get<VtArray<GfVec3f>>(&points_mm);
	result = m_raw.GetNormalsAttr().Get<VtArray<GfVec3f>>(&meshNormals);
	result = m_raw.GetFaceVertexIndicesAttr().Get<VtArray<int>>(&vecIndices);
	result = m_raw.GetFaceVertexCountsAttr().Get<VtArray<int>>(&faceVertexCounts);

	UsdAttribute displayColorAttr = m_raw.CreateDisplayColorAttr();
	{
		VtVec3fArray valueArray;
		if (displayColorAttr.Get(&valueArray))
		{
			if (!valueArray.empty())
			{
				GfVec3f rgbFace = valueArray[0];
				color = rgbFace;
			}
		}
	}

	TfToken visibilityToken;
	result = m_raw.GetVisibilityAttr().Get<TfToken>(&visibilityToken);
	std::string visibilityText = visibilityToken.GetString();
	if (visibilityText == "inherited")
	{
		SetVisible(true);
	}
	else if (visibilityText == "invisible")
	{
		SetVisible(false);
	}

	xformUtils::getLocalTransformSRT(
		m_raw.GetPrim(),
		translation_mm,
		rotation,
		rotationOrder,
		scale
	);

	QString translationText = QString("%1,%2,%3").arg(translation_mm[0]).arg(translation_mm[1]).arg(translation_mm[2]);
	QString rotationText = QString("%1,%2,%3").arg(rotation[0]).arg(rotation[1]).arg(rotation[2]);

	qInfo() << "refresh mesh : " << m_raw.GetPrim().GetName().data() << ", translation : " << translationText << ", rotation : " << rotationText << "visible : " << IsVisible();
}

bool mipUSDMesh::SetDataByMeshDataAndInfo(const mip::MeshTopology& data, const MeshInfo& info)
{
	QElapsedTimer timer;
	timer.restart();

	meshNormals.clear();
	points_mm.clear();
	vecIndices.clear();

	meshNormals.reserve((data.m_normals.size() / 3) + 1);
	points_mm.reserve(data.m_verts.size());
	vecIndices.reserve(data.m_tris.size());
	faceVertexCounts.reserve(meshNormals.size());

	for (int i = 0; i < data.m_normals.size(); i += 3)
	{
		GfVec3f v = To_GfVec3f(data.m_normals[i]);
		meshNormals.push_back(v);
	}

	for (int i = 0; i < data.m_tris.size(); i++)
	{
		int index = data.m_tris[i];
		vecIndices.push_back(index);
	}

	for (int i = 0; i < data.m_verts.size(); i++)
	{
		GfVec3f v_mm = To_GfVec3f(data.m_verts[i] * 10.0f);
		points_mm.push_back(v_mm);
	}

	for (int i = 0; i < meshNormals.size(); i++)
	{
		faceVertexCounts.push_back(3);
	}

	translation_mm = To_GfVec3d(data.translation) * 10.0f;
	rotation = GfVec3d(data.rotation.x, data.rotation.y, data.rotation.z);
	scale = GfVec3d(data.scale.x, data.scale.y, data.scale.z);

	qInfo() << "USD translate: " << QString("%1,%2,%3").arg(translation_mm[0]).arg(translation_mm[1]).arg(translation_mm[2]) <<
		", rotate" << QString("%1,%2,%3").arg(rotation[0]).arg(rotation[1]).arg(rotation[2]) <<
		", scale" << QString("%1,%2,%3").arg(scale[0]).arg(scale[1]).arg(scale[2]) <<
		"| Orign translate:" << QString("%1,%2,%3").arg(data.translation.x).arg(data.translation.y).arg(data.translation.z) <<
		", rotate" << QString("%1,%2,%3").arg(data.rotation.x).arg(data.rotation.y).arg(data.rotation.z) <<
		", scale" << QString("%1,%2,%3").arg(data.scale.x).arg(data.scale.y).arg(data.scale.z);



	/* ========== Update raw data ==============*/

	// Set orientation
	m_raw.CreateOrientationAttr(pxr::VtValue(UsdGeomTokens->rightHanded));

	// Add all of the vertices
	m_raw.CreatePointsAttr(pxr::VtValue(points_mm));

	// Calculate indices for each triangle
	m_raw.CreateFaceVertexIndicesAttr(pxr::VtValue(vecIndices));

	// Add vertex normals    
	m_raw.CreateNormalsAttr(pxr::VtValue(meshNormals));

	// Add face vertex count
	m_raw.CreateFaceVertexCountsAttr(pxr::VtValue(faceVertexCounts));

	xformUtils::setLocalTransformSRT(
		m_raw.GetPrim(),
		translation_mm,
		rotation,
		rotationOrder,
		scale
	);

	// set is as a static triangle mesh

	pxr::UsdAttribute displayColorAttr = m_raw.CreateDisplayColorAttr();
	{
		pxr::VtVec3fArray valueArray;
		pxr::GfVec3f rgbFace = pxr::GfVec3f((float)info.color.r / 255.0f, (float)info.color.g / 255.0f, (float)info.color.b / 255.0f);
		valueArray.push_back(rgbFace);
		displayColorAttr.Set(valueArray);
		color = rgbFace;
	}

	m_isVisible = info.show;

	qInfo() << "elapsed time : " << timer.elapsed();

	return true;
}

bool mipUSDMesh::SetDataByMeshLayer(const MeshLayerData& layer)
{
	SetDataByMeshDataAndInfo(*layer.Data, *layer.Info);

	return true;
}

bool mipUSDMesh::CopyToMeshTopology(mip::MeshTopology* pOut) const
{
	pOut->m_verts.clear();
	pOut->m_tris.clear();
	pOut->m_normals.clear();

	//pOut->m_verts.reserve(points_mm.size());
	//pOut->m_tris.reserve(vecIndices.size());
	//pOut->m_normals.reserve(meshNormals.size() * 3);

	for (int i = 0; i < meshNormals.size(); i++)
	{
		pOut->m_normals.push_back(To_mipVECTOR3(meshNormals[i]));
		pOut->m_normals.push_back(To_mipVECTOR3(meshNormals[i]));
		pOut->m_normals.push_back(To_mipVECTOR3(meshNormals[i]));
	}

	for (int i = 0; i < vecIndices.size(); i++)
	{
		int index = vecIndices[i];
		pOut->m_tris.push_back(index);
	}

	for (int i = 0; i < points_mm.size(); i++)
	{
		mip::VECTOR3 v_cm = To_mipVECTOR3(points_mm[i] * 0.1f);
		pOut->m_verts.push_back(v_cm);
	}

	CopyToSRT(pOut);

	return true;
}

bool mipUSDMesh::CopyToMeshLayerData(MeshLayerData* pOut) const
{
	QElapsedTimer timer;
	timer.restart();
	CopyToMeshTopology(pOut->Data);
	CopyToMeshInfo(pOut->Info);

	qInfo() << "CopyToMeshLayerData. elapsed time : " << timer.elapsed() <<
		", points_mm size : " << pOut->Data->m_verts.size() <<
		", vecIndices size : " << pOut->Data->m_tris.size() <<
		", normals size : " << pOut->Data->m_normals.size() <<
		", visible : " << pOut->Info->show;

	return true;
}

bool mipUSDMesh::CopyToMeshInfo(MeshInfo* pOut) const
{
	pOut->show = m_isVisible;
	pOut->color = To_mipCOLOR(color);
	return true;
}

bool mipUSDMesh::CopyToSRT(mip::MeshTopology* pOut) const
{
	pOut->setTranslate(To_mipVECTOR3(translation_mm * 0.1f));
	pOut->setRotate(To_mipQUATERNION(rotation));
	pOut->setScale(To_mipVECTOR3(scale));

	return true;
}

std::string mipUSDMesh::GetName() const
{
	return m_raw.GetPrim().GetName();
}

std::string mipUSDMesh::GetPath() const
{
	return m_raw.GetPrim().GetPath().GetString();
}

void mipUSDMesh::SetStatus(eOmniverseStatus status)
{
	this->m_status = status;
}

eOmniverseStatus mipUSDMesh::GetStatus() const
{
	return this->m_status;
}

mip::VECTOR3 mipUSDMesh::GetTranslation_mm() const
{
	return To_mipVECTOR3(translation_mm);
}

mip::VECTOR3 mipUSDMesh::GetTranslation_cm() const
{
	return GetTranslation_mm() * 0.1f;
}

std::vector<mip::VECTOR3> mipUSDMesh::GetVertice_mm() const
{
	std::vector<mip::VECTOR3> vertice_mm;
	for (auto pt : points_mm)
	{
		vertice_mm.push_back(To_mipVECTOR3(pt));
	}
	return vertice_mm;
}

std::vector<mip::VECTOR3> mipUSDMesh::GetVertice_cm() const
{
	std::vector<mip::VECTOR3> vertice_cm;
	for (auto pt : points_mm)
	{
		vertice_cm.push_back(To_mipVECTOR3(pt) * 0.1f);
	}
	return vertice_cm;
}

std::vector<mip::VECTOR3> mipUSDMesh::GetNormals() const
{
	std::vector<mip::VECTOR3> list;
	for (auto& v : meshNormals)
	{
		list.push_back(To_mipVECTOR3(v));
	}
	return list;
}

std::vector<muint32> mipUSDMesh::GetIndices() const
{
	std::vector<muint32> list;
	for (auto& v : vecIndices)
	{
		list.push_back(v);
	}
	return list;
}

std::vector<muint32> mipUSDMesh::GetFaceVertexCounts() const
{
	std::vector<muint32> list;
	for (auto& v : faceVertexCounts)
	{
		list.push_back(v);
	}
	return list;
}

mip::QUATERNION mipUSDMesh::GetRotation() const
{
	mip::VECTOR3 v = To_mipVECTOR3(rotation);
	return mip::QUATERNION(v[0], v[1], v[2], 1.0f);
}

mip::VECTOR3 mipUSDMesh::GetScale() const
{
	return To_mipVECTOR3(scale);
}

COLOR mipUSDMesh::GetColor() const
{
	return To_mipCOLOR(color);
}

void mipUSDMesh::SetVisible(bool value)
{
	m_isVisible = value;
}

bool mipUSDMesh::IsVisible() const
{
	return m_isVisible;
}

bool mipUSDMesh::BindMaterial(mipUSDMaterialPtr pMaterial)
{
	pxr::UsdShadeMaterialBindingAPI usdMaterialBinding(m_raw);
	if (usdMaterialBinding.Bind(*pMaterial->GetRaw()) == false)
	{
		qWarning() << "fail to bind material";
		return false;
	}

	return true;
}

bool mipUSDMesh::UnbindMaterial()
{
	pxr::UsdShadeMaterialBindingAPI usdMaterialBinding(m_raw);

	usdMaterialBinding.UnbindDirectBinding();

	return true;
}

mipUSDMaterialPtr mipUSDMesh::GetMaterial() const
{
	if (m_raw.GetPrim().IsValid() == false)
	{
		return nullptr;
	}
	UsdStageWeakPtr pStage = m_raw.GetPrim().GetStage();

	pxr::UsdShadeMaterialBindingAPI usdMaterialBinding(m_raw);
	pxr::UsdRelationship materialRelationShip = usdMaterialBinding.GetDirectBindingRel();
	if (materialRelationShip.IsValid() == false)
	{
		return nullptr;
	}
	pxr::SdfPathVector targets;
	if (materialRelationShip.GetTargets(&targets) == false)
	{
		return nullptr;
	}

	if (targets.empty())
	{
		return nullptr;
	}
	pxr::UsdShadeMaterial connectedMaterial = pxr::UsdShadeMaterial::Get(pStage, targets[0]);
	if (connectedMaterial.GetPrim().IsValid() == false)
	{
		return nullptr;
	}

	mipUSDMaterialPtr pMipMaterial = std::make_shared<mipUSDMaterial>(connectedMaterial);
	return pMipMaterial;
}

pxr::UsdGeomMesh mipUSDMesh::GetData() const
{
	return m_raw;
}

bool mipUSDMesh::operator==(const mipUSDMesh& rhs) const
{
	return
		this->points_mm == rhs.points_mm &&
		this->meshNormals == rhs.meshNormals &&
		this->vecIndices == rhs.vecIndices &&
		this->faceVertexCounts == rhs.faceVertexCounts &&
		this->translation_mm == rhs.translation_mm &&
		this->rotation == rhs.rotation &&
		this->scale == rhs.scale &&
		this->rotationOrder == rhs.rotationOrder;
}

bool mipUSDMesh::operator!=(const mipUSDMesh& rhs) const
{
	return !(*this == rhs);
}

