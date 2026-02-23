#include "stdafx.h"
#include "MeshPrimitiveBuilder.h"
#include "Renderer/MeshTopology.h"
#include "MeshControl.h"
#include "graphics/Mesh/MeshTopologyBuilder.h"

mip::MeshTopology* MeshPrimitiveBuilder::CreateCube(mip::Renderer* pRenderer, mip::VECTOR3 size_mm)
{
	mip::MeshTopology* pMesh = new mip::MeshTopology(pRenderer);
	if (BuildCube(pRenderer, pMesh, size_mm) == false)
	{
		SAFE_DELETE(pMesh);
	}
	return pMesh;
}

mip::MeshTopology* MeshPrimitiveBuilder::CreateCylinder(mip::Renderer* pRenderer, mip::VECTOR3 size_mm, int triangleCount)
{
	mip::MeshTopology* pMesh = new mip::MeshTopology(pRenderer);
	if (BuildCylinder(pRenderer, pMesh, size_mm, triangleCount) == false)
	{
		SAFE_DELETE(pMesh);
	}
	return pMesh;
}

mip::MeshTopology* MeshPrimitiveBuilder::CreateSphere(mip::Renderer* pRenderer, mip::VECTOR3 size_mm, int resolutionTheta, int resolutionPhi)
{
	mip::MeshTopology* pMesh = new mip::MeshTopology(pRenderer);
	if (BuildSphere(pRenderer, pMesh, size_mm, resolutionTheta, resolutionPhi) == false)
	{
		SAFE_DELETE(pMesh);
	}
	return pMesh;
}

bool MeshPrimitiveBuilder::BuildCube(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm)
{
	int result = mip::mesh_control::createCube(size_mm, pMesh->m_verts, pMesh->m_tris, pMesh->m_normals);

	if (result == 0)
	{
		return false;
	}

	return true;
}

bool MeshPrimitiveBuilder::BuildCube_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color)
{
	if (BuildCube(pRenderer, pMesh, size_mm) == false)
	{
		return false;
	}

	UpdateTopology_And_Subdivision(pRenderer, pMesh, color);

	return true;
}

bool MeshPrimitiveBuilder::BuildCylinder(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, int triangleCount)
{
	int result = mip::mesh_control::createCylinder(size_mm, triangleCount, pMesh->m_verts, pMesh->m_tris, pMesh->m_normals);
	if (result == 0)
	{
		return false;
	}

	return true;
}

bool MeshPrimitiveBuilder::BuildCylinder_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color, int triangleCount)
{
	if (BuildCylinder(pRenderer, pMesh, size_mm, triangleCount) == false)
	{
		return false;
	}

	UpdateTopology_And_Subdivision(pRenderer, pMesh, color);

	return true;
}


bool MeshPrimitiveBuilder::BuildSphere(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, int resolutionTheta, int resolutionPhi)
{
	int result = mip::mesh_control::createSphere(size_mm, resolutionTheta, resolutionPhi, pMesh->m_verts, pMesh->m_tris, pMesh->m_normals);
	if (result == 0)
	{
		return false;
	}

	return true;
}

bool MeshPrimitiveBuilder::BuildSphere_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color, int resolutionTheta, int resolutionPhi)
{
	if (BuildSphere(pRenderer, pMesh, size_mm, resolutionTheta, resolutionPhi) == false)
	{
		return false;
	}

	UpdateTopology_And_Subdivision(pRenderer, pMesh, color);

	return true;
}

void MeshPrimitiveBuilder::UpdateTopology_And_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, COLOR color)
{
	MeshTopologyBuilder meshTopologyBuilder;
	pMesh->mergingVertex();
	pMesh->buildTopologyHEdge();
	pMesh->buildTree();
	pMesh->updateVertex();

	mip::VECTOR4 v_color(mip::VECTOR4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));

	pMesh->updateColor(v_color);
	pMesh->enableAlpha(true);
	meshTopologyBuilder.BuildRenderBufferTopology(pRenderer, pMesh);

	mip::mesh_control::SubDivision(pMesh);

	pMesh->buildTree();
	pMesh->updateVertex();

	pMesh->setTopologyed(true);
}

void MeshPrimitiveBuilder::UpdateTopology(mip::Renderer* pRenderer, mip::MeshTopology* pMesh)
{
	MeshTopologyBuilder meshTopologyBuilder;
	pMesh->mergingVertex();
	pMesh->buildTopologyHEdge();
	pMesh->buildTree();
	pMesh->updateVertex();

	pMesh->setTopologyed(true);
}

