#pragma once

#ifndef MASKINFO_HELPER_H
#define MASKINFO_HELPER_H

#include "volumedata.h"
#include "Windows\windowManager.h"
#include "ActionManager.h"
#include "Windows/Tabwindow.h"


class MaskInfoHelper
{

public:
	MaskInfoHelper(VOLUME_DATA* pVolumeData, MaskInfo* _maskInfo) {
		maskInfo = _maskInfo;
		m_pVolumeData = pVolumeData;
	}
private:
	MaskInfo* maskInfo;

	VOLUME_DATA* m_pVolumeData;

public:

	bool isValid() {
		if (maskInfo)
			return true;
		else
			return false;
	}

	int getMaskIndex() {
		int maskIndex = maskInfo->uid >= MASK_SECOND_MAX ? (maskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		return maskIndex;
	}

	unsigned char getMaskValue() {
		int maskIndex = maskInfo->uid >= MASK_SECOND_MAX ? (maskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		unsigned char maskValue = maskIndex == 0 ? maskInfo->mask_id : maskInfo->mask_id2;
		return maskValue;
	}

	unsigned char * getMaskVolume() {
		int maskIndex = maskInfo->uid >= MASK_SECOND_MAX ? (maskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		unsigned char *maskVolume = m_pVolumeData->getMaskDataPoint(maskIndex);
		return maskVolume;
	}

	QString getMaskName() {
		QString maskName = QString::fromWCharArray(maskInfo->maskName).trimmed();
		return maskName;
	}

	int getMaskArrayIndex() {
		int maskNumInArray = -1;
		int maskCount = m_pVolumeData->getMaskInfoListCnt();
		QString maskName = QString::fromWCharArray(maskInfo->maskName).trimmed();
		for (int i = 0; i < maskCount; i++)
		{
			MaskInfo *_maskInfo = m_pVolumeData->getMaskInfo(i, false);
			if (QString::fromStdWString(_maskInfo->maskName).compare(maskName, Qt::CaseInsensitive) == 0)
			{
				maskNumInArray = i;
				break;
			}
		}

		return maskNumInArray;
	}

	void updateVoxelCount() {
		int debugVoxel = this->calculateVoxelCount();
		m_pVolumeData->setVoxelCount(this->getUid(), debugVoxel, true);
		WIN_MANAGER->GetTab()->getROITab()->applyVoxel(this->getUid()); // index아니고 uid 사용이네..
	}

	bool generateNewMaskInfoWithLayer(QString maskName) {
		MaskInfo* previousCurrent = m_pVolumeData->getCurrentMaskInfo();
		ACTION_MANAGER->action_MaskList_add();
		// ACTION_MANAGER->action_MaskList_text_change(ROIList, ROIList->indexOfTopLevelItem(item), item->text(column));		
		MaskInfo* newCurrent = m_pVolumeData->getCurrentMaskInfo();
		ACTION_MANAGER->action_MaskList_text_change(WIN_MANAGER->GetTab()->getROITab()->ROIList, m_pVolumeData->getCurrentMaskInfoID(), maskName);
		if (previousCurrent == newCurrent) {
			this->maskInfo = nullptr;
			return false;
		}
		else {
			this->maskInfo = newCurrent;
			return true;
		}
	}

	void clearMaskVolume() {
		int cx = m_pVolumeData->getCX();
		int cy = m_pVolumeData->getCY();
		int cz = m_pVolumeData->getCZ();

		int countMaskVolume = 0;
		for (int z = 0; z < cz; z++) {
			for (int y = 0; y < cy; y++) {
				for (int x = 0; x < cx; x++) {

					int index = z*(cy*cx) + y*(cx)+x;
					if (this->getMaskVolume()[index] & this->getMaskValue()) {
						this->getMaskVolume()[index] &= ~this->getMaskValue();
					}

				}
			}
		}
	}


private:
	int getUid() {
		return maskInfo->uid;
	}

	int calculateVoxelCount() {
		int cx = m_pVolumeData->getCX();
		int cy = m_pVolumeData->getCY();
		int cz = m_pVolumeData->getCZ();

		int countMaskVolume = 0;
		for (int z = 0; z < cz; z++) {
			for (int y = 0; y < cy; y++) {
				for (int x = 0; x < cx; x++) {

					int index = z*(cy*cx) + y*(cx)+x;
					if (this->getMaskVolume()[index] & this->getMaskValue()) {
						countMaskVolume++;
					}
				}
			}
		}
		return countMaskVolume;
	}

};
#endif



