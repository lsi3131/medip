#include "stdafx.h"
#include "ActionMesh.h"

#include "MedipQT.h"

#include "MeshControl.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainMeshWidget.h"
#include "Windows/MEVolumeView.h"
#include "Windows/Tabwindow.h"
#include "Renderer/MeshTopology.h"
#include "graphics/volumedata.h"
#include "System/stringManager.h"

#include "MeshEdit/CMeshCutManager.h"
#include "MeshEdit/CMeshViewRenderManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshHoleFillManager.h"

#include <ppl.h>
#include <thread>

#include <vtkImageGridSource.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageCast.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   Mesh Undo/Redo File  ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUndoRedo::CUndoRedo() :
	CUndoRedo(
		WIN_MANAGER,
		MESH_WORK_MANAGER)
{
}

CUndoRedo::CUndoRedo(
	WindowManager* pWinManager,
	CMeshWorkManager* pMeshWorkManager) :
	m_pWinManager(pWinManager),
	m_pMeshWorkManager(pMeshWorkManager)
{
	m_pMeshManipulator = m_pMeshWorkManager->GetMeshManipulator();
	m_pMeshHoleFillManager = m_pMeshWorkManager->GetMeshHoleFillManager();
	m_pMeshDlgManager = m_pMeshWorkManager->GetMeshDlgManager();
	m_pMeshViewRenderManager = m_pMeshWorkManager->GetRenderer();
	m_pMeshCutManager = m_pMeshWorkManager->GetMeshCutManager();
}

CUndoRedo::~CUndoRedo()
{
}

void CUndoRedo::saveDataAllRedoFile(QString filePath, mip::MeshTopology* pMesh, MESH_WORK_MODE _work_mode)
{
	if (pMesh)
	{
		int coreNumber = std::thread::hardware_concurrency();

		if (coreNumber >= 4)
		{
			concurrency::parallel_invoke(
				[this, filePath, pMesh]
				{
					saveTVertexRedoFile(filePath, pMesh);
				},
					[this, filePath, pMesh]
				{
					saveTTrisRedoFile(filePath, pMesh);
				},
					[this, filePath, pMesh]
				{
					saveTHedgeRedoFile(filePath, pMesh);
				},
					[this, filePath, pMesh]
				{
					saveTBrushDataFile(filePath, pMesh, true); //20210608_byPHS_BrushData추가  
				},
					[this, filePath, pMesh]
				{
					saveCoreTextureRedoFile(filePath, pMesh); //20210614_byPHS_3mf_Un/Redo추가
				}
				);
		}
		else
		{
			saveCoreTextureRedoFile(filePath, pMesh);
			saveTVertexRedoFile(filePath, pMesh, true);
			saveTTrisRedoFile(filePath, pMesh, true);
			saveTHedgeRedoFile(filePath, pMesh, true);
			saveTBrushDataFile(filePath, pMesh, true);
		}
	}
}


void CUndoRedo::saveCoreVertexRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_vertex";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		mip::AABB box;
		mip::mesh_control::getMinMax(pMesh->m_verts, box.min, box.max);

		pMesh->m_boundingBox = box;

		file.write((char*)(&pMesh->m_boundingBox.max), sizeof(mip::VECTOR3));
		file.write((char*)(&pMesh->m_boundingBox.min), sizeof(mip::VECTOR3));

		int  n_verts = pMesh->m_verts.size();

		file.write((char*)(&n_verts), sizeof(int));
		file.write((char*)pMesh->m_verts.data(), sizeof(mip::VECTOR3) * n_verts);

		file.close();
	}
}

void CUndoRedo::saveCoreTrisRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_tris";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_tris = pMesh->m_tris.size();

		file.write((char*)(&n_tris), sizeof(int));
		file.write((char*)pMesh->m_tris.data(), sizeof(muint32) * n_tris);

		file.close();
	}
}

void CUndoRedo::saveCoreNormalRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_normal";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_normals = pMesh->m_normals.size();

		file.write((char*)(&n_normals), sizeof(int));
		file.write((char*)pMesh->m_normals.data(), sizeof(mip::VECTOR3) * n_normals);

		file.close();
	}
}

void CUndoRedo::saveCoreColorRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_color";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_colors = pMesh->m_vertColors.size();

		file.write((char*)(&n_colors), sizeof(int));
		file.write((char*)pMesh->m_vertColors.data(), sizeof(mip::VECTOR4) * n_colors);

		file.write((char*)&pMesh->m_baseColor, sizeof(mip::VECTOR4));

		file.close();
	}

	//if (_b_single_mode)
	//{
	// QString path = filePath + "_color";

	// QFile file(path);

	// if (file.open(QIODevice::WriteOnly))
	// {
	//  int  n_colors = pMesh->m_vertColors.size();

	//  file.write((char*)(&n_colors), sizeof(int));
	//  file.write((char*)pMesh->m_vertColors.data(), sizeof(mip::VECTOR4)*n_colors);

	//  file.close();
	// }
	//}
	//else
	//{
	// QString path = filePath + "_color";

	// QFile file(path);

	// if (file.open(QIODevice::WriteOnly))
	// {
	//  int  n_colors = pMesh->m_vertColors.size();

	//  file.write((char*)(&n_colors), sizeof(int));
	//  file.close();
	// }

	// const auto  num_core = std::thread::hardware_concurrency();
	// int  n_core = (int)num_core;
	// int  n_tverts = pMesh->m_vertColors.size();
	// int  interval = n_tverts / n_core;
	// int  remain = n_tverts % n_core;

	// concurrency::parallel_for(0, n_core, [&](int i)
	// {
	//  int start = interval * i;
	//  int finish = start + interval;

	//  if (i == num_core - 1)
	//  {
	// finish += remain;
	//  }

	//  QString path = filePath + QString().sprintf("_color_%d", i);

	//  saveCoreColorRedoFile(path, pMesh->m_vertColors, start, finish);
	// });
	//}
}

void CUndoRedo::saveCoreColorRedoFile(QString filePath, std::vector<mip::VECTOR4>& _vt_color, int _start, int _end)
{
	QFile file(filePath);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_colors = _end - _start;

		auto p_color = &_vt_color[_start];

		file.write((char*)(&n_colors), sizeof(int));
		file.write((char*)p_color, sizeof(mip::VECTOR4) * n_colors);

		file.close();
	}
}

void CUndoRedo::saveCoreTextureRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_texture";

	QFile file(path);

	if (pMesh->m_uv.size() < 1 || pMesh->m_textureID.size() < 1 || pMesh->m_TextureDataList.size() < 1) return;

	if (file.open(QIODevice::WriteOnly))
	{
		//UV
		int  n_UVs = pMesh->m_uv.size();
		file.write((char*)(&n_UVs), sizeof(int));
		file.write((char*)pMesh->m_uv.data(), sizeof(mip::VECTOR2) * n_UVs);

		//TextureID
		int  n_TextureIDs = pMesh->m_textureID.size();
		file.write((char*)(&n_TextureIDs), sizeof(int));
		file.write((char*)pMesh->m_textureID.data(), sizeof(muint32) * n_TextureIDs);

		//Texture Data
		int  n_TextureDataCnt = pMesh->m_TextureDataList.size();

		file.write((char*)(&n_TextureDataCnt), sizeof(int));
		for (int ii = 0; ii < n_TextureDataCnt; ii++)
		{
			mip::TEXTURE& textureData = pMesh->m_TextureDataList[ii];

			std::string strName(textureData.name.begin(), textureData.name.end());
			int nameLength = strName.length();
			file.write((char*)(&nameLength), sizeof(int));
			file.write((char*)(strName.c_str()), sizeof(char) * (nameLength));

			int width = textureData.getWidth();
			int height = textureData.getHeight();
			int Channels = textureData.getChannels();
			int tileStyle = textureData.getTileStyle();
			int dataSize = width * height * Channels;

			file.write((char*)(&width), sizeof(int));
			file.write((char*)(&height), sizeof(int));
			file.write((char*)(&Channels), sizeof(int));
			file.write((char*)(&tileStyle), sizeof(int));
			file.write((char*)(&dataSize), sizeof(int));

#if 1
			unsigned char* pTxtData = pMesh->m_TextureDataList[ii].getTextureData();
			std::vector<unsigned char> vector(pTxtData, pTxtData + dataSize);
			file.write((char*)&(vector[0]), sizeof(unsigned char) * dataSize);
#endif
		}

		file.close();
	}
}

