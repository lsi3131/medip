#include "stdafx.h"
#include "WorkSaveJSON.h"

#include "windowManager.h"
#include "LicenseManager.h"

WorkSaveJSON::WorkSaveJSON(const QString& strFileName, QVector<QPair<QString, QString>>& _clinicalInfo, QVector<sLocalClinicalInfo>& _localClinicalInfo, VOLUME_DATA* pVolumeData)
{
	m_pVolumeData = pVolumeData;
	clinicalInfo = _clinicalInfo;
	localClinicalInfo = _localClinicalInfo;
	_FileName = strFileName;
}

void WorkSaveJSON::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveJSON::threadRun()
{
	setProgressValue(20);

	if (m_pVolumeData->isValidate() == false)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	QVector<muint32> maskUidVector;
	QVector<sMaskCoordinateArrayData> maskCoordianteVector;
	for (int n = 0; n < m_pVolumeData->getMaskInfoListCnt(); n++)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(n);
		if (info)
		{
			maskUidVector.push_back(info->uid);
			sMaskCoordinateArrayData arrayData;
			arrayData.maskName = QString::fromWCharArray(info->maskName);
			maskCoordianteVector.push_back(arrayData);
		}
	}

	setProgressValue(40);

	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();
	for (int z = 0; z < cz; z++)
	{
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z * cx * cy + y * cx + x;

				for (int i = 0; i < maskUidVector.size(); i++)
				{
					mask* pMask3D = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(maskUidVector.at(i)));
					mask maskBit = m_pVolumeData->getMask(maskUidVector.at(i));
					if (pMask3D[idx] & maskBit)
					{
						sMaskCoordinate data{ x, y, z };
						maskCoordianteVector[i].curMaskCoordinateVec.push_back(data);
					}
				}
			}
		}
		if (m_pVolumeData->threadStop)
		{
			emit finished();
			return;
		}
	}

	setProgressValue(70);

	/* json structure write */
	QJsonObject jsonObj;

	// user_id
	jsonObj.insert("user_id", LICENSE_DATA->getAuthenticatedID());

	// case_id
	QString strLatestOpenFileName;
	if (WIN_MANAGER->lastestPathGet(strLatestOpenFileName))
	{
		QFileInfo fi(strLatestOpenFileName);
		QString fileName = fi.fileName();
		jsonObj.insert("case_id", fileName.left(fileName.lastIndexOf(".")));
	}
	// roi list
	QJsonObject roiListObject;
	for (int i = 0; i < maskCoordianteVector.size(); i++)
	{
		QJsonArray coordinateArray;
		for (int j = 0; j < maskCoordianteVector.at(i).curMaskCoordinateVec.size(); j++)
		{
			sMaskCoordinate data = maskCoordianteVector.at(i).curMaskCoordinateVec.at(j);
			QJsonObject coordinate;
			coordinate.insert("x", data.x);
			coordinate.insert("y", data.y);
			coordinate.insert("z", data.z);
			coordinateArray.push_back(coordinate);

			if (m_pVolumeData->threadStop)
			{
				emit finished();
				return;
			}
		}
		roiListObject.insert(maskCoordianteVector.at(i).maskName, coordinateArray);
	}
	jsonObj.insert("roi_list", roiListObject);

	setProgressValue(80);

	// clinical info.
	if (!clinicalInfo.isEmpty())
	{
		QJsonObject ClinicalInfoObject;
		for (int i = 0; i < clinicalInfo.size(); i++)
			ClinicalInfoObject.insert(clinicalInfo.at(i).first, clinicalInfo.at(i).second);
		jsonObj.insert("Clinical Information (Global)", ClinicalInfoObject);
	}

	// local clinical info.
	if (!localClinicalInfo.isEmpty())
	{
		QJsonObject localObj_Layer;
		for (int i = 0; i < localClinicalInfo.size(); i++)
		{
			QJsonObject localObj_Attribute;
			sLocalClinicalInfo curData = localClinicalInfo.at(i);
			for (int j = 0; j < curData.data.size(); j++)
				localObj_Attribute.insert(curData.data.at(j).first, curData.data.at(j).second);
			localObj_Layer.insert(curData.maskName, localObj_Attribute);
		}
		jsonObj.insert("Clinical Information (Local)", localObj_Layer);
	}

	if (m_pVolumeData->threadStop)
	{
		emit finished();
		return;
	}

	setProgressValue(90);

	QFile saveFile(_FileName);

	if (!saveFile.open(QIODevice::WriteOnly)) {
		qInfo("Couldn't open file.");
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	//	QJsonArray recordsArray;
	//	recordsArray.push_back(recordObject);
	QJsonDocument saveDoc(jsonObj);
	//	qDebug() << saveDoc.toJson();

	QByteArray saveByteArr = saveDoc.toJson();

	// byte array 가공 작업.(x,y,z를 한줄로 표시)
#if 0	// qt 자료 구조 사용
	int findPos = 0;
	int findPos2 = 0;
	int length = 0;
	QVector<QPair<QByteArray, QByteArray>> findStrVector;
	findStrVector.push_back(QPair<QByteArray, QByteArray>(QByteArray("\"x\":"), QByteArray("{")));
	findStrVector.push_back(QPair<QByteArray, QByteArray>(QByteArray("\"y\":"), QByteArray(",")));
	findStrVector.push_back(QPair<QByteArray, QByteArray>(QByteArray("\"z\":"), QByteArray(",")));
	findStrVector.push_back(QPair<QByteArray, QByteArray>(QByteArray("}"), QByteArray("\n")));
	bool bFinished = false;
	while (true)
	{
		for (int i = 0; i < findStrVector.size(); i++)
		{
			findPos = saveByteArr.indexOf(findStrVector.at(i).first, findPos2);
			if (findPos != -1)
			{
				findPos2 = saveByteArr.lastIndexOf(findStrVector.at(i).second, findPos);
				findPos--;
				if (i != 3)
					findPos2++;
				length = (findPos - findPos2) + 1;
				saveByteArr.replace(findPos2, length, QByteArray(" "));
			}
			else
			{
				bFinished = true;
				break;
			}
		}
		if (bFinished)
			break;
	}
#elif 0	// std로 자료 구조 변경.
	int findPos = 0;
	int findPos2 = 0;
	int length = 0;
	std::vector<std::pair<QByteArray, QByteArray>> findStrVector;
	findStrVector.push_back(std::pair<QByteArray, QByteArray>(QByteArray("\"x\":"), QByteArray("{")));
	findStrVector.push_back(std::pair<QByteArray, QByteArray>(QByteArray("\"y\":"), QByteArray(",")));
	findStrVector.push_back(std::pair<QByteArray, QByteArray>(QByteArray("\"z\":"), QByteArray(",")));
	findStrVector.push_back(std::pair<QByteArray, QByteArray>(QByteArray("}"), QByteArray("\n")));
	bool bFinished = false;
	while (true)
	{
		for (int i = 0; i < findStrVector.size(); i++)
		{
			findPos = saveByteArr.indexOf(findStrVector.at(i).first, findPos2);
			if (findPos != -1)
			{
				findPos2 = saveByteArr.lastIndexOf(findStrVector.at(i).second, findPos);
				findPos--;
				if (i != 3)
					findPos2++;
				length = (findPos - findPos2) + 1;
				saveByteArr.replace(findPos2, length, QByteArray(" "));

				//	qInfo() << QString("saveByteArr[%1] : ").arg(findPos2) << saveByteArr.mid(findPos2, length) << ", next : " << saveByteArr.mid(findPos2 + length + 1, 10);
			}
			else
			{
				bFinished = true;
				break;
			}
		}
		if (bFinished)
			break;
	}
#elif 0	// 다른 자료 구조 변경
	struct PairInfo
	{
		QByteArray a;
		QByteArray b;
	};
	int findPos = 0;
	int findPos2 = 0;
	int length = 0;
	QVector<PairInfo> findStrVector;
	PairInfo first{ QByteArray("\"x\":"), QByteArray("{") };
	PairInfo second{ QByteArray("\"y\":"), QByteArray(",") };
	PairInfo third{ QByteArray("\"z\":"), QByteArray(",") };
	PairInfo fifth{ QByteArray("}"), QByteArray("\n") };
	findStrVector.push_back(first);
	findStrVector.push_back(second);
	findStrVector.push_back(third);
	findStrVector.push_back(fifth);
	bool bFinished = false;
	while (true)
	{
		for (int i = 0; i < findStrVector.size(); i++)
		{
			findPos = saveByteArr.indexOf(findStrVector.at(i).a, findPos2);
			if (findPos != -1)
			{
				findPos2 = saveByteArr.lastIndexOf(findStrVector.at(i).b, findPos);
				findPos--;
				if (i != 3)
					findPos2++;
				length = (findPos - findPos2) + 1;
				saveByteArr.replace(findPos2, length, QByteArray(" "));

				//	qInfo() << QString("saveByteArr[%1] : ").arg(findPos2) << saveByteArr.mid(findPos2, length) << ", next : " << saveByteArr.mid(findPos2 + length + 1, 10);
			}
			else
			{
				bFinished = true;
				break;
			}
		}
		if (bFinished)
			break;
	}
#endif

	// 최종 가공된 byte array를 write.
	saveFile.write(saveByteArr);
	saveFile.close();

	WIN_MANAGER->setSaveState(true);
	m_pVolumeData->threadResult = 1;

	setProgressValue(100);

	emit finished();
}