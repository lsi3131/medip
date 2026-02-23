#include "stdafx.h"
#include "UsdMeshPureDataConverter.h"
#include "Omniverse/Usd/mipUSDMesh.h"
#include "Renderer/MeshTopology.h"

static const int MIN_FACE_VERTEX_COUNT = 3;

mip::VECTOR3 UsdMeshPureDataConverter::GetDiagonalPoint(const MeshVertexArray4& v_arr4, int index)
{
	return v_arr4[GetDiagonalPointArrayIndex(v_arr4, index)];
}

int UsdMeshPureDataConverter::GetDiagonalPointArrayIndex(const MeshVertexArray4& v_arr4, int index)
{
	mip::VECTOR3 v0 = v_arr4[index];
	std::vector<int> p_otherIdxList;
	for (int i = 0; i < 4; ++i)
	{
		if (i == index)
		{
			continue;
		}
		p_otherIdxList.push_back(i);
	}

	mip::VECTOR3 v1 = v_arr4[p_otherIdxList[0]] - v0;
	mip::VECTOR3 v2 = v_arr4[p_otherIdxList[1]] - v0;
	mip::VECTOR3 v3 = v_arr4[p_otherIdxList[2]] - v0;

	if ((v1 + v2) == v3)
	{
		return p_otherIdxList[2];
	}
	else if ((v2 + v3) == v1)
	{
		return p_otherIdxList[0];
	}
	else if ((v1 + v3) == v2)
	{
		return p_otherIdxList[1];
	}
	else
	{
		qCritical() << "invalid result";
		Q_ASSERT(false);
		return -1;
	}

}

MeshIndexArray6 UsdMeshPureDataConverter::ConvertIndexArray4_To_6(const MeshVertexArray4& v_arr4, const MeshIndexArray4& idx_arr4)
{
	int arr_idx_0 = 0;
	int arr_idx_diagonal = GetDiagonalPointArrayIndex(v_arr4, arr_idx_0);
	std::vector<int> arr_idx_adjacency_list;
	for (int arr_idx = 1; arr_idx < 4; ++arr_idx)
	{
		if (arr_idx_diagonal != arr_idx)
		{
			arr_idx_adjacency_list.push_back(arr_idx);
		}
	}

	if (arr_idx_adjacency_list.size() != 2)
	{
		qCritical() << "adjacency size should = '2' ";
		Q_ASSERT(false);
	}

	return MeshIndexArray6({
		idx_arr4[0], idx_arr4[arr_idx_adjacency_list[0]], idx_arr4[arr_idx_adjacency_list[1]],
		idx_arr4[arr_idx_diagonal], idx_arr4[arr_idx_adjacency_list[0]], idx_arr4[arr_idx_adjacency_list[1]]
		});
}

bool UsdMeshPureDataConverter::ConvertUsdToMipMesh(mip::MeshTopology* pOutMipMesh, const mipUSDMesh& usdMesh)
{
	return usdMesh.CopyToMeshTopology(pOutMipMesh);
}

bool UsdMeshPureDataConverter::CanConvertable(const mipUSDMesh& usdMesh, eSupportMeshTypes types)
{
	std::vector<muint32> faceVertexCounts = usdMesh.GetFaceVertexCounts();

	auto it = std::find_if(faceVertexCounts.begin(), faceVertexCounts.end(), [](muint32 lhs) {
		return lhs > MIN_FACE_VERTEX_COUNT;
		});

	bool hasBiggerThanMinFaceVertexCount = (it != faceVertexCounts.end());

	if (hasBiggerThanMinFaceVertexCount)
	{
		return false;
	}

	return true;
}

