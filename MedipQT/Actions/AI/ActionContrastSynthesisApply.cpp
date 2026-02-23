#include "stdafx.h"
#include "ActionContrastSynthesisApply.h"
#include "Windows\windowManager.h"
#include "Windows\Tabwindow.h"
#include "System\stringManager.h"
#include "Actions\ActionManager.h"
#include "algorithm\Radiomics.h"

#include "DeepInsthink.h"
#include "Definitions.h"
#include "Metadata.h"
#include "Network/Network.h"
#include "LicenseManager.h"
#include "DataContext.h"

ActionContrastSynthesisApply::ActionContrastSynthesisApply(VOLUME_DATA* pVolumeData, const std::vector<mint16>& translatedData, QUndoCommand* parent) :
	m_pVolumeData(pVolumeData),
	m_translatedData(translatedData)
{
	int length = m_pVolumeData->getVolumeDataLength();
	int sizeOfByte = m_pVolumeData->getVolumeDataLength() * sizeof(mint16);
	m_originData.resize(length);
	memcpy(m_originData.data(), m_pVolumeData->getHUDataPoint(), sizeOfByte);
}

void ActionContrastSynthesisApply::undo()
{
	int sizeOfByte = m_pVolumeData->getVolumeDataLength() * sizeof(mint16);
	memcpy(m_pVolumeData->getHUDataPoint(), m_originData.data(), sizeOfByte);

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionContrastSynthesisApply::redo()
{
	int sizeOfByte = m_pVolumeData->getVolumeDataLength() * sizeof(mint16);
	memcpy(m_pVolumeData->getHUDataPoint(), m_translatedData.data(), sizeOfByte);

	WIN_MANAGER->forceUpdate2DViewData(true, false);
	WIN_MANAGER->renderLater_GridView(false);
}

