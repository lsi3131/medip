#include "stdafx.h"
#include "ActionImageCircumference.h"
#include "BoundingBox.h"
#include "ActionManager.h"

ActionImageCircumference::ActionImageCircumference()
	: QUndoCommand()
{

}

ActionImageCircumference::~ActionImageCircumference()
{

}

void ActionImageCircumference::undo()
{

}

void ActionImageCircumference::redo()
{

}

void WorkImageCircumference::setProgressValue(int value, bool init /*= false*/)
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

void WorkImageCircumference::threadRun()
{
	setProgressValue(0, true);
	mask *outputData = m_pMaskDataPointTmp;
	//mask *outputData = m_pVolData->getMaskTempDataPoint();
	muint32 cx = m_pVolData->getCX();
	muint32 cy = m_pVolData->getCY();
	muint32 cz = m_pVolData->getCZ();
	muint32 volumeSize = m_pVolData->getVolumeDataLength();
	BoundingBoxI workingRegion = m_pVolData->getBoundingBox();

	//>> Input
	int slicePosition = cz - m_nSliceNum -1;
	//<<


	//>> Output
	double circumNavelInch, circumNavelCm;
	double circumMidpointInch, circumMidpointCm;
	//<<

	//>> Prior
	int length = volumeSize;
	int width = cx;
	int height = cy;
	//<<
	unsigned char *midBuffer = new unsigned char[length];
	memset(midBuffer, 0, sizeof(mask)*length);

	//
	typedef struct _PointTag {
		int x;
		int y;
	}Point;


	typedef enum {
		RIGHT = 1, BOTTOM = 2, LEFT = 3, TOP = 4
	} Direction;


	int umbilicusPosition = slicePosition;

	setProgressValue(10);
	Point movingPoint;
	// Axial 영상으로 처리하므로, x의 위치는 x축의 중간값을 사용한다. 
	movingPoint.x = workingRegion.maxX - ((workingRegion.maxX - workingRegion.minX) / 2);
	//movingPoint.x = workingRegion.minX + ((workingRegion.maxX - workingRegion.minX) / 3);
	// x의 중간위치에서 y축 방향으로 내려와서, 만나는 첫 번째 지점(mask영역)을 사용한다.
	for (int nY = workingRegion.minY; nY <= workingRegion.maxY; nY++)
	{
		int indexVoxel = umbilicusPosition*width*height + nY*width + movingPoint.x;
		if (0 <= indexVoxel && indexVoxel < length)
		{
			if (m_pMaskDataPoint[indexVoxel] & m_maskBit)
			{
				movingPoint.y = nY;
				break;
			}
		}
	}


	Direction currentDirection = RIGHT;

	int maxLoop = 10000;
	int returnStartPoint = 4;
	int rightTurnCount = 0;

	std::vector<Point> contourTrace;
	Point startPoint = movingPoint;
	qDebug("startPoint : %d, %d", startPoint.x, startPoint.y);
	setProgressValue(30);
	while (maxLoop--)
	{
		Point P1, P2, P3;
		mask p1 = 0, p2 = 0, p3 = 0;

		qDebug("movingPoint : %d, %d", movingPoint.x, movingPoint.y);

		if (movingPoint.x == startPoint.x && movingPoint.y == startPoint.y) {
			returnStartPoint--;

			if (returnStartPoint == 0)
				break;
		}

		// clockwise
		switch (currentDirection) {
		case RIGHT:
			//   * 
			// o *
			//   *
			P1.x = movingPoint.x + 1; P1.y = movingPoint.y - 1;
			P2.x = movingPoint.x + 1; P2.y = movingPoint.y;
			P3.x = movingPoint.x + 1; P3.y = movingPoint.y + 1;
			break;
		case BOTTOM:
			//   o
			// * * *
			P1.x = movingPoint.x + 1; P1.y = movingPoint.y + 1;
			P2.x = movingPoint.x; P2.y = movingPoint.y + 1;
			P3.x = movingPoint.x - 1; P3.y = movingPoint.y + 1;
			break;
		case LEFT:
			// *
			// * o
			// *
			P1.x = movingPoint.x - 1; P1.y = movingPoint.y + 1;
			P2.x = movingPoint.x - 1; P2.y = movingPoint.y;
			P3.x = movingPoint.x - 1; P3.y = movingPoint.y - 1;
			break;
		case TOP:
			// * * *
			//   o
			P1.x = movingPoint.x - 1; P1.y = movingPoint.y - 1;
			P2.x = movingPoint.x; P2.y = movingPoint.y - 1;
			P3.x = movingPoint.x + 1; P3.y = movingPoint.y - 1;
			break;
		} 
		int maskDataPointIndex = (umbilicusPosition*width*height + P1.y*width + P1.x);
		if ( 0 <= maskDataPointIndex && maskDataPointIndex < length)
			p1 = m_pMaskDataPoint[umbilicusPosition*width*height + P1.y*width + P1.x] & m_maskBit;

		maskDataPointIndex = (umbilicusPosition*width*height + P2.y*width + P2.x);
		if (0 <= maskDataPointIndex && maskDataPointIndex < length)
			p2 = m_pMaskDataPoint[umbilicusPosition*width*height + P2.y*width + P2.x] & m_maskBit;

		maskDataPointIndex = (umbilicusPosition*width*height + P3.y*width + P3.x);
		if (0 <= maskDataPointIndex && maskDataPointIndex < length)
			p3 = m_pMaskDataPoint[umbilicusPosition*width*height + P3.y*width + P3.x] & m_maskBit;

		if (p1)
		{
			midBuffer[umbilicusPosition*width*height + movingPoint.y*width + movingPoint.x] |= m_maskBit;

			movingPoint = P1;
			int newDirection = currentDirection - 1;
			if (newDirection == 0)
				newDirection = 4;
			currentDirection = (Direction)newDirection;

			rightTurnCount = 0;
		}
		else if (p2)
		{
			midBuffer[umbilicusPosition*width*height + movingPoint.y*width + movingPoint.x] |= m_maskBit;

			movingPoint = P2;

			rightTurnCount = 0;
		}
		else if (p3)
		{
			midBuffer[umbilicusPosition*width*height + movingPoint.y*width + movingPoint.x] |= m_maskBit;

			movingPoint = P3;

			rightTurnCount = 0;
		}
		else
		{
			// right turn
			int newDirection = currentDirection + 1;
			if (newDirection == 5)
				newDirection = 0;
			currentDirection = (Direction)newDirection;

			rightTurnCount++;

			if (rightTurnCount == 4)
				break;
		}
	}

	setProgressValue(50);
	// retry
	movingPoint = startPoint;
	maxLoop = 10000;
	returnStartPoint = 1;
	while (maxLoop--)
	{
		Point P1, P2, P3;
		mask p1 = 0, p2 = 0, p3 = 0;

		if (contourTrace.size() != 0 && movingPoint.x == startPoint.x && movingPoint.y == startPoint.y) {
			returnStartPoint--;

			if (returnStartPoint == 0)
				break;
		}

		// clockwise
		switch (currentDirection) {
		case RIGHT:
			//   * 
			// o *
			//   *
			P1.x = movingPoint.x + 1; P1.y = movingPoint.y - 1;
			P2.x = movingPoint.x + 1; P2.y = movingPoint.y;
			P3.x = movingPoint.x + 1; P3.y = movingPoint.y + 1;
			break;
		case BOTTOM:
			//   o
			// * * *
			P1.x = movingPoint.x + 1; P1.y = movingPoint.y + 1;
			P2.x = movingPoint.x; P2.y = movingPoint.y + 1;
			P3.x = movingPoint.x - 1; P3.y = movingPoint.y + 1;
			break;
		case LEFT:
			// *
			// * o
			// *
			P1.x = movingPoint.x - 1; P1.y = movingPoint.y + 1;
			P2.x = movingPoint.x - 1; P2.y = movingPoint.y;
			P3.x = movingPoint.x - 1; P3.y = movingPoint.y - 1;
			break;
		case TOP:
			// * * *
			//   o
			P1.x = movingPoint.x - 1; P1.y = movingPoint.y - 1;
			P2.x = movingPoint.x; P2.y = movingPoint.y - 1;
			P3.x = movingPoint.x + 1; P3.y = movingPoint.y - 1;
			break;
		}

		int midBufferIndex = umbilicusPosition*width*height + P1.y*width + P1.x;
		if ( 0 <= midBufferIndex && midBufferIndex < length)
			p1 = midBuffer[umbilicusPosition*width*height + P1.y*width + P1.x];

		midBufferIndex = umbilicusPosition*width*height + P2.y*width + P2.x;
		if (0 <= midBufferIndex && midBufferIndex < length)
			p2 = midBuffer[umbilicusPosition*width*height + P2.y*width + P2.x];

		midBufferIndex = umbilicusPosition*width*height + P3.y*width + P3.x;
		if (0 <= midBufferIndex && midBufferIndex < length)
			p3 = midBuffer[umbilicusPosition*width*height + P3.y*width + P3.x];

		if (p1)
		{
			contourTrace.push_back(movingPoint);

			movingPoint = P1;
			int newDirection = currentDirection - 1;
			if (newDirection == 0)
				newDirection = 4;
			currentDirection = (Direction)newDirection;

			rightTurnCount = 0;
		}
		else if (p2)
		{
			contourTrace.push_back(movingPoint);

			movingPoint = P2;

			rightTurnCount = 0;
		}
		else if (p3)
		{
			contourTrace.push_back(movingPoint);

			movingPoint = P3;

			rightTurnCount = 0;
		}
		else
		{
			// right turn
			int newDirection = currentDirection + 1;
			if (newDirection == 5)
				newDirection = 0;
			currentDirection = (Direction)newDirection;

			rightTurnCount++;

			if (rightTurnCount == 4)
				break;
		}
	}

	setProgressValue(80);
	double xy = m_pVolData->getSpaceX();
	double diagonal = sqrt(xy*xy + xy*xy);
	double circumference = 0.0;
	if (!contourTrace.empty())
	{
		for (int i = 0; i < contourTrace.size() - 1; i++) {
			Point p1 = contourTrace[i];
			Point p2 = contourTrace[i + 1];

			int diff = abs(p1.x - p2.x) + abs(p1.y - p2.y);
			if (diff < 2) {
				circumference += xy;
			}
			else {
				circumference += diagonal;
			}
		}
	}
	double cmToinch = 0.393701; // cm * 0.393701 = inch
	qDebug() << "circumference : " << circumference << " cm";
	qDebug() << "circumference : " << circumference*cmToinch << " inch";

	// result
	circumNavelCm = circumference;
	circumNavelInch = circumference*cmToinch;

	if(!ACTION_MANAGER->m_qThreadNext.empty())
		ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<double>(circumNavelCm);

	// 
	memcpy(outputData, midBuffer, sizeof(mask)*length);
	
	//////////////// output 확인용
// 	QString strFileName = "circumfence.raw";
// 	QString strDataPath = "C:\\MedipTemp\\circumfence\\";
// 	QDir dir(strDataPath);
// 	if (!dir.exists())
// 		dir.mkdir(strDataPath);
// 
// 	//QString rawName = strDataPath + QString("/L3_predict_input.raw");
// 	QString rawName = strDataPath + strFileName;
// 	QFile file(rawName);
// 
// 	if (file.exists())
// 		file.remove();
// 
// 	if (!file.open(QIODevice::WriteOnly))
// 	{
// 		file.remove();
// 		return;
// 	}
// 
// 	//file.write((const char*)src.data, (cx * cz) * sizeof(mint16));
// 	file.write((const char*)&outputData[umbilicusPosition*width*height], (cx * cy) * sizeof(mint8));
// 	file.close();
	
	////////////////
	
	delete[] midBuffer;
	setProgressValue(100);

	emit finished();
}