void CUndoRedo::saveTVertexRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_tvertex";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		auto color = pMesh->m_baseColor;

		file.write((char*)(&color), sizeof(mip::VECTOR4));

		int   n_tverts = pMesh->m_tverts.size();

		file.write((char*)(&n_tverts), sizeof(int));

		if (_b_single_mode)
		{
			//file.write((char*)pMesh->m_tverts.data(), sizeof(mip::TVert) * n_tverts);

			int n_tis, n_heis, flag;
			for (int i = 0; i < n_tverts; ++i)
			{
				flag = pMesh->m_tverts[i].getFlag();
				file.write((char*)(&flag), sizeof(int));

				file.write((char*)(&pMesh->m_tverts[i].vi), sizeof(int));

				file.write((char*)(&pMesh->m_tverts[i].pos), sizeof(mip::VECTOR3));

				n_tis = pMesh->m_tverts[i].tis.size();
				file.write((char*)(&n_tis), sizeof(int));
				file.write((char*)pMesh->m_tverts[i].tis.data(), sizeof(int) * n_tis);

				n_heis = pMesh->m_tverts[i].heis.size();
				file.write((char*)(&n_heis), sizeof(int));
				file.write((char*)pMesh->m_tverts[i].heis.data(), sizeof(int) * n_heis);

				file.write((char*)(&pMesh->m_tverts[i].weight), sizeof(float));

				file.write((char*)(&pMesh->m_tverts[i].ori_pos), sizeof(mip::VECTOR3));
				file.write((char*)(&pMesh->m_tverts[i].ori_nor), sizeof(mip::VECTOR3));
			}
		}

		bool b_topology = pMesh->isTopologyed();
		file.write((char*)(&b_topology), sizeof(bool));

		file.write((char*)(&pMesh->rotation), sizeof(mip::QUATERNION));
		file.write((char*)(&pMesh->translation), sizeof(mip::VECTOR3));
		file.write((char*)(&pMesh->scale), sizeof(mip::VECTOR3));

		file.close();

		// Save parallel method
		if (!_b_single_mode)
		{
			const auto  num_core = std::thread::hardware_concurrency();
			int  n_core = (int)num_core;
			int  n_tverts = pMesh->m_tverts.size();
			int  interval = n_tverts / n_core;
			int  remain = n_tverts % n_core;

			//for (auto i = 0; i < num_core; ++i)
			concurrency::parallel_for(0, n_core, [&](int i)
				{
					int start = interval * i;
					int finish = start + interval;

					if (i == num_core - 1)
					{
						finish += remain;
					}

					QString path = filePath + QString().sprintf("_tvertex_%d", i);

					saveTVertexRedoFile(path, pMesh->m_tverts, start, finish);
				});
		}
	}
}

void CUndoRedo::saveTVertexRedoFile(
	QString filePath,
	std::vector<mip::TVert>& _vt_verts,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::WriteOnly))
	{
		int   n_tverts = _end - _start;

		//file.write((char*)(&n_tverts), sizeof(int));
		//file.write((char*)pMesh->m_tverts.data(), sizeof(mip::TVert) * n_tverts);

		int n_tis, n_heis, flag;
		for (int i = _start; i < _end; ++i)
		{
			flag = _vt_verts[i].getFlag();
			file.write((char*)(&flag), sizeof(int));

			file.write((char*)(&_vt_verts[i].vi), sizeof(int));

			file.write((char*)(&_vt_verts[i].pos), sizeof(mip::VECTOR3));

			n_tis = _vt_verts[i].tis.size();
			file.write((char*)(&n_tis), sizeof(int));
			file.write((char*)_vt_verts[i].tis.data(), sizeof(int) * n_tis);

			n_heis = _vt_verts[i].heis.size();
			file.write((char*)(&n_heis), sizeof(int));
			file.write((char*)_vt_verts[i].heis.data(), sizeof(int) * n_heis);

			file.write((char*)(&_vt_verts[i].weight), sizeof(float));

			file.write((char*)(&_vt_verts[i].ori_pos), sizeof(mip::VECTOR3));
			file.write((char*)(&_vt_verts[i].ori_nor), sizeof(mip::VECTOR3));
		}

		file.close();
	}
}

void CUndoRedo::saveTTrisRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	//QString path = filePath + "_ttris";

	//QFile file(path);

	//if (file.open(QIODevice::WriteOnly))
	//{
	// int  n_ttris = pMesh->m_ttris.size();

	// file.write((char*)(&n_ttris), sizeof(int));
	// //file.write((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);

	// int flag;
	// for (int i = 0; i < n_ttris; ++i)
	// {
	//  flag = pMesh->m_ttris[i].getFlag();

	//  file.write((char*)(&flag), sizeof(int));

	//  file.write((char*)pMesh->m_ttris[i].vi, sizeof(int) * 3);
	//  file.write((char*)(&pMesh->m_ttris[i].hei), sizeof(int));
	// }

	// file.close();
	//}

	if (_b_single_mode)
	{
		QString path = filePath + "_ttris";

		QFile file(path);

		if (file.open(QIODevice::WriteOnly))
		{
			int  n_ttris = pMesh->m_ttris.size();

			file.write((char*)(&n_ttris), sizeof(int));
			//file.write((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);

			int flag;
			for (int i = 0; i < n_ttris; ++i)
			{
				flag = pMesh->m_ttris[i].getFlag();

				file.write((char*)(&flag), sizeof(int));

				file.write((char*)pMesh->m_ttris[i].vi, sizeof(int) * 3);
				file.write((char*)(&pMesh->m_ttris[i].hei), sizeof(int));
			}

			file.close();
		}
	}
	else
	{
		QString path = filePath + "_ttris";

		QFile file(path);

		if (file.open(QIODevice::WriteOnly))
		{
			int  n_ttris = pMesh->m_ttris.size();

			file.write((char*)(&n_ttris), sizeof(int));
			//file.write((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);
			file.close();
		}

		// Save parallel method
		const auto  num_core = std::thread::hardware_concurrency();
		int  n_core = (int)num_core;
		int  n_ttris = pMesh->m_ttris.size();
		int  interval = n_ttris / n_core;
		int  remain = n_ttris % n_core;

		//for (auto i = 0; i < num_core; ++i)
		concurrency::parallel_for(0, n_core, [&](int i)
			{
				int start = interval * i;
				int finish = start + interval;

				if (i == num_core - 1)
				{
					finish += remain;
				}

				QString path = filePath + QString().sprintf("_ttris_%d", i);

				saveTTrisRedoFile(path, pMesh->m_ttris, start, finish);
			});
	}
}

void CUndoRedo::saveTTrisRedoFile(
	QString filePath,
	std::vector<mip::TTri>& _vt_tris,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_ttris = _end - _start;

		//file.write((char*)(&n_ttris), sizeof(int));
		//file.write((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);

		int flag;
		for (int i = _start; i < _end; ++i)
		{
			flag = _vt_tris[i].getFlag();

			file.write((char*)(&flag), sizeof(int));

			file.write((char*)_vt_tris[i].vi, sizeof(int) * 3);
			file.write((char*)(&_vt_tris[i].hei), sizeof(int));
		}

		file.close();
	}
}

void CUndoRedo::saveTHedgeRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_thedge";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		int n_thedge = pMesh->m_tVHedges.size();

		file.write((char*)(&n_thedge), sizeof(int));
		file.write((char*)pMesh->m_tVHedges.data(), sizeof(mip::THEdge) * n_thedge);

		file.close();
	}
}

