#pragma once

#ifndef C3MFLOADER_H
#define C3MFLOADER_H

#include "Mip/core.h"
#include "Renderer\Mesh.h"
#include "Renderer\MeshTopology.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <set>

#include "lib3mf_implicit.hpp"

using namespace Lib3MF;

namespace mip
{
	namespace lib3mf
	{
#define TILESTYLE_WRAP 0
#define TILESTYLE_MIRROR 1
#define TILESTYLE_CLAMP 2
#define TILESTYLE_NOTILE 3

#define PPL_USE_TRICNT 50000

		class C3MFLoader
		{
		public:
			C3MFLoader();
			virtual ~C3MFLoader();

		public:
			bool Load3mfFile(std::string sFileName, std::vector<mip::MeshTopology*>& out, float fScale = 1);
			void Save3mfFile(std::string sFileName, std::vector<mip::MeshTopology*>& meshList, std::vector<std::string>& meshNamelist, float fScale = 1);

			bool freeStbi_Data(unsigned char* data);

			void update3mfVertex(mip::MeshTopology * pMT);

			int CreateTexture(std::string sFileName, mip::MeshTopology * pMT, int tileStyle_U = TILESTYLE_CLAMP, int tileStyle_V = TILESTYLE_CLAMP);

			bool Load3mfFile_test(std::string sFileName, std::vector<mip::MeshTopology*>& out, float fScale = 1);
			void Save3mfFile_sample(std::string sFileName, mip::MeshTopology * in_pMT);
		private:
			int Combine_3mfMeshObject(mip::MeshTopology * pMT, std::vector<std::vector<sPosition>>& VerticesBuffer, std::vector<std::vector<sTriangle>>& IndicesBuffer,
				std::vector<std::vector<double>>& UVCoord, std::vector<std::vector<int>>& UVTextureId, float scale = 1.0f);
		
			int Get_GeometryDataIn3mfMeshObject(PModel model, PMeshObject pMeshObj, std::vector<Lib3MF_uint32>& listObjResId, int& InOut_meshObjCnt, int& InOut_TextureCnt,
				std::vector<std::vector<sPosition>>& VerticesBuffer, std::vector<std::vector<sTriangle>>& IndicesBuffer,
				std::vector<std::vector<double>>& UVCoord, std::vector<std::vector<int>>& UVTextureId);

			Lib3MF_uint32 GetTexture2DGroupByID(PModel model, const Lib3MF_uint32 nUniqueResourceID, PTexture2DGroup& pTex2DGroup);

			int Create3mfVertexFromTopology(mip::MeshTopology * in_pMT, std::vector<sLib3MFPosition> & vertices, float fScale = 1);
			int Create3mfTriFromTopology(mip::MeshTopology * in_pMT, std::vector<sLib3MFTriangle>& triangles);		

			bool mergingVertexFrom3mf(mip::MeshTopology * pMT, std::vector<sLib3MFPosition> & vertices, std::vector<sLib3MFTriangle>& triangles);
			bool mergingVertexFrom3mf(mip::MeshTopology * pMT, std::vector<mip::VECTOR3> & vertices, std::vector<muint32>& triangles);
		private:
			void printVersion(PWrapper wrapper);
			sLib3MFPosition fnCreateVertex(float x, float y, float z);
			sLib3MFTriangle fnCreateTriangle(int v0, int v1, int v2);
			PTexture2DGroup fnLoadModelTexture(PModel model, const std::string sOPCPath, const std::string sFilePath, eLib3MFTextureType eType, eLib3MFTextureTileStyle eTileStyleU, eLib3MFTextureTileStyle eTileStyleV);
			sLib3MFTriangleProperties fnCreateTexture(PTexture2DGroup textureGroup, double u1, double v1, double u2, double v2, double u3, double v3);
			sLib3MFTriangleProperties fnCreateTriangleColor(PColorGroup colorGroup, Lib3MF_uint32 colorID1, Lib3MF_uint32 colorID2, Lib3MF_uint32 colorID3);
			void ShowMetaDataInformation(PMetaDataGroup metaDataGroup);
			void ShowSliceStack(PSliceStack sliceStack, std::string indent);
			void ShowObjectProperties(PObject object);
			Lib3MF_uint32 C3MFLoader::ShowMeshObjectInformation(PMeshObject meshObject);
			void ShowTransform(sLib3MFTransform transform, std::string indent);
			void ShowComponentsObjectInformation(PComponentsObject componentsObject);

			

		};
	}
}
#endif