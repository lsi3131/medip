#include "stdafx.h"
#include "WorkMeshTo3MF.h"
#include "windowManager.h"
#include "stringManager.h"
#include "ActionManager.h"
#include "3MF\C3MFLoader.h"

WorkMeshTo3MF::WorkMeshTo3MF(VOLUME_DATA* pVolumeData, MeshData* pMeshData, QString& strFilename, mip::MeshTopology* m, float sc, bool bPatientCoordinate) :
	_strFilename(strFilename),
	_mesh(m),
	_scale(sc),
	m_bPatientCoordinate(bPatientCoordinate),
	m_pVolumeData(pVolumeData),
	m_pMeshData(pMeshData)
{

}


void WorkMeshTo3MF::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkMeshTo3MF::threadRun()
{
	setProgressValue(0, true);

	setProgressValue(50);

	int exportCnt = WIN_MANAGER->exportList.size();

	std::vector<mip::MeshTopology*> meshlist;
	std::vector<std::string> meshNamelist;
	for (int ii = 0; ii < exportCnt; ii++)
	{
		int	index = WIN_MANAGER->exportList[ii];
		MeshInfo* _meshinfo = m_pMeshData->GetMeshInfo(index);
		mip::MeshTopology* m = m_pMeshData->GetMesh(index);
		meshlist.push_back(m);

		std::wstring wsName(_meshinfo->MeshName);
		std::string strName(wsName.begin(), wsName.end());
		meshNamelist.push_back(strName);
	}

	_strFilename.replace(".3mf", "");
	_strFilename.replace(".", "");
	_strFilename.replace(".3MF", "");

	_strFilename += ".3mf";

	std::string strFilename = _strFilename.toUtf8().constData();
	mip::lib3mf::C3MFLoader _3mf;
	_3mf.Save3mfFile(strFilename, meshlist, meshNamelist, 10);

	//tempData Delete
	std::string sTextureFolder = STRING_MANAGER->m_strAppDataLocalPath.toUtf8().constData();
	for (int ii = 0; ii < meshlist.size(); ii++)
	{
		for (int jj = 0; jj < meshlist[ii]->m_TextureDataList.size(); jj++)
		{
			std::string TempName = "/temp_Write_3mf_";
			std::string filename = TempName + std::to_string(ii) + std::to_string(jj) + ".png";
			std::remove((sTextureFolder + filename).c_str());
		}
	}

	setProgressValue(100);

	if (m_pVolumeData->threadResult && m_pVolumeData->threadStop)
	{
		LPCWSTR strVariable = (const wchar_t*)_strFilename.utf16();

		::DeleteFile(strVariable);
	}

	ACTION_MANAGER->SetAfterThread(THREAD_NONE);

	emit finished();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