void CUndoRedo::saveTHedgeRedoFile(QString filePath, std::vector<mip::THEdge>& _vt_edges, int _start, int _end)
{
	QFile file(filePath);

	if (file.open(QIODevice::WriteOnly))
	{
		int  n_thedge = _end - _start;

		//file.write((char*)(&n_thedge), sizeof(int));

		auto p_hedge = &_vt_edges[_start];

		file.write((char*)p_hedge, sizeof(mip::THEdge) * n_thedge);

		file.close();
	}
}

void CUndoRedo::saveTBrushDataFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_tBrushData";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		//write 2arr 
		int  n_tbrush2ArrDV_1 = pMesh->m_2ArrforDisplayVerts.size();

		file.write((char*)(&n_tbrush2ArrDV_1), sizeof(int));
		for (int ii = 0; ii < n_tbrush2ArrDV_1; ii++)
		{
			muint32 size = pMesh->m_2ArrforDisplayVerts[ii].size();
			file.write((char*)(&size), sizeof(muint32));
			file.write((char*)pMesh->m_2ArrforDisplayVerts[ii].data(), sizeof(muint32) * size);
		}

		//int  n_tbrush2ArrDN_1 = pMesh->m_2ArrforDisplayNormals.size();

		//file.write((char*)(&n_tbrush2ArrDN_1), sizeof(int));
		//for (int ii = 0; ii < n_tbrush2ArrDN_1; ii++)
		//{
		// muint32 size = pMesh->m_2ArrforDisplayNormals[ii].size();
		// file.write((char*)(&size), sizeof(muint32));
		// file.write((char*)pMesh->m_2ArrforDisplayNormals[ii].data(), sizeof(muint32) * size);
		//}  

		file.close();
	}

	//if (_b_single_mode)
	//{
	// QString path = filePath + "_tBrushData";

	// QFile file(path);

	// if (file.open(QIODevice::WriteOnly))
	// {
	//  //write 2arr 
	//  int  n_tbrush2ArrDV_1 = pMesh->m_2ArrforDisplayVerts.size();

	//  file.write((char*)(&n_tbrush2ArrDV_1), sizeof(int));
	//  for (int ii = 0; ii < n_tbrush2ArrDV_1; ii++)
	//  {
	// muint32 size = pMesh->m_2ArrforDisplayVerts[ii].size();
	// file.write((char*)(&size), sizeof(muint32));
	// file.write((char*)pMesh->m_2ArrforDisplayVerts[ii].data(), sizeof(muint32) * size);
	//  }

	//  //int  n_tbrush2ArrDN_1 = pMesh->m_2ArrforDisplayNormals.size();

	//  //file.write((char*)(&n_tbrush2ArrDN_1), sizeof(int));
	//  //for (int ii = 0; ii < n_tbrush2ArrDN_1; ii++)
	//  //{
	//  // muint32 size = pMesh->m_2ArrforDisplayNormals[ii].size();
	//  // file.write((char*)(&size), sizeof(muint32));
	//  // file.write((char*)pMesh->m_2ArrforDisplayNormals[ii].data(), sizeof(muint32) * size);
	//  //}  

	//  file.close();
	// }
	//}
	//else
	//{
	// QString path = filePath + "_tBrushData";

	// QFile file(path);

	// if (file.open(QIODevice::WriteOnly))
	// {
	//  //write 2arr 
	//  int  n_tbrush2ArrDV_1 = pMesh->m_2ArrforDisplayVerts.size();

	//  file.write((char*)(&n_tbrush2ArrDV_1), sizeof(int));
	//  file.close();
	// }

	// // Save parallel method
	// const auto  num_core = std::thread::hardware_concurrency();
	// int  n_core = (int)num_core;
	// int  n_tbrush2ArrDV_1 = pMesh->m_2ArrforDisplayVerts.size();
	// int  interval = n_tbrush2ArrDV_1 / n_core;
	// int  remain = n_tbrush2ArrDV_1 % n_core;

	// //for (auto i = 0; i < num_core; ++i)
	// concurrency::parallel_for(0, n_core, [&](int i)
	// {
	//  int start = interval * i;
	//  int finish = start + interval;

	//  if (i == num_core - 1)
	//  {
	// finish += remain;
	//  }

	//  QString path = filePath + QString().sprintf("_tBrushData_%d", i);

	//  saveTBrushDataFile(path, pMesh->m_2ArrforDisplayVerts, start, finish);
	// });
	//}
}

void CUndoRedo::saveTBrushDataFile(QString filePath, std::vector<std::vector<muint32>>& _vt_verts, int _start, int _end)
{
	QFile file(filePath);

	if (file.open(QIODevice::WriteOnly))
	{
		//write 2arr 
		int  n_tbrush2ArrDV_1 = _end - _start;

		file.write((char*)(&n_tbrush2ArrDV_1), sizeof(int));
		for (int ii = _start; ii < _end; ii++)
		{
			muint32 size = _vt_verts[ii].size();
			file.write((char*)(&size), sizeof(muint32));
			file.write((char*)_vt_verts[ii].data(), sizeof(muint32) * size);
		}

		file.close();
	}
}

void CUndoRedo::saveOctreeRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	// 재작성 필요 
	//if (pMesh->m_treeTris)
	//{
	// QString path = filePath + "_octree";

	// QFile file(path);

	// if (file.open(QIODevice::WriteOnly))
	// {
	//  //file.write((char*)pMesh->m_treeTris, sizeof(mip::OcTreeTri));

	//  auto &  p_tree = pMesh->m_treeTris;

	//  float  max_size = p_tree->getMaxSize();
	//  
	//  UINT  max_count = p_tree->getMaxCount();
	//  
	//  mip::VECTOR3  size = p_tree->getSize();

	//  mip::TREETYPE  tree_type = p_tree->getType();


	//  file.write((char*)(&max_size), sizeof(float));
	//  file.write((char*)(&max_count), sizeof(UINT));
	//  file.write((char*)(&size), sizeof(mip::VECTOR3));
	//  file.write((char*)(&tree_type), sizeof(mip::TREETYPE));

	//  // octree depth 4
	//  mip::OcNode* p_root = p_tree->getRoot();

	//  saveOctreeRoot(&file, p_root);

	//  if (p_root)
	//  {
	// int n_root = 8;
	// for (int i = 0; i < n_root; ++i)
	// {
	//  mip::OcNode* p_child1 = p_root->pChildren[i];
	//  saveOctreeRoot(&file, p_child1);

	//  if (!p_child1)
	//  {
	//   continue;
	//  }

	//  for (int j = 0; j < n_root; ++j)
	//  {
	//   mip::OcNode* p_child2 = p_child1->pChildren[j];
	//   saveOctreeRoot(&file, p_child2);

	//   if (!p_child2)
	//   {
	//  continue;
	//   }

	//   for (int k = 0; k < n_root; ++k)
	//   {
	//  mip::OcNode* p_child3 = p_child2->pChildren[k];
	//  saveOctreeRoot(&file, p_child3);

	//  if (!p_child3)
	//  {
	//   continue;
	//  }

	//  for (int l = 0; l < n_root; ++l)
	//  {
	//   mip::OcNode* p_child4 = p_child3->pChildren[l];
	//   saveOctreeRoot(&file, p_child4);
	//  }
	//   }
	//  }
	// }
	//  }

	//  file.close();
	// }
	//}
}

void CUndoRedo::saveOctreeRoot(QFile* _p_file, mip::OcNode* _p_root)
{
	if (_p_root)
	{
		_p_file->write((char*)(&_p_root->box.max), sizeof(mip::VECTOR3));
		_p_file->write((char*)(&_p_root->box.min), sizeof(mip::VECTOR3));

		int n_tris = _p_root->tris.size();
		_p_file->write((char*)(&n_tris), sizeof(int));


		_p_file->write((char*)(_p_root->tris.data()), sizeof(int) * n_tris);
	}
	else
	{
		char* str = "NULL";
		_p_file->write(str, sizeof(char) * 5);
	}
}

