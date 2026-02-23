#include "stdafx.h"
#include "WorkMeshToUSD.h"
#include "Windows/windowManager.h"
#include "Renderer/model.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Usd/mipUsdMesh.h"

WorkMeshToUSD::WorkMeshToUSD(MeshData* pMeshData, const std::vector<int>& indexList, const QString& filepath) :
	m_pMeshData(pMeshData),
	m_indexList(indexList),
	m_filePath(filepath)
{
}

void WorkMeshToUSD::setProgressValue(int value, bool init /*= false*/)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
	{
		return;
	}

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkMeshToUSD::threadRun()
{
	if (m_indexList.empty())
	{
		return;
	}

	mipUsdStage usdStage;
	if (usdStage.CreateNew(m_filePath.toStdString()) == false)
	{
		qWarning() << "fail to create stage : " << m_filePath;
		return;
	}

	for (int index : m_indexList)
	{
		MeshLayerData meshLayerData;
		if (m_pMeshData->TryGet(&meshLayerData, index))
		{
			usdStage.AddMesh(*meshLayerData.Data, *meshLayerData.Info);
		}
	}

	usdStage.Save();
	emit finished();
}
