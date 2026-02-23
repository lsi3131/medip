#include "stdafx.h"
#include "WorkTXTOpen.h"

WorkTXTOpen::WorkTXTOpen(QString filename, VOLUME_DATA* volume_data)
{
	_strFilename = filename;
	_volumData = volume_data;
}

void WorkTXTOpen::setProgressValue(int value, bool init /*= false*/)
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

void WorkTXTOpen::threadRun()
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

		if (width <= _content.at(0).toInt() ||
			height <= _content.at(1).toInt() ||
			slice <= _content.at(2).toInt())
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

		int x, y, z;
		int fileSize = file.size();
		while (!in.atEnd())
		{
			QString _info = in.readLine();
			QStringList _content = _info.split("\t");

			x = y = z = 0;
			if (_content.size() != 3)
			{
				_volumData->threadResult = -1;
				file.close();
				emit finished();

				return;
			}

			x = _content.at(0).toInt();
			y = _content.at(1).toInt();
			z = _content.at(2).toInt();

			if (x >= width ||
				y >= height ||
				z >= slice)
			{
				_volumData->threadResult = -1;
				file.close();
				emit finished();

				return;
			}

			buff[(z * width * height) + (y * width) + x] = 1;

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