void CUndoRedo::saveManipulatorInfo(
	DataContext* pDataContext,
	QString  filePath,
	int _idx_mesh,
	bool  _b_prev
)
{
	if (!pDataContext)
	{
		return;
	}

	QString path = filePath + "_info";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		printf_s("save : %s\n", path.toLocal8Bit().toStdString().c_str());

		if (!_b_prev)
		{
			auto mesh = pDataContext->m_MeshData.GetMesh(_idx_mesh);
			file.write((char*)(&mesh->rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&mesh->translation), sizeof(mip::VECTOR3));
			file.write((char*)(&mesh->scale), sizeof(mip::VECTOR3));

			for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
			{
				auto mat = m_pMeshManipulator->getTransformMatAxis(_idx_mesh, i);
				file.write((char*)(&mat), sizeof(mip::MATRIX44));
			}

			auto trans = m_pMeshManipulator->getTransformMat(_idx_mesh);
			file.write((char*)(&trans.rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&trans.translation), sizeof(mip::VECTOR3));
			file.write((char*)(&trans.scale), sizeof(mip::VECTOR3));

			auto trans_prev = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
			file.write((char*)(&trans_prev.rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&trans_prev.translation), sizeof(mip::VECTOR3));
			file.write((char*)(&trans_prev.scale), sizeof(mip::VECTOR3));
		}
		else
		{
			//auto mat = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
			//file.write((char*)(&mat), sizeof(mip::MATRIX44));

			auto tr = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
			file.write((char*)(&tr.rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&tr.translation), sizeof(mip::VECTOR3));
			file.write((char*)(&tr.scale), sizeof(mip::VECTOR3));

			for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
			{
				auto mat = m_pMeshManipulator->getPrevTransformMatAxis(_idx_mesh, i);
				file.write((char*)(&mat), sizeof(mip::MATRIX44));
			}

			auto trans_prev = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
			file.write((char*)(&trans_prev.rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&trans_prev.translation), sizeof(mip::VECTOR3));
			file.write((char*)(&trans_prev.scale), sizeof(mip::VECTOR3));

			auto trans = m_pMeshManipulator->getTransformMat(_idx_mesh);
			file.write((char*)(&trans.rotation), sizeof(mip::QUATERNION));
			file.write((char*)(&trans.translation), sizeof(mip::VECTOR3));
			file.write((char*)(&trans.scale), sizeof(mip::VECTOR3));
		}

		//auto mat_trans = m_pMeshManipulator->getTransformMat(_idx_mesh);
		//file.write((char*)(&mat_trans), sizeof(mip::MATRIX44));
		//auto mat_prev_trans = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
		//file.write((char*)(&mat_prev_trans), sizeof(mip::MATRIX44));

		//auto trans = m_pMeshManipulator->getTransformMat(_idx_mesh);
		//file.write((char*)(&trans.rotation), sizeof(mip::QUATERNION));
		//file.write((char*)(&trans.translation), sizeof(mip::VECTOR3));
		//file.write((char*)(&trans.scale), sizeof(mip::VECTOR3));

		//auto trans_prev = m_pMeshManipulator->getPrevTransformMat(_idx_mesh);
		//file.write((char*)(&trans_prev.rotation), sizeof(mip::QUATERNION));
		//file.write((char*)(&trans_prev.translation), sizeof(mip::VECTOR3));
		//file.write((char*)(&trans_prev.scale), sizeof(mip::VECTOR3));

		file.close();
	}
}

void CUndoRedo::saveHoleInfo(QString filePath)
{
	QString path = filePath + "hole_info";

	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		auto hole_info = m_pMeshHoleFillManager->getBoundaryIdxPtr();

		auto hole_num = hole_info->size();

		file.write((char*)(&hole_num), sizeof(int));

		for (int i = 0; i < hole_num; ++i)
		{
			int  n_border = (*hole_info)[i].first.size();
			bool    b_check = (*hole_info)[i].second;

			file.write((char*)(&n_border), sizeof(int));
			file.write((char*)(&b_check), sizeof(bool));

			for (int j = 0; j < n_border; ++j)
			{
				file.write((char*)(&(*hole_info)[i].first[j]), sizeof(int));
			}
		}

		file.close();
	}
}

void CUndoRedo::loadDataAllRedoFile(QString filePath, mip::MeshTopology* pMesh, MESH_WORK_MODE _work_mode)
{
	pMesh->m_tverts.clear();
	pMesh->m_ttris.clear();
	pMesh->m_tVHedges.clear();
	pMesh->m_verts.clear();
	pMesh->m_tris.clear();
	pMesh->m_normals.clear();
	pMesh->m_vertColors.clear();
	pMesh->m_2ArrforDisplayVerts.clear();

	const auto num_core = std::thread::hardware_concurrency();

	if (num_core >= 4)
	{
		concurrency::parallel_invoke(
			// [this, filePath, pMesh]
			//{
			// //loadCoreVertexRedoFile(filePath, pMesh);
			// //loadCoreTrisRedoFile(filePath, pMesh);
			// //loadCoreNormalRedoFile(filePath, pMesh);
			// //loadCoreColorRedoFile(filePath, pMesh);
			//},
			[this, filePath, pMesh]
			{
				loadTVertexRedoFile(filePath, pMesh);
			},
				[this, filePath, pMesh]
			{
				loadTTrisRedoFile(filePath, pMesh);
			},
				[this, filePath, pMesh]
			{
				loadTHedgeRedoFile(filePath, pMesh);
				//loadOctreeRedoFile(filePath, pMesh);
			},
				[this, filePath, pMesh]
			{
				loadTBrushDataFile(filePath, pMesh, true); //20210608_byPHS_BrushData추가
			},
				[this, filePath, pMesh]
			{
				loadCoreTextureRedoFile(filePath, pMesh); //20210614_byPHS_3mf_Un/Redo추가
			}
			);
	}
	else
	{
		//loadCoreVertexRedoFile(filePath, pMesh);
		//loadCoreTrisRedoFile(filePath, pMesh);
		//loadCoreNormalRedoFile(filePath, pMesh);
		//loadCoreColorRedoFile(filePath, pMesh, true);
		loadCoreTextureRedoFile(filePath, pMesh); //20210614_byPHS_3mf_Un/Redo추가
		loadTVertexRedoFile(filePath, pMesh, true);
		loadTTrisRedoFile(filePath, pMesh, true);
		loadTHedgeRedoFile(filePath, pMesh, true);
		loadTBrushDataFile(filePath, pMesh, true);
		//loadOctreeRedoFile(filePath, pMesh);
	}

	//if (pMesh->m_boundingBox.isNull())
	{
		updateBB(pMesh);
	}

	MEVolumeView* pMeshEditView = nullptr;
	if (m_pWinManager->mainMeshWidget)
	{
		pMeshEditView = m_pWinManager->mainMeshWidget->getMainView();;
	}
	MESH_WORK_MODE workMode = m_pMeshWorkManager->getWorkMode();

	switch (workMode)
	{
	case MESH_WORK_BRUSH_SELECTION:
	case MESH_WORK_SMOOTH:
	case MESH_WORK_REDUCTION:
	{
		pMesh->updateVertex();
		if (pMesh->m_uv.size() < 1 || pMesh->m_textureID.size() < 1 || pMesh->m_TextureDataList.size() < 1)
		{
			pMesh->initShader(mip::SHADERTYPE::SHADER_PONG);
		}
		else
		{
			pMesh->initShader(mip::SHADERTYPE::SHADER_3MF);
		}

		if (pMeshEditView)
		{
			pMeshEditView->updateSelectionUI(pMesh);
		}

		if (static_cast<ReduceDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_REDUCE)) != nullptr)
		{
			static_cast<ReduceDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_REDUCE))->setTopology(pMesh);
			static_cast<ReduceDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_REDUCE))->initOptionValue(static_cast<ReduceDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_DIALOG_REDUCE))->getReduceMode());
		}
	}
	break;
	default:
	{
		// 222121 허 건 과장
		//if (mode != MESH_WORK_POLYGON_CUT && 
		// mode != MESH_WORK_POLYLINE_CUT &&
		// mode != MESH_WORK_FREEPOLYLINE_CUT
		//)
		//{
		// m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag(pMesh);
		//}

		//20210615_byPHS_MeshEdit uv 작업 전까지 3mf파일을 기본 쉐이더로
		//if (m_pMeshDlgManager->getMeshDialog() != nullptr)
		//pMesh->initShader(mip::SHADERTYPE::SHADER_PONG);

		if (m_pMeshWorkManager->getWorkMode() == MESH_WORK_HOLLOW)
		{
			HollowDlg* dlg = static_cast<HollowDlg*>(m_pMeshDlgManager->getMeshDialog(MESH_WORK_HOLLOW));
			if (dlg && dlg->isVisible())
			{
				if (dlg->getState() == 0) //apply 
				{
					m_pMeshViewRenderManager->delTempMesh();
				}
				else
				{
					dlg->setState(0);
				}
			}
		}

		mip::mesh_control::updateDisplay(pMesh);
	}
	break;
	}

	pMesh->updateColor(pMesh->m_baseColor);

	QPolygon* poly_line = m_pMeshCutManager->getPolyLinePtr();
	poly_line->clear();

	auto hole_ptr = m_pMeshCutManager->getHolePtr();
	hole_ptr->clear();

	if (pMeshEditView)
	{
		pMeshEditView->updateGeometryCount();
	}

	m_pWinManager->buildRenderBufferTopology(pMesh);
}

