#include "stdafx.h"
#include "WorkROIOpen.h"

WorkROIOpen::WorkROIOpen(QString filename, VOLUME_DATA* volume_data)
{
	_strFilename = filename;
	_volumData = volume_data;
}

void WorkROIOpen::setProgressValue(int value, bool init /*= false*/)
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

void WorkROIOpen::threadRun()
{
	setProgressValue(0, true);
	_addValue = 1;

	QFile file(_strFilename);
	if (!file.exists() || !file.open(QIODevice::ReadOnly | QFile::Text))
	{
		_volumData->threadResult = -1;
		emit finished();
		return;
	}

	QTextStream in(&file);

	QString _info = in.readLine();

	if (_info.length() != 0)
	{
		QStringList _content = _info.split("\t");

		if (_content.size() != 3)
		{
			_volumData->threadResult = -1;
			file.close();
			emit finished();

			return;
		}

		int width, height, slice;

		width = height = slice = 0;

		width = _volumData->getCX();
		height = _volumData->getCY();
		slice = _volumData->getCZ();

		if (width != _content.at(0).toInt() ||
			height != _content.at(1).toInt() ||
			slice != _content.at(2).toInt())
		{
			_volumData->threadResult = -1;
			file.close();
			emit finished();

			return;
		}

		bool res = _volumData->createTempMaskData();

		if (!res)
		{
			_volumData->threadResult = -1;
			file.close();

			emit finished();

			return;
		}

		mask* buff = _volumData->getMaskTempDataPoint();

		ushort sStart = 0;
		ushort sVal = 0;
		//		ushort rv = 0;
		long lCount = 0;
		int index = 0;
		int fileSize = file.size();
		while (!in.atEnd())
		{
			QString _info = in.readLine();
			QStringList _content = _info.split("\t");

			if (_content.size() != 2)
			{
				_volumData->threadResult = -1;
				file.close();
				emit finished();

				return;
			}

			sStart = _content.at(0).toUShort();
			lCount = _content.at(1).toLong();

			sVal = sStart / 10;
			//	rv = mValue % 10;

			for (int i = 0; i < lCount; i++)
			{
				if (sVal != 0)
				{
					buff[index] = 1;
				}
				index++;
			}

			if ((((float)in.pos() / fileSize) * 100) >= (_addValue * 10))
			{
				setProgressValue(_addValue * 10);
				_addValue++;
			}
			if (_volumData->threadStop == true)
			{
				file.close();
				emit finished();
				return;
			}
		}

		file.close();

		_volumData->threadResult = 1;
		emit finished();

		return;
	}
	else
	{
		file.close();

		emit finished();
		return;
	}

}