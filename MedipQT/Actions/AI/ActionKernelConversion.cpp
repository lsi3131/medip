#include "stdafx.h"
#include "AI/ActionKernelConversion.h"

#include "Windows\windowManager.h"
#include "Actions\ActionManager.h"
#include "stringManager.h"

#include "Windows/Tabwindow.h"

#include "mart.h"

ActionKernelContinousApply::ActionKernelContinousApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& imgData01, const std::vector<mint16>& imgData02, float alpha, QUndoCommand* parent /*= NULL*/) :
	m_pVolumeData(pVolumeData),
	m_image_Origin(imgData01),
	m_image_Output(imgData02),
	m_alpha(alpha)
{	
	m_dataSize = m_pVolumeData->getVolumeDataLength();
	m_origin.reserve(m_dataSize);
	m_origin.resize(m_dataSize);
	memcpy((char*)m_origin.data(), (char*)m_pVolumeData->getHUDataPoint(), m_dataSize * sizeof(mint16));

}

void ActionKernelContinousApply::undo()
{
	memcpy((char*)m_pVolumeData->getHUDataPoint(), (char*)m_origin.data(), m_dataSize * sizeof(mint16));

	AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
	if (pKernelConversiontab)
	{
		pKernelConversiontab->SetImageData_Origin(m_image_origin_OldData);
		pKernelConversiontab->SetImageData_Output(m_image_output_OldData);

		if (m_image_origin_OldData.empty() || m_image_output_OldData.empty())
			pKernelConversiontab->SetVisibleInterpolation(false);
	}
	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionKernelContinousApply::redo()
{	
	AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
	if (pKernelConversiontab)
	{
		pKernelConversiontab->SetVisibleInterpolation(true);

		m_image_origin_OldData = pKernelConversiontab->GetImageData_Origin();
		m_image_output_OldData = pKernelConversiontab->GetImageData_Output();

		pKernelConversiontab->SetImageData_Origin(m_image_Origin);
		pKernelConversiontab->SetImageData_Output(m_image_Output);

		std::vector<mint16> result(m_dataSize, 0);

		for (unsigned int i = 0; i < m_dataSize; ++i)
		{
			result[i] = (1.f - m_alpha) * m_image_Origin[i] + m_alpha * m_image_Output[i];
		}

		memcpy(m_pVolumeData->getHUDataPoint(), result.data(), m_dataSize * sizeof(mint16));
	}
	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}


ActionKernelNeutralizationApply::ActionKernelNeutralizationApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& output_Data, float alpha, QUndoCommand* parent /*= NULL*/):
	m_pVolumeData(pVolumeData),
	m_Output_Data(output_Data),
	m_alpha(alpha)
{	
	m_dataSize = m_pVolumeData->getVolumeDataLength();
	m_origin.reserve(m_dataSize);
	m_origin.resize(m_dataSize);
	memcpy((char*)m_origin.data(), (char*)m_pVolumeData->getHUDataPoint(), m_dataSize * sizeof(mint16));

}

void ActionKernelNeutralizationApply::undo()
{
	memcpy((char*)m_pVolumeData->getHUDataPoint(), (char*)m_origin.data(), m_dataSize * sizeof(mint16));

	AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
	if (pKernelConversiontab)
	{
		pKernelConversiontab->SetImageData_Origin(m_origin_Old);
		pKernelConversiontab->SetImageData_Output(m_Output_Data_Old);

		if (m_origin_Old.empty() || m_Output_Data_Old.empty())
			pKernelConversiontab->SetVisibleInterpolation(false);
	}
	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionKernelNeutralizationApply::redo()
{
	AIKernelConversionTab* pKernelConversiontab = WIN_MANAGER->GetTab()->getAIKernelConversionTab();
	pKernelConversiontab->SetVisibleInterpolation(true);

	if (pKernelConversiontab)
	{
		m_origin_Old = pKernelConversiontab->GetImageData_Origin();
		m_Output_Data_Old = pKernelConversiontab->GetImageData_Output();

		pKernelConversiontab->SetImageData_Origin(m_origin);
		pKernelConversiontab->SetImageData_Output(m_Output_Data);

		std::vector<mint16> result(m_dataSize, 0);

		for (unsigned int i = 0; i < m_dataSize; ++i)
		{
			result[i] = (1.f - m_alpha) * m_origin[i] + m_alpha * m_Output_Data[i];
		}

		memcpy((char*)m_pVolumeData->getHUDataPoint(), (char*)result.data(), m_dataSize * sizeof(mint16));
	}

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