void CUndoRedo::loadCoreVertexRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_vertex";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		file.read((char*)(&pMesh->m_boundingBox.max), sizeof(mip::VECTOR3));
		file.read((char*)(&pMesh->m_boundingBox.min), sizeof(mip::VECTOR3));

		int  n_verts;

		file.read((char*)(&n_verts), sizeof(int));

		pMesh->m_verts.reserve(n_verts);

		pMesh->m_verts.assign(n_verts, mip::VECTOR3());

		mip::VECTOR3* data = pMesh->m_verts.data();

		file.read((char*)data, sizeof(mip::VECTOR3) * n_verts);

		file.close();
	}
}

void CUndoRedo::loadCoreTrisRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_tris";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_tris;

		file.read((char*)(&n_tris), sizeof(int));

		pMesh->m_tris.reserve(n_tris);

		pMesh->m_tris.assign(n_tris, 0);

		muint32* data = pMesh->m_tris.data();

		file.read((char*)data, sizeof(muint32) * n_tris);

		file.close();
	}
}

void CUndoRedo::loadCoreNormalRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_normal";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_normals;

		file.read((char*)(&n_normals), sizeof(int));

		pMesh->m_normals.reserve(n_normals);

		pMesh->m_normals.assign(n_normals, mip::VECTOR3());

		file.read((char*)pMesh->m_normals.data(), sizeof(mip::VECTOR3) * n_normals);

		file.close();
	}
}

void CUndoRedo::loadCoreColorRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_color";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_colors;

		file.read((char*)(&n_colors), sizeof(int));

		pMesh->m_vertColors.reserve(n_colors);

		pMesh->m_vertColors.assign(n_colors, mip::VECTOR4());

		file.read((char*)pMesh->m_vertColors.data(), sizeof(mip::VECTOR4) * n_colors);

		file.read((char*)&pMesh->m_baseColor, sizeof(mip::VECTOR4));

		file.close();
	}

	//if (_b_single_mode)
	//{
	// QString path = filePath + "_color";

	// QFile file(path);

	// if (file.open(QIODevice::ReadOnly))
	// {
	//  int  n_colors;

	//  file.read((char*)(&n_colors), sizeof(int));

	//  pMesh->m_vertColors.reserve(n_colors);

	//  pMesh->m_vertColors.assign(n_colors, mip::VECTOR4());

	//  file.read((char*)pMesh->m_vertColors.data(), sizeof(mip::VECTOR4) * n_colors);

	//  file.close();
	// }
	//}
	//else
	//{
	// QString path = filePath + "_color";

	// QFile file(path);

	// if (file.open(QIODevice::ReadOnly))
	// {
	//  int  n_colors;

	//  file.read((char*)(&n_colors), sizeof(int));

	//  pMesh->m_vertColors.resize(n_colors, mip::VECTOR4());

	//  file.close();

	//  const auto  num_core = std::thread::hardware_concurrency();
	//  int  n_core = (int)num_core;

	//  int  interval = n_colors / n_core;
	//  int  remain = n_colors % n_core;

	//  concurrency::parallel_for(0, n_core, [&](int i)
	//  {
	// int start = interval * i;
	// int finish = start + interval;

	// if (i == num_core - 1)
	// {
	//  finish += remain;
	// }

	// QString path = filePath + QString().sprintf("_color_%d", i);

	// loadCoreColorRedoFile(path, pMesh->m_vertColors, start, finish);
	//  });
	// }
	//}
}

void CUndoRedo::loadCoreColorRedoFile(
	QString filePath,
	std::vector<mip::VECTOR4>& _vt_color,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_colors;

		file.read((char*)(&n_colors), sizeof(int));

		auto p_color = &_vt_color[_start];
		file.read((char*)p_color, sizeof(mip::VECTOR4) * n_colors);

		file.close();
	}
}

void CUndoRedo::loadCoreTextureRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	QString path = filePath + "_texture";

	QFile file(path);

	//20211013_byPHS_3MF 로드를 위해 수정
	//if (pMesh->m_uv.size() < 1 || pMesh->m_textureID.size() < 1 || pMesh->m_TextureDataList.size() < 1) return;

	if (file.open(QIODevice::ReadOnly))
	{
		//UV
		int  n_UVs;
		file.read((char*)(&n_UVs), sizeof(int));

		pMesh->m_uv.reserve(n_UVs);
		pMesh->m_uv.assign(n_UVs, mip::VECTOR2());
		file.read((char*)pMesh->m_uv.data(), sizeof(mip::VECTOR2) * n_UVs);

		//TextureID
		int  n_textureID;
		file.read((char*)(&n_textureID), sizeof(int));
		pMesh->m_textureID.reserve(n_textureID);
		pMesh->m_textureID.assign(n_textureID, sizeof(float));
		file.read((char*)pMesh->m_textureID.data(), sizeof(float) * n_textureID);

		//Texture Data
		int  n_TextureDataCnt;
		file.read((char*)(&n_TextureDataCnt), sizeof(int));;

		for (int ii = 0; ii < n_TextureDataCnt; ii++)
		{
			int nameLength; std::wstring wsName;
			std::string strName;
			file.read((char*)(&nameLength), sizeof(int));
			strName.assign(nameLength, '0');
			file.read((char*)(&strName[0]), sizeof(char) * (nameLength));

			int width; int height; int Channels; int tileStyle; int dataSize;
			file.read((char*)(&width), sizeof(int));
			file.read((char*)(&height), sizeof(int));
			file.read((char*)(&Channels), sizeof(int));
			file.read((char*)(&tileStyle), sizeof(int));
			file.read((char*)(&dataSize), sizeof(int));

			mip::TEXTURE texture(strName, width, height, 0, Channels, tileStyle);
#if 1
			std::vector<uchar> data;
			data.reserve(sizeof(unsigned char) * dataSize);
			data.assign(dataSize, sizeof(unsigned char));
			file.read((char*)&data[0], sizeof(unsigned char) * dataSize);

			unsigned char* pTxtData = new unsigned char[dataSize];
			std::copy(data.begin(), data.end(), pTxtData);
#else
			unsigned char* pTxtData = new unsigned char[dataSize];
			file.read((char*)&pTxtData, sizeof(unsigned char) * dataSize);
#endif
			texture.setTextureData(pTxtData);

			m_pWinManager->makeCurrent();
			int textId = pMesh->createTexture(pTxtData, width, height, Channels, tileStyle);
			m_pWinManager->doneCurrent();

			std::string key = "Texture_" + std::to_string(ii + 1);
			pMesh->addTextureId(textId, key);

			pMesh->m_TextureDataList.push_back(texture);
			pMesh->initShader(mip::SHADERTYPE::SHADER_3MF);
		}

		file.close();
	}
}

