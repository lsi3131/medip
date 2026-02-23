#include "stdafx.h"
#include "VisualPrintWorkMeshExport.h"
#include "windowManager.h"
#include "stringManager.h"
#include "Renderer/MeshTopology.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintWorkMeshExport Class Member Functions - Start
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
VisualPrintWorkMeshExport::VisualPrintWorkMeshExport(MeshData* pVisualPrintMeshData, MeshData* pSourceMeshData)
{
	m_pVisualPrintMeshData = pVisualPrintMeshData;
	m_pSourceMeshData = pSourceMeshData;
}

VisualPrintWorkMeshExport::~VisualPrintWorkMeshExport()
{

}

/*
@brief
@return
*/
void	VisualPrintWorkMeshExport::slot_threadRun()
{
	int meshCount = m_pSourceMeshData->GetMeshCount();

	float interval = 95.f / (float)meshCount;
	float progress_val = 0.f;

	WIN_MANAGER->roiUploadList.clear();
	WIN_MANAGER->roiDeleteList.clear();
	WIN_MANAGER->roiSaveMeshList.clear();

	//WIN_MANAGER->vt_pckIDVisualPrint.clear();
	m_pVisualPrintMeshData->ClearMeshInfo();

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	// For js file
	QString file = save_path + "\\files.js";
	FILE* fp = fopen(file.toLocal8Bit().toStdString().c_str(), "wt");

	fputs("var files = [\n", fp);

	MAINTAB_TYPE prevType = WIN_MANAGER->mainTabType;

	//WIN_MANAGER->mainTabType = MAINTAB_TYPE::MAINTAB_VISUAL_PRINT_MESH_EDITING;

	for (int i = 0; i < meshCount; ++i)
	{
		int progress_tmp = 0;

		mip::MeshTopology* meshSrc = m_pSourceMeshData->GetMesh(i);
		QString meshNameSrc = m_pSourceMeshData->GetMeshName(i);
		MeshInfo* meshInfoSrc = m_pSourceMeshData->GetMeshInfo(i);

		QString progress_name;

		// Update upload list
		QString save_file = save_path + "\\" + meshNameSrc + ".stl";

		WIN_MANAGER->roiUploadList.push_back(save_file);
		WIN_MANAGER->roiDeleteList.push_back(save_file);

		// Copy mesh form mesh-list
		{
			//mip::MeshTopology* copy_mesh = nullptr;
			//*copy_mesh = *mesh; //<- 오류 발생

			progress_tmp += 100.f * 0.1f;
			progress_val += interval * 0.1f;

			progress_name = meshNameSrc + QString().sprintf("(%d%)", progress_tmp);
			emit sig_progress((int)progress_val, progress_name);

			// 201109 허 건 대리
			// copy data
			{
				m_pVisualPrintMeshData->CreateMeshInfo();

				int uid = m_pVisualPrintMeshData->GetCurrentMeshIndex();

				m_pVisualPrintMeshData->SetMeshName(meshNameSrc, uid);

				MeshInfo* info = m_pVisualPrintMeshData->GetMeshInfo(uid);

				if (info->uid == -1)
				{
					info->uid = uid;
				}

				info->color = meshInfoSrc->color;
				meshSrc->m_baseColor = mip::VECTOR4(info->color.r / 255.f, info->color.g / 255.f, info->color.b / 255.f, 1.f);


				if (info)
				{
					info->upScale = false;
				}

				//WIN_MANAGER->vt_pckIDVisualPrint.push_back(true);
				m_pVisualPrintMeshData->SetMeshInfoModeSelectMode(uid, true);

				m_pVisualPrintMeshData->CopyMeshMap(uid, meshSrc);


				//if (mesh_info->uid != -1)
				//{
				//	DATA_CONTEXT->volume_data.VisualPrintSetConnectMesh(m_UID, mesh_info->uid);
				//}
			}
		}

		progress_tmp += 100.f * 0.75f;
		progress_val += interval * 0.75f;

		progress_name = meshNameSrc + QString().sprintf("(%d%)", progress_tmp);
		emit sig_progress((int)progress_val, progress_name);

		// change id && color
		{
			MeshInfo* mesh_info_visual = m_pVisualPrintMeshData->GetMeshInfo(i);

			mesh_info_visual->uid = i;
			mesh_info_visual->color = meshInfoSrc->color;
		}

		progress_tmp += 100.f * 0.05f;
		progress_val += interval * 0.05f;

		progress_name = meshNameSrc + QString().sprintf("(%d%)", progress_tmp);
		emit sig_progress((int)progress_val, progress_name);

		// Save stl file
		WIN_MANAGER->saveMeshFilesVisualPrint(save_file, i, meshInfoSrc->uid, EX_FILES_STL, false, false);

		// Update js file
		{
			QString fileName = "";
			QString strSTL = "";

			strSTL = save_path;

			fileName = strSTL + "/" + meshNameSrc + ".stl";
			fileName.replace("/", "\\");

			QString savename = fileName.section("\\", -1);

			// Mesh 정보 파일 Export			
			fputs("{\n", fp);

			// Mesh Name Export
			QString tempName = savename;
			tempName.chop(4);

			QString FullName = "\"name\": \"" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			// Export Path Info			
			QString qstrPath = "data/" + WIN_MANAGER->qstrURL_LinkPath;
			FullName = "\"filePath\" : \"" + qstrPath + "/" + savename + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);

			char cTemp[16];
			snprintf(cTemp, sizeof cTemp, "%02x%02x%02x", meshInfoSrc->color.r, meshInfoSrc->color.g, meshInfoSrc->color.b);
			tempName = QString(cTemp);
			FullName = "\"color\" : \"#" + tempName + "\",\n";
			fputs(FullName.toLocal8Bit().toStdString().c_str(), fp);
			fputs("\"opacity\" : 1,\n", fp);
			fputs("\"visible\" : true,\n", fp);
			fputs("},\n", fp);
		}

		progress_tmp = 100.f;
		progress_val += interval * 0.1f;

		progress_name = meshNameSrc + QString().sprintf("(%d%)", progress_tmp);
		emit sig_progress((int)progress_val, progress_name);
	}

	fputs("];", fp);
	fclose(fp);

	QString dicom_file = save_path + "\\" + "DICOM.nrrd";

	// 201012 nii -> nrrd 변경
	WIN_MANAGER->saveNRRD(dicom_file, false);

	WIN_MANAGER->roiUploadList.push_back(dicom_file);
	WIN_MANAGER->roiDeleteList.push_back(dicom_file);

	WIN_MANAGER->mainTabType = prevType;

	emit sig_progress(100, "");

	emit sig_finished();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VisualPrintWorkMeshExport Class Member Functions - End
//////////////////////////////////////////////////////////////////////////////////////////////////////////////