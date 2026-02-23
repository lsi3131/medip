/*
@company		메디컬아이피
@autor			허 건 과장
@date			2021-11-26
@brief			MarchingCube 헤더파일
*/

#pragma once

namespace mip
{
	/*
	@brief
	*/
	class		MarchingCube
	{
	public:
		MarchingCube();
		~MarchingCube();

		mip::VECTOR3 MCSlicingEdge(const float* ZAXIS_element, const float& DepthOfSlice, int& element);

		int getMCTableValue(int _index1, int _index2);
	};

};
