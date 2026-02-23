#pragma once

#include "graphics/color.h"

namespace mip
{
	class Renderer;
	class MeshTopology;
}

enum class eMeshPrimitiveType
{
	Cube,
	Cylinder,
	Sphere,
};

class MeshPrimitiveBuilder
{
public:
	/*
		반환하는 mip::MeshTopology는 new로 생성된 객체
		클라이언트에서 delete 처리를 해야한다.
	*/
	mip::MeshTopology* CreateCube(mip::Renderer* pRenderer, mip::VECTOR3 size_mm);
	mip::MeshTopology* CreateCylinder(mip::Renderer* pRenderer, mip::VECTOR3 size_mm, int triangleCount);
	mip::MeshTopology* CreateSphere(mip::Renderer* pRenderer, mip::VECTOR3 size_mm, int resolutionTheta, int resolutionPhi);

	bool BuildCube(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm);
	bool BuildCube_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color);

	bool BuildCylinder(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, int triangleCount);
	bool BuildCylinder_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color, int triangleCount);

	bool BuildSphere(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, int resolutionTheta, int resolutionPhi);
	bool BuildSphere_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, mip::VECTOR3 size_mm, COLOR color, int resolutionTheta, int resolutionPhi);

	void UpdateTopology_And_Subdivision(mip::Renderer* pRenderer, mip::MeshTopology* pMesh, COLOR color);
	void UpdateTopology(mip::Renderer* pRenderer, mip::MeshTopology* pMesh);
};
