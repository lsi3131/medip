#pragma once
#include "meshlib.h"

namespace mip
{
	namespace mesh
	{
		int beginBrush(std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals);
		void endBrush(int BrushId, std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals);
		void clearBrush(int BrushId);

		// ring_count : 1 ~ 15
		int selectionSmooth(int BrushId, const VECTOR2 & screen, const VECTOR2 & screen_size, const MATRIX44 & wvp,
			const int radius_size, bool & _allupdate, float weight_factor = 0.1f, bool feature_clean = false );

		int selectionPull(int BrushId, const VECTOR2 & screen, const VECTOR2 & screen_size, const MATRIX44 & wvp,
			const int radius_size, bool & _allupdate, float weight_factor = 0.01f, float subdivide_length = 0.5f );

		int selectionMoveStart(int BrushId, const VECTOR2 & screen, const VECTOR2 & screen_size, const MATRIX44 & wvp,
			const int radius_size, bool & _allupdate, float subdivide_length = 0.5f );
		int selectionMoveUpdate(int BrushId, const VECTOR3 & start, const VECTOR3 & end, const mip::MATRIX44 & world);
		void selectionMoveEnd(int BrushId );

		void * getVertexBuffer(int BrushId);
		int getMeshDatas(int brush_id, std::vector<VECTOR3> & verts, std::vector<muint32> & tris, std::vector<VECTOR3> & normals);

		bool updateVertexBuffer(unsigned int VAO, unsigned int pointBO, void * buffer_data, unsigned int size);
	};
};