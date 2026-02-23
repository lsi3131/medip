#include "stdafx.h"
#include "WorkSaveSplits.h"

WorkSaveSplits::WorkSaveSplits(VOLUME_DATA* pVolumeData, mask _m, int _mI, int uid, int spCount, QString filePath, int fileType)
{
	this->_m = _m;
	this->_mI = _mI;
	this->_uid = uid;
	this->_spCount = spCount;
	this->_filePath = filePath;
	this->_fileType = fileType;

	m_pVolumeData = pVolumeData;
};

void WorkSaveSplits::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveSplits::threadRun()
{
	int height = m_pVolumeData->getCY();
	int width = m_pVolumeData->getCX();
	int cnt = m_pVolumeData->getCZ();
	int length = m_pVolumeData->getVolumeDataLength();

	bool res = m_pVolumeData->createTempMaskData(true, -1, 1);

	if (!res)
	{
		emit finished();
		return;
	}

	mask* tmp = m_pVolumeData->getMaskTempDataPoint();
	mask* _mask = m_pVolumeData->getMaskTempDataPoint(1);

	setProgressValue(0, true);


	mask mVal = VM_MASK0;

	BoundingBoxI box = m_pVolumeData->getBoundingBox(_uid);

	if (!box.validateCheck())
		box.reset(width, height, cnt);

	QString fileName = _filePath + "/" + m_pVolumeData->getMaskName(_uid, true);

	if (0 == _fileType)
	{
		for (int seq = 0; seq < _spCount; seq++)
		{
			std::memset(_mask, 0, length);
			_addValue = 1;
			for (int z = box.minZ; z <= box.maxZ; z++)
			{
				for (int y = box.minY; y <= box.maxY; y++)
				{
					for (int x = box.minX; x <= box.maxX; x++)
					{
						int idx = (z * width * height) + (y * width) + x;

						if (idx < 0 || idx >= length) continue;

						int idx2 = ((cnt - (z + 1)) * width * height) + (y * width) + x;

						if (idx2 < 0 || idx2 >= length) continue;

						_mask[idx] = tmp[idx2] & mVal ? 1 : 0;
					}
				}

				if (m_pVolumeData->threadStop == true)
				{
					emit finished();
					return;
				}
			}

			float val = seq;
			val /= _spCount;
			val *= 100;

			setProgressValue(val);
			QString _FileName = fileName + QString(" Split%1.raw").arg(seq + 1);
			QString _TempName = _FileName;
			_TempName.append(".tmp_");

			QFile file(_TempName);

			if (file.exists())
			{
				res = file.remove();
				if (!res)
				{
					m_pVolumeData->threadResult = -1;
					emit finished();

					return;
				}
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

			mVal <<= 1;
		}
	}
	emit finished();
}