#include "stdafx.h"
#include "ActionLowdoseReconstruction.h"
#include "stringManager.h"
#include "ActionManager.h"
#include "windowManager.h"
#include "Windows/Tabwindow.h"
#include "Tab/AILowdoseCTReconstuctionTab.h"

#include "mart.h"

ActionLowdoseReconstruction::ActionLowdoseReconstruction(VOLUME_DATA* pVolumeData, const std::vector<mint16>& output_Data, float alpha, QUndoCommand* parent /*= NULL*/) :
	m_pVolumeData(pVolumeData),
	m_Output_Data(output_Data),
	m_alpha(alpha)
{
	m_dataSize = m_pVolumeData->getVolumeDataLength();
	m_origin.reserve(m_dataSize);
	m_origin.resize(m_dataSize);
	memcpy((char*)m_origin.data(), (char*)m_pVolumeData->getHUDataPoint(), m_dataSize * sizeof(mint16));
}

ActionLowdoseReconstruction::~ActionLowdoseReconstruction()
{
}

void ActionLowdoseReconstruction::undo()
{
	memcpy((char*)m_pVolumeData->getHUDataPoint(), (char*)m_origin.data(), m_dataSize * sizeof(mint16));

	AILowdoseCTReconstuctionTab* pLowdoseRecontab = WIN_MANAGER->GetTab()->getAILowdoseCTReconTab();

	pLowdoseRecontab->SetImageData_Origin(m_origin_old);
	pLowdoseRecontab->SetImageData_Output(m_Output_OldData);

	if (m_origin_old.empty() || m_Output_OldData.empty())
		pLowdoseRecontab->SetVisibleInterpolation(false);

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionLowdoseReconstruction::redo()
{
	AILowdoseCTReconstuctionTab* pLowdoseRecontab = WIN_MANAGER->GetTab()->getAILowdoseCTReconTab();
	pLowdoseRecontab->SetVisibleInterpolation(true);

	m_origin_old = pLowdoseRecontab->GetImageData_Origin();
	m_Output_OldData = pLowdoseRecontab->GetImageData_Origin();

	pLowdoseRecontab->SetImageData_Origin(m_origin);
	pLowdoseRecontab->SetImageData_Output(m_Output_Data);

	std::vector<mint16> result(m_dataSize, 0);

	for (unsigned int i = 0; i < m_dataSize; ++i)
	{
		result[i] = (1.f - m_alpha) * m_origin[i] + m_alpha * m_Output_Data[i];
	}

	memcpy((char*)m_pVolumeData->getHUDataPoint(), (char*)result.data(), m_dataSize * sizeof(mint16));

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

