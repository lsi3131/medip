/*
@brief WorkLoadSTL 쓰레드 수행함수
@return 없음
*/

#include "stdafx.h"
#include "WorkLoadSTL.h"
#include "windowManager.h"
#include "ActionManager.h"

WorkLoadSTL::WorkLoadSTL(DataContext* pDataContext, QStringList& _list_files)
{
	m_list_files = _list_files;
	m_pDataContext = pDataContext;
}

void WorkLoadSTL::threadRun()
{
	if (m_pDataContext == nullptr)
	{
		emit sig_updateProgress(100, QString("Final Process"));

		emit finished();
		return;
	}

	int n_list = m_list_files.length();

	std::vector<mint32> vt_layer_uid(n_list, -1);

	std::vector<QString> vt_file_name(n_list);

	std::vector<mip::MeshTopology*> vt_topology(n_list, NULL);

	int progress_val = 0;
	int interval = 90 / m_list_files.length();

	WIN_MANAGER->setRenderable(true);

	for (int i = 0; i < m_list_files.length(); ++i)
	{
		if (m_pDataContext->volume_data.threadStop)
		{
			emit finished();
			return;
		}

		emit sig_updateProgress(progress_val, m_list_files[i]);

		WIN_MANAGER->loadFiles(m_list_files[i]);

		int idx = m_pDataContext->m_MeshData.GetMeshCount() - 1;
		mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetMesh(idx);

		if (p_mesh)
		{
			vt_topology[i] = p_mesh;

			//WIN_MANAGER->moveMeshCenterScene(i);
		}

		QString temp_name = m_list_files[i];
		temp_name.chop(4);

		vt_file_name[i] = temp_name;

		vt_layer_uid[i] = -1;

		progress_val += interval;

		// update 
		emit sig_updateUI();

		WIN_MANAGER->setSaveState(false);

		WIN_MANAGER->MEViewRenderLater();
	}

	emit sig_updateProgress(90, QString("Final Process"));

	// push action undo / Redo
	ACTION_MANAGER->action_MeshList_add_Multi(m_pDataContext, vt_layer_uid, vt_file_name, vt_topology);

	//MESH_MODELVIEW_MANAGER->MoveScreenCenterMesh();

	emit sig_updateProgress(100, QString("Final Process"));

	emit finished();
}
