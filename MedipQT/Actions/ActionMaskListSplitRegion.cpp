#include "stdafx.h"
#include "ActionMaskListSplitRegion.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
//#include "qfile.h"
#include "ActionRegionGrowing.h"
#include "ActionManager.h"

ActionMaskListSplitRegion::ActionMaskListSplitRegion(VOLUME_DATA* pVolumeData, sWorkMaskListSplitRegionResult &_resultData, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;
	static int s_id = ACT_ID_MASKLIST_SPLIT_REGION;
	m_id = s_id++;
	m_first = false;
	resultData = _resultData;
}

ActionMaskListSplitRegion::~ActionMaskListSplitRegion()
{
	resultData.pThreadResult.reset();
}

bool ActionMaskListSplitRegion::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListSplitRegion::undo()
{
	for (int i = (separateMaskVector.size()-1); i >= 0; i--)
		m_pVolumeData->delMaskInfo(separateMaskVector.at(i).first);

	m_pVolumeData->setCurrentMaskIndex(0);
	MaskInfo * info = m_pVolumeData->getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListSplitRegion::redo()
{
	muint32	curCreatedMaskIndex;
	if (m_first == false)
	{
		m_first = true; 
		
		for (int i = 0; i < resultData.numberOfSplitRegion; i++)
		{
			MaskInfo* pNewMaskInfo = nullptr;
			bool r = m_pVolumeData->createMaskInfo();
			if (r)
			{
				curCreatedMaskIndex = m_pVolumeData->getCurrentMaskInfoID();
				pNewMaskInfo = m_pVolumeData->getCurrentMaskInfo();
				m_pVolumeData->setMaskName(resultData.maskName + QString("_%1").arg(i + 1), pNewMaskInfo->uid, true);
				//	pNewMaskInfo->color = COLOR(5, 144, 189, 255);
				separateMaskVector.push_back(QPair<muint32, MaskInfo>(curCreatedMaskIndex, *pNewMaskInfo));
			}
		}
	}
	else
	{
		for (int i=0; i<separateMaskVector.size(); i++)
			m_pVolumeData->insertMaskInfo(separateMaskVector.at(i).first, separateMaskVector.at(i).second);
	}

	int *result = nullptr;
	result = resultData.pThreadResult.get();

	for (int i = 0; i < separateMaskVector.size(); i++)
	{
		MaskInfo curMaskInfo = separateMaskVector.at(i).second;
		MaskInfo *pCurMaskInfo = &curMaskInfo;
		if (pCurMaskInfo)
		{
			int nByteIdx = m_pVolumeData->GetMaskByteIndex(pCurMaskInfo->uid);
			mask8 maskBit = m_pVolumeData->getMask(pCurMaskInfo->uid);
			mask8* pMaskData = m_pVolumeData->getMaskDataPoint(nByteIdx);

			//	volume_data.clearMaskData(maskBitCopy, nByteIdxCopy);

			int nMaskCount = 0;
			for (int j = 0; j < resultData.length; j++)
			{

				if (result[j] != 0 && result[j] == (i + 1))
				{
					//	outputData[i] |= maskBit;
					pMaskData[j] |= maskBit;
					++nMaskCount;
				}
			}

			m_pVolumeData->setVoxelCount(pCurMaskInfo->uid, nMaskCount, true);
			m_pVolumeData->updateUIDBoundingBox(pCurMaskInfo->uid, true);
			WIN_MANAGER->updateUI(true, separateMaskVector.at(i).first);
		}
	}

	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

void WorkMaskListSplitRegion::updateProgress(int val, void* dt)
{
	WorkMaskListSplitRegion* worker = (WorkMaskListSplitRegion*)dt;
	worker->setProgressValue(val);
}

void WorkMaskListSplitRegion::setProgressValue(int value, bool init)
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

void WorkMaskListSplitRegion::threadRun() // 2019.10.14 이두희 차장 추가
{
	setProgressValue(10);

	MaskInfo * info = m_pVolumeData->getCurrentMaskInfo();

	int uid = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0; // 0~3 (bin)			
	unsigned char* inputData = m_pVolumeData->getMaskDataPoint(uid);

	int maskIndex = uid == 0 ? info->mask_id : info->mask_id2; // maskIndex

	qDebug() << "WorkMaskListSplitRegion" << info->uid << " : " << QString().fromWCharArray(info->maskName);
	qDebug() << "WorkMaskListSplitRegion" << maskIndex << endl;

	muint32 dataCX = m_pVolumeData->getCX();
	muint32 dataCY = m_pVolumeData->getCY();
	muint32 dataCZ = m_pVolumeData->getCZ();
	muint32 length = dataCX*dataCY*dataCZ;// m_pVolumeData->getLength();

	unsigned char _ml = 0;
#if 0
	unsigned char *input = new unsigned char[length];
	int *output = new int[length];
	int *result = new int[length];
#else
	std::shared_ptr<unsigned char> pInput(new unsigned char[length], std::default_delete<unsigned char[]>());
	std::shared_ptr<int> pOutput(new int[length], std::default_delete<int[]>());
	std::shared_ptr<int> pResult(new int[length], std::default_delete<int[]>());
	unsigned char *input = pInput.get();
	int *output = pOutput.get();
	int *result = pResult.get();
#endif

	setProgressValue(20);

	for (int i = 0; i < length; i++)
	{
		if (inputData[i] & maskIndex)
		{
			input[i] = 1;
		}

		else input[i] = 0;
		output[i] = 0;
	}

	int res = 0;
	res = mip::LabelImage(dataCX, dataCY, dataCZ, input, output);

	int availableMaskCount = MASK_MAX - m_pVolumeData->getMaskInfoListCnt();
	if (!res || res == 1 || res > availableMaskCount)
	{
		pResult.reset();
		pInput.reset();
		pOutput.reset();
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	//	writeRawFile(output, length, QString("D:\\output.raw"));
// 	std::vector<int> a, b;
// 	for (int i = 0; i < 100000000; ++i)
// 	{
// 		a.push_back(i);
// 		if (i % 500000 == 0)
// 			qDebug() << " sleep1";
// 	}
	setProgressValue(50);
	mip::SortLabelingMask(output, result, dataCX, dataCY, dataCZ, res);
	//	writeRawFile(result, length, QString("D:\\result.raw"));

	//int m_n_edit = m_editComponent->text().toInt();
	
// 	for (int i = 0; i < 100000000; ++i)
// 	{
// 		b.push_back(i);
// 		if (i % 500000 == 0)
// 			qDebug() << " sleep2";
// 	}
	
	if (!ACTION_MANAGER->m_qThreadNext.empty())
	{
		sWorkMaskListSplitRegionResult resultData;
		resultData.maskName = QString::fromWCharArray(info->maskName);
		resultData.length = length;
		resultData.numberOfSplitRegion = res;
		resultData.pThreadResult = std::move(pResult);
		ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<sWorkMaskListSplitRegionResult>(resultData);
	}

#if 0
	delete[]result;
	delete[]input;
	delete[]output;
#else
	pResult.reset();
	pInput.reset();
	pOutput.reset();
#endif

	setProgressValue(100);

	m_pVolumeData->threadResult = 1;
	emit finished();
}