void CUndoRedo::loadTVertexRedoFile(
	QString filePath,
	mip::MeshTopology* pMesh,
	bool _b_single_mode)
{
	QString path = filePath + "_tvertex";
	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		file.read((char*)(&pMesh->m_baseColor), sizeof(mip::VECTOR4));

		int n_tverts;

		file.read((char*)(&n_tverts), sizeof(int));

		pMesh->m_tverts.reserve(n_tverts);

		pMesh->m_tverts.assign(n_tverts, mip::TVert());

		//file.read((char*)pMesh->m_tverts.data(), sizeof(mip::TVert) * n_tverts);

		if (_b_single_mode)
		{
			int n_tis, n_heis, flag;
			for (int i = 0; i < n_tverts; ++i)
			{
				file.read((char*)(&flag), sizeof(int));

				//if ((flag & mip::DELETED) != 0 || (flag & mip::USER_SEL1) != 0)
				if ((flag & mip::DELETED) != 0)
				{
					pMesh->m_tverts[i].setD();
				}

				if ((flag & mip::SELECTED) != 0)
				{
					pMesh->m_tverts[i].setS();
				}

				file.read((char*)(&pMesh->m_tverts[i].vi), sizeof(int));

				file.read((char*)(&pMesh->m_tverts[i].pos), sizeof(mip::VECTOR3));

				file.read((char*)(&n_tis), sizeof(int));
				pMesh->m_tverts[i].tis.resize(n_tis, -1);
				file.read((char*)pMesh->m_tverts[i].tis.data(), sizeof(int) * n_tis);

				file.read((char*)(&n_heis), sizeof(int));
				pMesh->m_tverts[i].heis.resize(n_heis, -1);
				file.read((char*)pMesh->m_tverts[i].heis.data(), sizeof(int) * n_heis);

				file.read((char*)(&pMesh->m_tverts[i].weight), sizeof(float));

				file.read((char*)(&pMesh->m_tverts[i].ori_pos), sizeof(mip::VECTOR3));
				file.read((char*)(&pMesh->m_tverts[i].ori_nor), sizeof(mip::VECTOR3));
			}
		}

		bool b_topology;
		file.read((char*)(&b_topology), sizeof(bool));
		pMesh->setTopologyed(b_topology);

		mip::QUATERNION rotation;
		mip::VECTOR3 translation;
		mip::VECTOR3 scale;

		file.read((char*)(&rotation), sizeof(mip::QUATERNION));
		file.read((char*)(&translation), sizeof(mip::VECTOR3));
		file.read((char*)(&scale), sizeof(mip::VECTOR3));

		pMesh->setRotate(rotation);
		pMesh->setTranslate(translation);
		pMesh->setScale(scale);

		file.close();

		if (!_b_single_mode)
		{
			const auto  num_core = std::thread::hardware_concurrency();
			int  n_core = (int)num_core;

			int  interval = n_tverts / n_core;
			int  remain = n_tverts % n_core;

			//concurrency::parallel_for(0, n_core, [&](int i)
			for (int i = 0; i < n_core; ++i)
			{
				int start = interval * i;
				int finish = start + interval;

				if (i == num_core - 1)
				{
					finish += remain;
				}

				QString path = filePath + QString().sprintf("_tvertex_%d", i);

				loadTVertexRedoFile(path, pMesh->m_tverts, start, finish);
			}
			//});
		}
	}
}

void CUndoRedo::loadTVertexRedoFile(
	QString filePath,
	std::vector<mip::TVert>& _vt_tverts,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		int n_tis, n_heis, flag;
		for (int i = _start; i < _end; ++i)
		{
			file.read((char*)(&flag), sizeof(int));

			//if ((flag & mip::DELETED) != 0 || (flag & mip::USER_SEL1) != 0)
			if ((flag & mip::DELETED) != 0)
			{
				_vt_tverts[i].setD();
			}

			if ((flag & mip::SELECTED) != 0)
			{
				_vt_tverts[i].setS();
			}

			file.read((char*)(&_vt_tverts[i].vi), sizeof(int));

			file.read((char*)(&_vt_tverts[i].pos), sizeof(mip::VECTOR3));

			file.read((char*)(&n_tis), sizeof(int));
			_vt_tverts[i].tis.resize(n_tis, -1);
			file.read((char*)_vt_tverts[i].tis.data(), sizeof(int) * n_tis);

			file.read((char*)(&n_heis), sizeof(int));
			_vt_tverts[i].heis.resize(n_heis, -1);
			file.read((char*)_vt_tverts[i].heis.data(), sizeof(int) * n_heis);

			file.read((char*)(&_vt_tverts[i].weight), sizeof(float));

			file.read((char*)(&_vt_tverts[i].ori_pos), sizeof(mip::VECTOR3));
			file.read((char*)(&_vt_tverts[i].ori_nor), sizeof(mip::VECTOR3));
		}

		file.close();
	}
}

void CUndoRedo::loadTTrisRedoFile(
	QString filePath,
	mip::MeshTopology* pMesh,
	bool _b_single_mode
)
{
	//QString path = filePath + "_ttris";

	//QFile file(path);

	//if (file.open(QIODevice::ReadOnly))
	//{
	// int  n_ttris;

	// file.read((char*)(&n_ttris), sizeof(int));

	// pMesh->m_ttris.reserve(n_ttris);

	// pMesh->m_ttris.assign(n_ttris, mip::TTri());

	// //file.read((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);

	// int flag;
	// for (int i = 0; i < n_ttris; ++i)
	// {
	//  file.read((char*)(&flag), sizeof(int));

	//  if ((flag & mip::DELETED) != 0 || (flag & mip::USER_SEL1) != 0)
	//  {
	// pMesh->m_ttris[i].setD();
	//  }

	//  if ((flag & mip::SELECTED) != 0)
	//  {
	// pMesh->m_ttris[i].setS();
	//  }

	//  file.read((char*)pMesh->m_ttris[i].vi, sizeof(int) * 3);
	//  file.read((char*)(&pMesh->m_ttris[i].hei), sizeof(int));
	// }

	// file.close();
	//}

	QString path = filePath + "_ttris";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_ttris;

		file.read((char*)(&n_ttris), sizeof(int));

		pMesh->m_ttris.reserve(n_ttris);

		pMesh->m_ttris.assign(n_ttris, mip::TTri());

		//file.read((char*)pMesh->m_ttris.data(), sizeof(mip::TTri) * n_ttris);

		if (_b_single_mode)
		{
			int flag;
			for (int i = 0; i < n_ttris; ++i)
			{
				file.read((char*)(&flag), sizeof(int));

				if ((flag & mip::DELETED) != 0)
					//if ((flag & mip::DELETED) != 0 || (flag & mip::USER_SEL1) != 0)
				{
					pMesh->m_ttris[i].setD();
				}

				if ((flag & mip::SELECTED) != 0)
				{
					pMesh->m_ttris[i].setS();
				}

				if ((flag & mip::USER_SEL1) != 0)
				{
					pMesh->m_ttris[i].setUS1();
				}

				file.read((char*)pMesh->m_ttris[i].vi, sizeof(int) * 3);
				file.read((char*)(&pMesh->m_ttris[i].hei), sizeof(int));
			}
		}

		file.close();

		if (!_b_single_mode)
		{
			const auto  num_core = std::thread::hardware_concurrency();
			int  n_core = (int)num_core;

			int  interval = n_ttris / n_core;
			int  remain = n_ttris % n_core;

			concurrency::parallel_for(0, n_core, [&](int i)
				{
					int start = interval * i;
					int finish = start + interval;

					if (i == num_core - 1)
					{
						finish += remain;
					}

					QString path = filePath + QString().sprintf("_ttris_%d", i);

					loadTTrisRedoFile(path, pMesh->m_ttris, start, finish);
				});
		}
	}
}

