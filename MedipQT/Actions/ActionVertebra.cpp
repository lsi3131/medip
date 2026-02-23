#include "stdafx.h"
#include "ActionVertebra.h"

#include "System/stringManager.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/VolumeView.h"
#include "Windows/AnalMPRPlaneView.h"

#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"

//#include "ActionRegionGrowing.h"
#include "Actions/ActionManager.h"





WorkCalculateSlicePostion::WorkCalculateSlicePostion(VOLUME_DATA* pVolumeData, std::vector<int> VbNumList, int selectType, float cutOffset)
{
//SUPPORT_DEEPCATCH_VERSION_2
	m_cutOffset = cutOffset;
	m_VbNumList = VbNumList;
	m_SelectType = selectType;

	m_pVolumeData = pVolumeData;
}

void WorkCalculateSlicePostion::setProgressValue(int value, bool init)
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


void WorkCalculateSlicePostion::threadRun()
{
//SUPPORT_DEEPCATCH_VERSION_2
	qDebug() << "<<<<<<<<<<<<<<<<<<<< Mask Cutting thread Run";
	///////////////////////////////////// make input data

	int size = m_VbNumList.size();
	std::vector<int> SliceNum;
	for (int ii = 0; ii < m_VbNumList.size(); ii++)
	{
		QString str = getVBString(m_VbNumList[ii]);		
		MaskInfo* pMaskInfo = m_pVolumeData->findMaskInfo(str);
		MaskInfo* pMaskInfo_dup = nullptr;
		QString strDup;
		int duplicationMax = DEEPCATCH_DUPLICATION_MAX;
		for (int jj = 0; jj < duplicationMax; jj++)
		{
			strDup = str + "-dup" + QString::number(jj);
			pMaskInfo_dup = m_pVolumeData->findMaskInfo(strDup);
			if (pMaskInfo_dup != nullptr)
			{
				pMaskInfo = pMaskInfo_dup;
				str = strDup;
				break;
			}
		}
		

		if (pMaskInfo == nullptr)
		{
			m_pVolumeData->threadResult = 1;
			//QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INIT_FAIL), "Theres is not Vertebra Mask.");
			ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(SliceNum);
			emit finished();
			return;
		}
		
		//if (size == 1)
		if (m_SelectType == ST_SINGLE)
		{
			int pos = getSlicePostion(str, eSPCT_Mid);
			SliceNum.push_back(pos);

			//printf_s("\n pos - %d ", pos);
		}
			
		else if (m_SelectType == ST_MULTIPLE)
		{
			int pos = -1;
			if(ii == eSPCT_Low)
				pos = getSlicePostion(str, eSPCT_Low);
			else
				pos = getSlicePostion(str, eSPCT_Up);

			SliceNum.push_back(pos);
		}			
	}

	if (m_SelectType == ST_SINGLE)
	{
		if(SliceNum.size() > 0)
			ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<int>(SliceNum[ST_SINGLE]);	
		else
			ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<int>(0);
	}
	else if (m_SelectType == ST_MULTIPLE)	
	{
		ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(SliceNum);
	}

	setProgressValue(100);
	bool res = true;
	res = true;	

	if (!res)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		m_pVolumeData->threadResult = 1;
	}

	emit finished();
	return;
}

int WorkCalculateSlicePostion::getLowestSagittalX(unsigned char * pMask3D, mask maskBit, int cx, int cy, int cz)
{
	int _sagittal_depth = INT_MAX;
	int _sagittal_x		= INT_MAX;
	int _sagittal_y		= INT_MAX;
	for (int i = 0; i < cz; i++)
	{
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				int idx = i*cy*cx + y*cx + x;
				if (pMask3D[i*cy*cx + y*cx + x] & maskBit)
				{
					if (i < _sagittal_y)
					{
						_sagittal_y = i;
					}

					if (x < _sagittal_depth)
					{
						_sagittal_depth = x;
					}

					if (y < _sagittal_x)
					{
						_sagittal_x = y;
					}
				}
			}
		}
	}

	return _sagittal_x;
}

