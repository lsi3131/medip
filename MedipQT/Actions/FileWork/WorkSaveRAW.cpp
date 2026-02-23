#include "stdafx.h"
#include "WorkSaveRAW.h"

WorkSaveRAW::WorkSaveRAW(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName)
{
	m_pVolumeData = pVolumeData;
	this->_m = _m;
	this->_mI = _mI;
	_FileName = strFileName;
	_addValue = 0;
}

void WorkSaveRAW::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveRAW::threadRun()
{
	int height = m_pVolumeData->getCY();
	int width = m_pVolumeData->getCX();
	int cnt = m_pVolumeData->getCZ();

	bool res = m_pVolumeData->createTempMaskData();

	if (!res)
	{
		emit finished();
		return;
	}

	unsigned char* _mask = m_pVolumeData->pData3D_Mask_Temp;

	setProgressValue(0, true);
	_addValue = 1;

	for (int z = 0; z < cnt; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				_mask[z * height * width + y * width + x] = m_pVolumeData->getMaskData(x, y, cnt - (z + 1), _mI) & _m ? 1 : 0; // save 1 ~ 5
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

	file.write((const char*)_mask, (width * height * cnt));
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