void CUndoRedo::loadTTrisRedoFile(QString filePath, std::vector<mip::TTri>& _vt_ttris, int _start, int _end)
{
	QFile file(filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		int flag;
		for (int i = _start; i < _end; ++i)
		{
			file.read((char*)(&flag), sizeof(int));

			//if ((flag & mip::DELETED) != 0 || (flag & mip::USER_SEL1) != 0)
			if ((flag & mip::DELETED) != 0)
			{
				_vt_ttris[i].setD();
			}

			if ((flag & mip::SELECTED) != 0)
			{
				_vt_ttris[i].setS();
			}

			if ((flag & mip::USER_SEL1) != 0)
			{
				_vt_ttris[i].setUS1();
			}

			file.read((char*)_vt_ttris[i].vi, sizeof(int) * 3);
			file.read((char*)(&_vt_ttris[i].hei), sizeof(int));
		}
	}
}

void CUndoRedo::loadTHedgeRedoFile(QString filePath, mip::MeshTopology* pMesh, bool _b_single_mode)
{
	QString path = filePath + "_thedge";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int  n_thedge;

		file.read((char*)(&n_thedge), sizeof(int));

		pMesh->m_tVHedges.reserve(n_thedge);

		pMesh->m_tVHedges.assign(n_thedge, mip::THEdge());

		file.read((char*)pMesh->m_tVHedges.data(), sizeof(mip::THEdge) * n_thedge);

		file.close();
	}

	//QString path = filePath + "_thedge";

	//QFile file(path);

	//if (file.open(QIODevice::ReadOnly))
	//{
	// int  n_thedge;

	// file.read((char*)(&n_thedge), sizeof(int));

	// pMesh->m_tVHedges.reserve(n_thedge);

	// pMesh->m_tVHedges.assign(n_thedge, mip::THEdge());

	// if (_b_single_mode)
	// {
	//  file.read((char*)pMesh->m_tVHedges.data(), sizeof(mip::THEdge) * n_thedge);
	// }

	// file.close();

	// if (!_b_single_mode)
	// {
	//  const auto  num_core = std::thread::hardware_concurrency();
	//  int  n_core = (int)num_core;

	//  int  interval = n_thedge / n_core;
	//  int  remain = n_thedge % n_core;

	//  concurrency::parallel_for(0, n_core, [&](int i)
	//  {
	// int start = interval * i;
	// int finish = start + interval;

	// if (i == num_core - 1)
	// {
	//  finish += remain;
	// }

	// QString path = filePath + QString().sprintf("_thedge_%d", i);

	// loadTHedgeRedoFile(path, pMesh->m_tVHedges, start, finish);
	//  });
	// }
	//}
}

void CUndoRedo::loadTHedgeRedoFile(
	QString filePath,
	std::vector<mip::THEdge>& _vt_thedge,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		auto n_thedge = _end - _start;
		auto p_hedge = &_vt_thedge[_start];

		file.read((char*)p_hedge, sizeof(mip::THEdge) * n_thedge);
	}
}

void CUndoRedo::loadTBrushDataFile(
	QString filePath,
	mip::MeshTopology* pMesh,
	bool _b_single_mode)
{
	QString path = filePath + "_tBrushData";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		int n_tbrush2ArrDV_1 = 0;

		file.read((char*)(&n_tbrush2ArrDV_1), sizeof(int));
		pMesh->m_2ArrforDisplayVerts.reserve(n_tbrush2ArrDV_1);

		for (int ii = 0; ii < n_tbrush2ArrDV_1; ii++)
		{
			muint32 size;
			file.read((char*)&size, sizeof(muint32));
			std::vector<muint32> list;
			list.reserve(size);
			list.assign(size, 0);
			file.read((char*)list.data(), sizeof(muint32) * size);
			pMesh->m_2ArrforDisplayVerts.push_back(list);
		}

		// ------------------------------------------

		//int  n_tbrush2ArrDN_1 = 0;

		//file.read((char*)(&n_tbrush2ArrDN_1), sizeof(int));
		//pMesh->m_2ArrforDisplayNormals.reserve(n_tbrush2ArrDN_1);

		//for (int ii = 0; ii < n_tbrush2ArrDN_1; ii++)
		//{
		// muint32 size;
		// file.read((char*)&size, sizeof(muint32));
		// std::vector<muint32> list;
		// list.reserve(size);
		// list.assign(size, 0);
		// file.read((char*)list.data(), sizeof(muint32) * size);
		// pMesh->m_2ArrforDisplayNormals.push_back(list);
		//}


		file.close();
	}

	//QString path = filePath + "_tBrushData";

	//QFile file(path);

	//if (file.open(QIODevice::ReadOnly))
	//{
	// int  n_tbrush2ArrDV_1 = 0;

	// file.read((char*)(&n_tbrush2ArrDV_1), sizeof(int));

	// if (_b_single_mode)
	// {
	//  pMesh->m_2ArrforDisplayVerts.reserve(n_tbrush2ArrDV_1);

	//  for (int ii = 0; ii < n_tbrush2ArrDV_1; ii++)
	//  {
	// muint32 size;
	// file.read((char*)&size, sizeof(muint32));
	// std::vector<muint32> list;
	// list.reserve(size);
	// list.assign(size, 0);
	// file.read((char*)list.data(), sizeof(muint32) * size);
	// pMesh->m_2ArrforDisplayVerts.push_back(list);
	//  }

	//  // ------------------------------------------

	//  //int  n_tbrush2ArrDN_1 = 0;

	//  //file.read((char*)(&n_tbrush2ArrDN_1), sizeof(int));
	//  //pMesh->m_2ArrforDisplayNormals.reserve(n_tbrush2ArrDN_1);

	//  //for (int ii = 0; ii < n_tbrush2ArrDN_1; ii++)
	//  //{
	//  // muint32 size;
	//  // file.read((char*)&size, sizeof(muint32));
	//  // std::vector<muint32> list;
	//  // list.reserve(size);
	//  // list.assign(size, 0);
	//  // file.read((char*)list.data(), sizeof(muint32) * size);
	//  // pMesh->m_2ArrforDisplayNormals.push_back(list);
	//  //}
	// }

	// file.close();

	// if (!_b_single_mode)
	// {
	//  pMesh->m_2ArrforDisplayVerts.resize(n_tbrush2ArrDV_1);

	//  const auto  num_core = std::thread::hardware_concurrency();
	//  int  n_core = (int)num_core;

	//  int  interval = n_tbrush2ArrDV_1 / n_core;
	//  int  remain = n_tbrush2ArrDV_1 % n_core;

	//  concurrency::parallel_for(0, n_core, [&](int i)
	//  {
	// int start = interval * i;
	// int finish = start + interval;

	// if (i == num_core - 1)
	// {
	//  finish += remain;
	// }

	// QString path = filePath + QString().sprintf("_thedge_%d", i);

	// loadTBrushDataFile(path, pMesh->m_2ArrforDisplayVerts, start, finish);
	//  });
	// }
	//}
}

void CUndoRedo::loadTBrushDataFile(
	QString filePath,
	std::vector<std::vector<muint32>>& _vt_diplay,
	int _start,
	int _end
)
{
	QFile file(filePath);

	if (file.open(QIODevice::ReadOnly))
	{
		for (int ii = _start; ii < _end; ii++)
		{
			muint32 size;
			file.read((char*)&size, sizeof(muint32));
			std::vector<muint32> list;
			list.reserve(size);
			list.assign(size, 0);
			file.read((char*)list.data(), sizeof(muint32) * size);
			_vt_diplay[ii].swap(list);
		}

		file.close();
	}
}

