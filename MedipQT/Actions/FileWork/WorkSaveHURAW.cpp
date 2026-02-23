#include "stdafx.h"
#include "WorkSaveHURAW.h"

WorkSaveHURAW::WorkSaveHURAW(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName, bool patchy, BoundingBoxI box)
{
	m_pVolumeData = pVolumeData;
	this->_m = _m;
	this->_mI = _mI;
	_FileName = strFileName;
	_addValue = 0;
	_patchy = patchy;
	_box = box;
}

void WorkSaveHURAW::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveHURAW::threadRun()
{
	int height = m_pVolumeData->getCY();
	int width = m_pVolumeData->getCX();
	int cnt = m_pVolumeData->getCZ();

	bool res = m_pVolumeData->createTempHUData();

	if (!res)
	{
		emit finished();
		return;
	}
	setProgressValue(0, true);
	_addValue = 1;

	mint16* maskRaw = m_pVolumeData->pData3D_HU_Temp;

	bool resZ, resY, resX;

	resZ = resY = resX = false;

	for (int z = 0; z < cnt; z++)
	{
		if (_patchy)
		{
			if (_box.minZ <= z && z <= _box.maxZ)
				resZ = true;
			else
				resZ = false;
		}
		for (int y = 0; y < height; y++)
		{
			if (_patchy)
			{
				if (_box.minY <= y && y <= _box.maxY)
					resY = true;
				else
					resY = false;
			}

			for (int x = 0; x < width; x++)
			{
				if (!_patchy)
				{
					mask data = m_pVolumeData->getMaskData(x, y, z, _mI);
					if (data & _m)
						maskRaw[(cnt - z - 1) * height * width + y * width + x] = m_pVolumeData->getHUDataPoint()[z * height * width + y * width + x];
					else
						maskRaw[(cnt - z - 1) * height * width + y * width + x] = -3024;
				}
				else
				{
					if (_box.minX <= x && x <= _box.maxX)
						resX = true;
					else
						resX = false;

					if (resX && resY && resZ)
						maskRaw[(cnt - z - 1) * height * width + y * width + x] = m_pVolumeData->getHUDataPoint()[z * height * width + y * width + x];
					else
						maskRaw[(cnt - z - 1) * height * width + y * width + x] = -3024;
				}
			}
		}
		if ((((float)cnt / 10) * _addValue) <= z)
		{
			setProgressValue(_addValue * 10);
			_addValue++;
		}
		if (m_pVolumeData->threadStop == true)
		{
			emit finished();
			return;
		}
	}


	QString _TempName = _FileName;

	_TempName.append(".tmp_");

	QFile file(_TempName);

	if (file.exists())
	{
		m_pVolumeData->threadResult = -1;
		emit finished();

		return;
	}

	if (!file.open(QIODevice::WriteOnly))
	{
		file.remove();
		emit finished();
		return;
	}

	file.write((const char*)maskRaw, (width * height * cnt) * sizeof(short));
	file.close();

	QFile orgFile(_FileName);

	res = true;

	if (orgFile.exists())
		res = orgFile.remove();

	if (res)
	{
		if (file.rename(_FileName))
		{
			m_pVolumeData->threadResult = 1;
		}
	}

	emit finished();
}