int WorkCalculateSlicePostion::getSlicePostion(QString strMaskName, int returnType)
{
	qDebug() << "<<<<<<<<<<<<<<<<<<<< Mask Cutting thread Run";
	setProgressValue(0, true);

	muint32 cx, cy, cz;
	m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	//추가
	MaskInfo* pMaskInfo = m_pVolumeData->findMaskInfo(strMaskName);
	if (pMaskInfo == nullptr)
	{		
		return -1;
	}

	//mint16 *maskRaw = new mint16[cx*cy];
	qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);

	pMaskInfo->uid;
	m_pVolumeData->getCurrentMaskInfo()->uid;

	int byteIdx = m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid);
	unsigned char* pMask3D = m_pVolumeData->getMaskDataPoint(byteIdx);
	mask maskBit = m_pVolumeData->getMask(pMaskInfo->uid);
	BoundingBoxI box = m_pVolumeData->getBoundingBox(pMaskInfo->uid);

	bool res = false;
	int lowestSagittal_X_Idx = getLowestSagittalX(pMask3D, maskBit, cx, cy, cz);
	if (lowestSagittal_X_Idx == INT_MAX)
	{
		return -1;
	}

	int _start = box.getLimitZ() - box.getMaxZ() - 1;
	int _end = box.getLimitZ() - box.getMinZ() - 1;

	bool bDelete = false;

	int maxI = 0; int minI = 999;
	float _3cmOffset = 30.7f;
	for (int i = 0; i < cz; i++)
	{
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				int idx = i*cy*cx + y*cx + x;
				if (pMask3D[i*cy*cx + y*cx + x] & maskBit)
				{
					if (y > lowestSagittal_X_Idx + _3cmOffset)
					{
						//if (bDelete)
							//pMask3D[i*cy*cx + y*cx + x] &= ~maskBit;					
					}
					else
					{
						maxI = i > maxI ? i : maxI;
						minI = i < minI ? i : minI;
					}
				}
			}
		}

		setProgressValue(((float)(i) / _end) * 20);

		//qDebug() << "mask data save complete";

		if (m_pVolumeData->threadStop)
		{
			//SAFE_DELETES(maskRaw);
			emit finished();
			return -1;
		}
	}
	int MidI = (maxI + minI) / 2;

	int resultI = -1;
	if (returnType == eSPCT_Low)
		resultI = minI;
	else if (returnType == eSPCT_Up)
		resultI = maxI;
	else if (returnType == eSPCT_Mid)
		resultI = MidI;
	
	resultI = cz- resultI;
	
	//printf_s("\n MIDY - %d", MidI);
	//printf_s("\n maxY - %d", maxI);
	//printf_s("\n minY - %d", minI);
	return resultI;	
}

QString WorkCalculateSlicePostion::getVBString(int vbNum)
{
	switch (vbNum)
	{
	case eDCVB_T1:
		return QString(DEEPCATCH_VB_MASKNAME_T1);
		break;
	case eDCVB_T2:
		return QString(DEEPCATCH_VB_MASKNAME_T2);
		break;
	case eDCVB_T3:
		return QString(DEEPCATCH_VB_MASKNAME_T3);
		break;
	case eDCVB_T4:
		return QString(DEEPCATCH_VB_MASKNAME_T4);
		break;
	case eDCVB_T5:
		return QString(DEEPCATCH_VB_MASKNAME_T5);
		break;
	case eDCVB_T6:
		return QString(DEEPCATCH_VB_MASKNAME_T6);
		break;
	case eDCVB_T7:
		return QString(DEEPCATCH_VB_MASKNAME_T7);
		break;
	case eDCVB_T8:
		return QString(DEEPCATCH_VB_MASKNAME_T8);
		break;
	case eDCVB_T9:
		return QString(DEEPCATCH_VB_MASKNAME_T9);
		break;
	case eDCVB_T10:
		return QString(DEEPCATCH_VB_MASKNAME_T10);
		break;
	case eDCVB_T11:
		return QString(DEEPCATCH_VB_MASKNAME_T11);
		break;
	case eDCVB_T12:
		return QString(DEEPCATCH_VB_MASKNAME_T12);
		break;
	case eDCVB_L1:
		return QString(DEEPCATCH_VB_MASKNAME_L1);
		break;
	case eDCVB_L2:
		return QString(DEEPCATCH_VB_MASKNAME_L2);
		break;
	case eDCVB_L3:
		return QString(DEEPCATCH_VB_MASKNAME_L3);
		break;
	case eDCVB_L4:
		return QString(DEEPCATCH_VB_MASKNAME_L4);
		break;
	case eDCVB_L5:
		return QString(DEEPCATCH_VB_MASKNAME_L5);
		break;
	default:
		break;
	}	
	return nullptr;
}


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------