void CUndoRedo::loadOctreeRedoFile(QString filePath, mip::MeshTopology* pMesh)
{
	// 재작성 필요
	//QString path = filePath + "_octree";

	//QFile file(path);

	//if (file.open(QIODevice::ReadOnly))
	//{
	// auto &  p_tree = pMesh->m_treeTris;

	// float  max_size;

	// UINT  max_count ;

	// mip::VECTOR3  size;

	// mip::TREETYPE  tree_type;


	// file.read((char*)(&max_size), sizeof(float));
	// file.read((char*)(&max_count), sizeof(UINT));
	// file.read((char*)(&size), sizeof(mip::VECTOR3));
	// file.read((char*)(&tree_type), sizeof(mip::TREETYPE));

	// p_tree = new mip::OcTreeTri(tree_type, max_count, max_size);

	// p_tree->setSize(size);

	// // octree depth 4
	// mip::OcNode* p_root = p_tree->getRoot();

	// int n_depth = 4;
	// int n_root = 8;

	// p_root = new mip::OcNode();
	// loadOctreeRoot(&file, p_root);

	// if (p_root)
	// {
	//  for (int i = 0; i < n_root; ++i)
	//  {
	// mip::OcNode* p_child1 = p_root->pChildren[i];

	// p_child1 = new mip::OcNode();
	// loadOctreeRoot(&file, p_child1);

	// if (!p_child1)
	// {
	//  continue;
	// }

	// for (int j = 0; j < n_root; ++j)
	// {
	//  mip::OcNode* p_child2 = p_child1->pChildren[j];

	//  p_child2 = new mip::OcNode();
	//  loadOctreeRoot(&file, p_child2);

	//  if (!p_child2)
	//  {
	//   continue;
	//  }

	//  for (int k = 0; k < n_root; ++k)
	//  {
	//   mip::OcNode* p_child3 = p_child2->pChildren[k];

	//   p_child3 = new mip::OcNode();
	//   loadOctreeRoot(&file, p_child3);

	//   if (!p_child3)
	//   {
	//  continue;
	//   }

	//   for (int l = 0; l < n_root; ++l)
	//   {
	//  mip::OcNode* p_child4 = p_child3->pChildren[l];

	//  p_child4 = new mip::OcNode();
	//  loadOctreeRoot(&file, p_child4);

	//  if (!p_child4)
	//  {
	//   continue;
	//  }
	//   }
	//  }
	// }
	//  }
	// }

	// file.close();
	//}
}

void CUndoRedo::loadOctreeRoot(QFile* _p_file, mip::OcNode* _p_root)
{
	if (_p_root)
	{
		_p_file->read((char*)(&_p_root->box.max), sizeof(mip::VECTOR3));
		_p_file->read((char*)(&_p_root->box.min), sizeof(mip::VECTOR3));

		int n_tris = _p_root->tris.size();
		_p_file->read((char*)(&n_tris), sizeof(int));

		_p_root->tris.resize(n_tris, -1);

		_p_file->read((char*)(_p_root->tris.data()), sizeof(int) * n_tris);
	}
	else
	{
		_p_file->read((char*)_p_root, sizeof(mip::OcNode*));
	}
}

bool CUndoRedo::loadManipulatorInfo(
	DataContext* pDataContext,
	QString  filePath,
	int _idx_mesh
)
{
	if (!pDataContext)
	{
		return false;
	}

	QString path = filePath + "_info";

	QFile file(path);

	bool b_success = false;

	if (file.open(QIODevice::ReadOnly))
	{
		printf_s("load : %s\n", path.toLocal8Bit().toStdString().c_str());

		mip::TRANSFORM tr;

		file.read((char*)(&tr.rotation), sizeof(mip::QUATERNION));
		file.read((char*)(&tr.translation), sizeof(mip::VECTOR3));
		file.read((char*)(&tr.scale), sizeof(mip::VECTOR3));

		auto mesh = pDataContext->m_MeshData.GetMesh(_idx_mesh);

		mesh->setRotate(tr.rotation);
		mesh->setTranslate(tr.translation);
		mesh->setScale(tr.scale);

		for (int i = 0; i < NUM_AXIS_MANIPULATOR; ++i)
		{
			mip::MATRIX44 mat;
			file.read((char*)(&mat), sizeof(mip::MATRIX44));
			m_pMeshManipulator->UpdateTransformAxis(_idx_mesh, i, mat);
		}

		//mip::MATRIX44 mat_trans, mat_prev_trans;
		//file.read((char*)(&mat_trans), sizeof(mip::MATRIX44));
		//file.read((char*)(&mat_prev_trans), sizeof(mip::MATRIX44));

		//m_pMeshManipulator->setTransformMat(_idx_mesh, mat_trans);
		//m_pMeshManipulator->setPrevTransformMat(_idx_mesh, mat_prev_trans);

		mip::TRANSFORM trans, trans_prev;
		file.read((char*)(&trans.rotation), sizeof(mip::QUATERNION));
		file.read((char*)(&trans.translation), sizeof(mip::VECTOR3));
		file.read((char*)(&trans.scale), sizeof(mip::VECTOR3));

		file.read((char*)(&trans_prev.rotation), sizeof(mip::QUATERNION));
		file.read((char*)(&trans_prev.translation), sizeof(mip::VECTOR3));
		file.read((char*)(&trans_prev.scale), sizeof(mip::VECTOR3));

		m_pMeshManipulator->setTransformMat(_idx_mesh, trans);
		m_pMeshManipulator->setPrevTransformMat(_idx_mesh, trans_prev);

		file.close();

		b_success = true;
	}

	return b_success;
}

void CUndoRedo::loadHoleInfo(QString filePath)
{
	QString path = filePath + "hole_info";

	QFile file(path);

	if (file.open(QIODevice::ReadOnly))
	{
		auto hole_info = m_pMeshHoleFillManager->getBoundaryIdxPtr();

		int hole_num = 0;

		file.read((char*)(&hole_num), sizeof(int));

		hole_info->clear();
		hole_info->resize(hole_num);

		for (int i = 0; i < hole_num; ++i)
		{
			int  n_border = 0;
			bool    b_check = false;

			file.read((char*)(&n_border), sizeof(int));
			file.read((char*)(&b_check), sizeof(bool));

			(*hole_info)[i].first.resize(n_border);
			(*hole_info)[i].second = b_check;

			for (int j = 0; j < n_border; ++j)
			{
				file.read((char*)(&(*hole_info)[i].first[j]), sizeof(int));
			}
		}

		file.close();
	}
}

void CUndoRedo::updateBB(mip::MeshTopology* pMesh)
{
	auto& bb_min = pMesh->m_boundingBox.min;
	auto& bb_max = pMesh->m_boundingBox.max;

	auto& tverts = pMesh->m_tverts;
	int n_verts = (int)tverts.size();

	bb_min = mip::VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	bb_max = mip::VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int vi = 0; vi < n_verts; ++vi)
	{
		if (tverts[vi].isD() || tverts[vi].isUS1())
		{
			continue;
		}

		auto& pos = tverts[vi].pos;

		if (bb_min.x > pos.x)
		{
			bb_min.x = pos.x;
		}

		if (bb_min.y > pos.y)
		{
			bb_min.y = pos.y;
		}

		if (bb_min.z > pos.z)
		{
			bb_min.z = pos.z;
		}

		if (bb_max.x < pos.x)
		{
			bb_max.x = pos.x;
		}

		if (bb_max.y < pos.y)
		{
			bb_max.y = pos.y;
		}

		if (bb_max.z < pos.z)
		{
			bb_max.z = pos.z;
		}
	}
}

void CUndoRedo::deleteUndoRedoFile(const QString& id, QDir* dir)
{
	const QFileInfoList entryInfoList = dir->entryInfoList();

	for (int i = 0; i < entryInfoList.size(); ++i)
	{
		QString fileName = entryInfoList[i].fileName();

		if (compareFileNameUndoRedoId(id, fileName))
		{
			dir->remove(fileName);
		}
	}
}

bool CUndoRedo::compareFileNameUndoRedoId(const QString& id, const QString fileName)
{
	return fileName.contains(id);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


