#include "stdafx.h"
#include "WorkSaveTXT.h"

WorkSaveTXT::WorkSaveTXT(VOLUME_DATA* pVolumeData, mask _m, int _mI, int _index, const QString& strFileName)
{
	m_pVolumeData = pVolumeData;
	this->_m = _m;
	this->_mI = _mI;
	this->_index = _index;
	_FileName = strFileName;
	_addValue = 0;
}

void WorkSaveTXT::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveTXT::threadRun()
{
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

	MaskInfo* info = m_pVolumeData->getMaskInfo(_index, true);
	BoundingBoxI box = m_pVolumeData->getBoundingBox(info->uid);// m_pVolumeData->boundingBoxROI[info->uid];
	QString content = QString("%1\t%2\t%3\r\n").arg(box.maxX).arg(box.maxY).arg(box.maxZ);
	unsigned char _mask;
	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int slice = m_pVolumeData->getCZ();

	_addValue = 0;
	file.write(content.toUtf8().constData());
	setProgressValue(0, true);

	for (int z = box.minZ; z <= box.maxZ; z++)
	{
		for (int y = box.minY; y <= box.maxY; y++)
		{
			for (int x = box.minX; x <= box.maxX; x++)
			{
				_mask = m_pVolumeData->getMaskData(z * height * width + y * width + x, _mI);
				if (_mask & _m)
				{
					content = QString("%1\t%2\t%3\r\n").arg(x).arg(y).arg(z);
					file.write(content.toUtf8().constData());
				}
			}
		}
		if ((((float)box.maxZ / 9) * _addValue) <= z)
		{
			_addValue++;
			setProgressValue(_addValue * 10);
		}

		if (m_pVolumeData->threadStop == true)
		{
			file.close();
			file.remove();
			emit finished();
			return;
		}
	}

	file.close();

	QFile orgFile(_FileName);

	bool res = true;

	if (orgFile.exists())
		res = orgFile.remove();

	if (res)
	{
		if (file.rename(_FileName))
		{
			m_pVolumeData->threadResult = TRUE;
		}
	}

	emit finished();
}