void WorkFindAbdominalWaist::threadRun()
{	
	setProgressValue(0, true);
	// output
	int resultPosWaistStart = 0;
	int resultPosWaistEnd = 0;

	// constant
	constexpr double REMOVAL_RATIO = 0.18;
	constexpr double SEARCH_START_PERCENTILE = 0.1;
	constexpr double SEARCH_END_PERCENTILE = 0.9;
	const QString MASK_NAME = DEEPCATCH_WHOLEBODY_MASKNAME_BONE;

	//
	MaskInfo *boneMaskInfo = nullptr;

	muint32 cx = m_pVolData->getCX();
	muint32 cy = m_pVolData->getCY();
	muint32 cz = m_pVolData->getCZ();
	muint32 volumeSize = m_pVolData->getVolumeDataLength();

	// 
	int cntMasInfoList = m_pVolData->getMaskInfoListCnt();
	for (int i = 0; i < cntMasInfoList; i++)
	{
		MaskInfo *maskInfo = m_pVolData->getMaskInfo(i, false);
		if (QString::fromStdWString(maskInfo->maskName).compare(MASK_NAME) == 0)
		{
			boneMaskInfo = maskInfo;
			break;
		}
	}

	setProgressValue(40);
	if (boneMaskInfo)
	{
		MaskInfo *maskInfo = boneMaskInfo;
		int maskIndex = maskInfo->uid >= MASK_SECOND_MAX ? (maskInfo->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		unsigned char maskValue = maskIndex == 0 ? maskInfo->mask_id : maskInfo->mask_id2;
		mask* maskVolume = m_pVolData->getMaskDataPoint(maskIndex);

		// 		
		mask *tempBuffer = new mask[volumeSize];
		memset(tempBuffer, 0, volumeSize);

		// removing vertebral area		
		int centerX = cx / 2;
		int leftBound = centerX - (cx * REMOVAL_RATIO);
		int rightBound = centerX + (cx * REMOVAL_RATIO);
		for (int z = 0; z < cz; z++) {
			for (int y = 0; y < cy; y++) {
				for (int x = 0; x < cx; x++) {

					int indexVoxel = z*cx*cy + y*cx + x;

					if (x > leftBound && x < rightBound)
						continue;

					if (maskVolume[indexVoxel] & maskValue) {
						tempBuffer[indexVoxel] |= maskValue;
					}
				}
			}
		}

		setProgressValue(70);
		//
		int maxEmptySpace = 0;
		int cntEmptySlice = 0;
		int posStartEmptySlice, posEndEmptySlice;
		bool isEmptyContinue = false;
		for (int z = cz*SEARCH_START_PERCENTILE; z < cz*SEARCH_END_PERCENTILE; z++)
		{
			bool isExistBone = false;

			for (int y = 0; y < cy; y++) {
				for (int x = 0; x < cx; x++) {
					int indexVoxel = z*cx*cy + y*cx + x;
					if (tempBuffer[indexVoxel] & maskValue) {
						isExistBone = true;
						break;
					}
				}

				if (isExistBone)
					break;
			}

			//
			if (!isExistBone)
			{
				if (isEmptyContinue == false) {
					isEmptyContinue = true;
					cntEmptySlice = 0;
					posStartEmptySlice = z - 1;
				}
				cntEmptySlice++;
			}
			else
			{
				if (isEmptyContinue == true) {
					isEmptyContinue = false;
					posEndEmptySlice = z;
					if (cntEmptySlice > maxEmptySpace)
					{
						maxEmptySpace = cntEmptySlice;

						//
						resultPosWaistStart = posStartEmptySlice;
						resultPosWaistEnd = posEndEmptySlice;
					}
				}
			}
		}


		// delete
		delete[] tempBuffer;

		// (DEBUG)			
		qDebug() << "cz" << cz;
		qDebug() << "resultPosWaistStart" << resultPosWaistStart;
		qDebug() << "resultPosWaistEnd" << resultPosWaistEnd;
		qDebug() << "m_pVolData->getCZ() - 1 - resultPosWaistEnd;" << m_pVolData->getCZ() - 1 - resultPosWaistEnd;
		qDebug() << "m_pVolData->getCZ() - 1 - resultPosWaistStart;" << m_pVolData->getCZ() - 1 - resultPosWaistStart;

		std::vector<int>  vecWaistRange;
		//vecWaistRange.push_back(m_pVolData->getCZ() - 1 - resultPosWaistEnd);		// 위쪽, start
		//vecWaistRange.push_back(m_pVolData->getCZ() - 1 - resultPosWaistStart);	// 아랫쪽, end
		vecWaistRange.push_back(resultPosWaistEnd);		// 위쪽, start
		vecWaistRange.push_back(resultPosWaistStart);	// 아랫쪽, end

		if (!ACTION_MANAGER->m_qThreadNext.empty())
			ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<std::vector<int>>(vecWaistRange);

		m_pVolData->threadResult = vecWaistRange.size();

	}
	setProgressValue(100);


	emit finished();
}