WorkFindPickMaskName::WorkFindPickMaskName(VOLUME_DATA* pVolumeData, vector<QString> & _strMaskNameList, float cutOffset)
{
	m_cutOffset = cutOffset;
	m_strMaskNameList = &_strMaskNameList;
	m_pVolumeData = pVolumeData;
}

void WorkFindPickMaskName::setProgressValue(int value, bool init)
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


void WorkFindPickMaskName::threadRun()
{
	qDebug() << "<<<<<<<<<<<<<<<<<<<< Find Pick Mask Name";

	muint32 cx, cy, cz;
	m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);
	setProgressValue(0, true);
	///////////////////////////////////// make input data

	std::vector<QUndoCommand*> reservationWorkList = ACTION_MANAGER->GetReservationWorkList();

	qDebug() << "reservationWorkList.size() = " << reservationWorkList.size();

	//추가
	MaskInfo* pMaskInfo = m_pVolumeData->findMaskInfo(m_strMaskName);
	if (pMaskInfo == nullptr)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	//mint16 *maskRaw = new mint16[cx*cy];
	qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);

	pMaskInfo->uid;
	m_pVolumeData->getCurrentMaskInfo()->uid;

	int byteIdx = m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid);
	unsigned char* pMask3D = m_pVolumeData->getMaskDataPoint(byteIdx);
	mask maskBit = m_pVolumeData->getMask(pMaskInfo->uid);
	BoundingBoxI box = m_pVolumeData->getBoundingBox(pMaskInfo->uid);

	bool res = false;
	int _start = box.getLimitZ() - box.getMaxZ() - 1;
	int _end = box.getLimitZ() - box.getMinZ() - 1;

	float _3cmOffset = 30.7f;
	for (int i = 0; i < cz; i++)
	{
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				int idx = i*cy*cx + y*cx + x;
				if (pMask3D[i*cy*cx + y*cx + x] & maskBit)
				{
					//if (y > lowestSagittal_X_Idx + _3cmOffset)
					{
						pMask3D[i*cy*cx + y*cx + x] &= ~maskBit;
					}
				}
			}
		}

		setProgressValue(((float)(i) / _end) * 20);

		//qDebug() << "mask data save complete";

		if (m_pVolumeData->threadStop)
		{
			//SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}
	}

	qDebug() << ">>>>>>>>>>>>> action Cut Bone start";
	qDebug() << "m_start" << _start;
	qDebug() << "m_end" << _end;
	qDebug() << "cx" << cx;
	qDebug() << "cy" << cy;
	qDebug() << "cz" << cz;
	qDebug() << ">>>>>>>>>>>>> action Cut Bone  end";


	m_pVolumeData->setVoxelCount(pMaskInfo->uid, m_pVolumeData->fillMaskCount, false);
	m_pVolumeData->updateUIDBoundingBox(pMaskInfo->uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->applyVoxelToUI(pMaskInfo->uid);
	WIN_MANAGER->setSaveState(false);


	m_strMaskNameList->push_back(QString("TEST"));

	QString _maskname = QString::fromWCharArray(pMaskInfo->maskName);
	//ACTION_MANAGER->m_qThreadNext.front().pTempData = make_shared<QString>(_maskname);

	//auto strData = std::static_pointer_cast<std::string>(ACTION_MANAGER->m_qThreadNext.front().pTempData);
	//QString strFileName = QString::fromStdString(*strData);

	res = true;
	//SAFE_DELETES(maskRaw);

	if (!res)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		m_pVolumeData->threadResult = 1;
	}

	emit finished();
	return;
}

