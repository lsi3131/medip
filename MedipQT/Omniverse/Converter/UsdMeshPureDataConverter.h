#pragma once

#include "Math/Math.h"
#include <array>

using MeshVertexArray4 = std::array<mip::VECTOR3, 4>;

using MeshIndexArray4 = std::array<muint32, 4>;
using MeshIndexArray6 = std::array<muint32, 6>;

using MeshNoramlArray4 = std::array<mip::VECTOR3, 4>;
using MeshNoramlArray6 = std::array<mip::VECTOR3, 6>;

namespace mip
{
	class MeshTopology;
}
class mipUSDMesh;

enum class eSupportMeshTypes
{
	MipMesh = 0x0000,
	//SubdivisionSurface = 0x0001 //현재 지원하지 않음. 추후 이슈사항 기능 구현되었을 때 추가할 예정
};

class UsdMeshPureDataConverter
{
public:
	static mip::VECTOR3 GetDiagonalPoint(const MeshVertexArray4& v_arr4, int index);
	static int GetDiagonalPointArrayIndex(const MeshVertexArray4& v_arr4, int index);
	static MeshIndexArray6 ConvertIndexArray4_To_6(const MeshVertexArray4& v_arr4, const MeshIndexArray4& idx_arr4);

	static bool ConvertUsdToMipMesh(mip::MeshTopology* pOutMipMesh, const mipUSDMesh& usdMesh);

	static bool CanConvertable(const mipUSDMesh& usdMesh, eSupportMeshTypes types);
};
