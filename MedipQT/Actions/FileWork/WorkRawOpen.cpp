#include "stdafx.h"
#include "WorkRawOpen.h"

WorkRawOpen::WorkRawOpen(QString filename, VOLUME_DATA* volume_data, bool _reverse)
{
	_strFilename = filename;
	_volumData = volume_data;
	this->_reverse = _reverse;
}

void WorkRawOpen::setProgressValue(int value, bool init /*= false*/)
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

void WorkRawOpen::threadRun()
{
	setProgressValue(0, true);
	_addValue = 1;

	bool res = _volumData->createTempMaskData();

	if (!res)
	{
		emit finished();
		return;
	}
	res = false;

	if (_reverse)
	{
		res = _volumData->createTempMaskData(true, -1, 1);

		if (!res)
		{
			emit finished();
			return;
		}
	}

	int height = _volumData->getCY();
	int width = _volumData->getCX();
	int cnt = _volumData->getCZ();
	int length = _volumData->getVolumeDataLength();
	mask* buff = _volumData->getMaskTempDataPoint();
	mask* rebuff = NULL;

	if (_reverse)
		rebuff = _volumData->getMaskTempDataPoint(1);


	QFile file(_strFilename);
	if (!file.exists() || !file.open(QIODevice::ReadOnly))
	{
		emit finished();
		return;
	}

	file.read((char*)buff, length);

	file.close();


	if (_reverse)
	{
		for (int z = 0; z < cnt; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (buff[(_reverse ? cnt - z - 1 : z) * width * height + (y)*width + (x)] != 0)
					{
						rebuff[z * width * height + y * width + x] = 1;
					}
				}
			}
			if ((((float)cnt / 5) * _addValue) <= z)
			{
				setProgressValue(_addValue * 10);
				_addValue++;
			}
			if (_volumData->threadStop == true)
			{
				emit finished();
				return;
			}
		}

		std::memset(buff, 0, length);
	}
	else
	{
		rebuff = buff;
	}

	int voxel_cnt = 0;
	for (int i = 0; i < length; i++)
	{
		if (rebuff[i] != 0) buff[i] = 1;

		if ((((float)length / 10) * _addValue) <= i)
		{
			setProgressValue(_addValue * 10);
			_addValue++;
		}
		if (_volumData->threadStop == true)
		{
			emit finished();
			return;
		}
	}
	_volumData->threadResult = 1;

	emit finished();

	return;
}