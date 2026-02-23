#include "stdafx.h"
#include "VisualPrintWorkSurfaceExport.h"
#include "stringManager.h"
#include "windowManager.h"
#include "ActionManager.h"
#include "Renderer/MeshTopology.h"

#include <ppl.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintWorkSurfaceExport Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
VisualPrintWorkSurfaceExport::VisualPrintWorkSurfaceExport(QVector<int>& vecROIList, VOLUME_DATA* pVolume, MeshData* pMeshData, EXPORT_3D_SURFACE_MESH_METHOD method, bool bExtract)
	: VolumeTo3DSurface(pVolume, method)
{
	m_vecROIList = vecROIList;

	m_bExtract = bExtract;

	m_pMeshData = pMeshData;
}

VisualPrintWorkSurfaceExport::~VisualPrintWorkSurfaceExport()
{

}

void VisualPrintWorkSurfaceExport::singleProcess()
{
	if (m_vecROIList.empty() || !m_pVolumData)
	{
		emit finished();
		return;
	}

	int		nExport = (int)m_vecROIList.size();
	int		interval = 90 / nExport;
	int		progress_val = 0;

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	MAINTAB_TYPE prevType = WIN_MANAGER->mainTabType;

	double space[3];
	space[0] = m_pVolumData->getSpaceX();
	space[1] = m_pVolumData->getSpaceY();
	space[2] = m_pVolumData->getSpaceZ();

	const int	 nHeight = m_pVolumData->getCY();
	const int	 nWidth = m_pVolumData->getCX();
	const int	 nSlice = m_pVolumData->getCZ();

	const int nLength = nHeight * nWidth * nSlice;

	for (int i = 0; i < nExport; ++i)
	{
		MaskInfo* pMaskInfo = m_pVolumData->getMaskInfo(m_vecROIList[i]);

		mip::MeshTopology* pMeshToplogy = new mip::MeshTopology(g_Renderer);

		if (!pMaskInfo || !pMeshToplogy)
		{
			continue;
		}

		int						layerUID = pMaskInfo->uid;

		int						maskIdx = pMaskInfo->uid >= MASK_SECOND_MAX ? (pMaskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		mask					maskData = ((maskIdx == 0) ? pMaskInfo->mask_id : pMaskInfo->mask_id2);


		int			progress_tmp = 0;
		QString		progress_name;
		QString		file_name = QString::fromStdWString(pMaskInfo->maskName);

		progress_val = i * interval + 10;
		progress_name = file_name + QString().sprintf("(%d%)", progress_tmp);

		emit sig_progress(progress_val, file_name);

		Create3DSurface(pMeshToplogy, maskData, maskIdx);

		progress_tmp += 50;
		progress_val += interval / 6;
		progress_name = file_name + QString().sprintf("(%d%)", progress_tmp);
		emit sig_progress(progress_val, progress_name);

		ACTION_MANAGER->action_MeshList_add_VisualPrint(layerUID, file_name, pMeshToplogy);

		mint8 mUID = m_pMeshData->GetMeshUID(layerUID);

		MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(mUID);

		if (pMeshInfo)
		{
			pMeshInfo->uid = i;
		}

		mip::MeshTopology* p_mesh = m_pMeshData->GetMesh(i);

		if (layerUID != -1)
		{
			QColor col = m_pVolumData->getMaskColor(layerUID, true);

			p_mesh->m_baseColor = mip::VECTOR4(col.red() / 255.f, col.green() / 255.f, col.blue() / 255.f, 1.f);
		}

		p_mesh->updateColor(p_mesh->m_baseColor);

		m_pMeshData->MeshRenderUpdate(mUID);

		QString nameMesh = m_pMeshData->GetMeshName(mUID);
		QString saveFile = saveFile + "\\" + nameMesh + ".stl";
		WIN_MANAGER->saveMeshFilesVisualPrint(saveFile, i, mUID, EX_FILES_STL, false, false);

		progress_val = interval * (i + 1);
		progress_tmp = 100;
		progress_name = file_name + QString().sprintf("(%d%)", progress_tmp);
		emit sig_progress(progress_val, progress_name);
	}

	QString dicom_file = save_path + "\\DICOM.nrrd";

	// 201012 nii -> nrrd 변경
	WIN_MANAGER->saveNRRD(dicom_file, false);

	WIN_MANAGER->mainTabType = prevType;

	progress_val = 100;
	emit sig_progress(progress_val, "");

	emit finished();
}

void VisualPrintWorkSurfaceExport::parallelProcess()
{
	int		nExport = m_vecROIList.size();

	if (nExport < 1)
	{
		emit finished();
		return;
	}

	std::vector<mint32>					vecLayerUID(nExport, -1);
	std::vector<mip::MeshTopology*>		vecMeshTopology(nExport, NULL);
	std::vector<MaskInfo*>				vecMaskInfo(nExport, NULL);
	std::vector<QString>				vecFileName(nExport);

	concurrency::combinable<float>		val_progress;

	float								interval = 50.f / (float)nExport;

	QString		current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString		folder_name = "Temp";
	QString		save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	MAINTAB_TYPE prevType = WIN_MANAGER->mainTabType;

	for (int i = 0; i < nExport; ++i)
	{
		vecMeshTopology[i] = new mip::MeshTopology(g_Renderer);
		vecMaskInfo[i] = m_pVolumData->getMaskInfo(m_vecROIList[i]);
	}

	//concurrency::parallel_for(0, nExport, [&](int i)
	for (int i = 0; i < nExport; ++i)
	{
		float tmp_progress = 0.f;

		if (vecMaskInfo[i] && vecMeshTopology[i])
		{
			mint32					layer_uid = vecMaskInfo[i]->uid;

			int						maskIdx = vecMaskInfo[i]->uid >= MASK_SECOND_MAX ? (vecMaskInfo[i]->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			mask					maskData = ((maskIdx == 0) ? vecMaskInfo[i]->mask_id : vecMaskInfo[i]->mask_id2);

			QString					file_name = QString::fromStdWString(vecMaskInfo[i]->maskName);


			val_progress.local() += interval * 0.05f;
			tmp_progress = val_progress.combine(plus<float>());
			emit sig_progress((int)tmp_progress, "Generate Mesh...");

			Create3DSurface(vecMeshTopology[i], maskData, maskIdx);

			vecLayerUID[i] = layer_uid;
			vecFileName[i] = file_name;

			if (layer_uid != -1)
			{
				QColor col = m_pVolumData->getMaskColor(layer_uid, true);

				vecMeshTopology[i]->m_baseColor = mip::VECTOR4(col.red() / 255.f, col.green() / 255.f, col.blue() / 255.f, 1.f);
			}

			vecMeshTopology[i]->updateColor(vecMeshTopology[i]->m_baseColor);
		}

		val_progress.local() += interval * 0.6f;
		tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress((int)tmp_progress, "Generate Mesh...");
	}

	interval = 10.f / (float)nExport;

	for (int i = 0; i < nExport; ++i)
	{
		MaskInfo* pMaskInfo = m_pVolumData->getMaskInfo(m_vecROIList[i]);

		if (pMaskInfo)
		{
			ACTION_MANAGER->action_MeshList_add_VisualPrint(pMaskInfo->uid, vecFileName[i], vecMeshTopology[i]);

			mint8 mUID = m_pMeshData->GetMeshUID(pMaskInfo->uid);

			MeshInfo* pMeshInfo = m_pMeshData->GetMeshInfo(mUID);
			if (pMeshInfo)
			{
				pMeshInfo->uid = i;
			}

			m_pMeshData->MeshRenderUpdate(mUID);
		}

		val_progress.local() += interval;
		float tmp_progress = val_progress.combine(plus<float>());
		emit sig_progress((int)tmp_progress, "Generate Mesh...");
	}


	QString dicom_file = save_path + "\\DICOM.nrrd";

	// 201012 nii -> nrrd 변경
	WIN_MANAGER->saveNRRD(dicom_file, false);

	emit sig_progress(95, "Generate Mesh...");

	WIN_MANAGER->mainTabType = prevType;

	emit sig_progress(100, "Generate Mesh...");

	emit finished();
}

void VisualPrintWorkSurfaceExport::threadRun()
{
	parallelProcess();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintWorkSurfaceExport Class Member Functions - End
/////////////////////////////////////////////////////////////////////////////////////////////////////////////