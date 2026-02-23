#include "stdafx.h"
#include "WorkAIContrastSynthesis.h"
#include "Windows\windowManager.h"
#include "Windows\Tabwindow.h"
#include "System\stringManager.h"
#include "Actions\ActionManager.h"
#include "algorithm\Radiomics.h"

#include "DeepInsthink.h"
#include "Definitions.h"
#include "Metadata.h"
#include "Network/Network.h"
#include "LicenseManager.h"
#include "DataContext.h"

using namespace std;

const vector<vector<int>> WorkAIContrastSynthesis::MEDIP_CONTRAST_SYNTHESIS_TRANS_SALT =
{
	{
		546857975, 1206038495, 168341876, 705786705, 2088930087, 915972608, 346285530,
			352315002, 1062402511, 256550641, 1107713622, 335710866, 268138010, 1826114301, 1830976666, 791110495,
			1486281456, 1178250288, 1817403054, 1358959822, 1363576248, 317338534, 1709500397, 1502044173,
			566675089, 1017128345, 555790810, 1088649745, 292833056, 1326424169, 1562707811, 786226257,
	}
};

WorkAIContrastSynthesis::WorkAIContrastSynthesis(VOLUME_DATA* pVolumeData, ActionManager* pActionManager, eAIContrastSynthesisOption option, bool useGPU) :
	m_pVolumeData(pVolumeData),
	m_pActionManager(pActionManager),
	m_option(option),
	m_useGPU(useGPU)
{
	m_inputPredictDirPath = STRING_MANAGER->m_strLocalAITransPath + "/predict";
	m_inputRawSliceDirPath = m_inputPredictDirPath + "/data";
	m_outputPredictResultDirPath = m_inputPredictDirPath + "/output";
	m_AIWeightDirPath = STRING_MANAGER->AITranslationPath + "/weight";

	if (m_option == eAIContrastSynthesisOption::Adult)
	{
		m_AIWeightFileName = "Adt_N2C_v0";
	}
	else if (m_option == eAIContrastSynthesisOption::Pediatric)
	{
		m_AIWeightFileName = "Ped_N2C_v0";
	}
	else if (m_option == eAIContrastSynthesisOption::NonContrast)
	{
		m_AIWeightFileName = "Adt_C2N_v0";
	}
	else
	{
		qCritical() << "invalid option type : " << (int)m_option;
		Q_ASSERT(false);
	}
}

WorkAIContrastSynthesis::~WorkAIContrastSynthesis()
{
	qInfo() << "destructor WorkAIContrastSynthesis";
}

void WorkAIContrastSynthesis::threadRun()
{
	if (m_pVolumeData->isValidate() == false)
	{
		m_pVolumeData->threadResult = -1;
		return;
	}

	initWorkDirectory();

	if (saveRawSlice() == false)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	/* API 호출을 진행한다*/
	int addValue = 20;

	bool result = doPredict(addValue);

	if (!result)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}

	m_pVolumeData->threadResult = 1;

	InsertActionThreadResult(m_pActionManager, ACTP_AI_CONTRAST_SYNTHESIS_PREDICT, m_translatedData);

	emit finished();
}

void WorkAIContrastSynthesis::initWorkDirectory()
{
	QDir dir;

	QDir(m_inputPredictDirPath).removeRecursively();
	QDir(m_inputRawSliceDirPath).removeRecursively();
	QDir(m_outputPredictResultDirPath).removeRecursively();

	dir.mkpath(m_inputPredictDirPath);
	dir.mkpath(m_inputRawSliceDirPath);
	dir.mkpath(m_outputPredictResultDirPath);
}

bool WorkAIContrastSynthesis::saveRawSlice()
{
	muint32 cx, cy, cz;

	m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	int start = 0;
	int end = cz - 1;

	mint16* maskRaw = new mint16[cx * cy];
	bool result = true;

	for (int z = start; z <= end; z++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, z);
			}
		}

		QString saveRawSliceFilePath = m_inputRawSliceDirPath + QString("/Translation_slice_%1.raw").arg(z);
		QString tempRawSliceFilePath = saveRawSliceFilePath + ".tmp_";

		QFile tempRawSliceFile(tempRawSliceFilePath);

		if (tempRawSliceFile.exists())
		{
			tempRawSliceFile.remove();
		}

		if (!tempRawSliceFile.open(QIODevice::WriteOnly))
		{
			tempRawSliceFile.remove();
			result = false;
			break;
		}

		tempRawSliceFile.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		tempRawSliceFile.close();

		result = tempRawSliceFile.rename(saveRawSliceFilePath);

		if (m_pVolumeData->threadStop)
		{
			result = false;
		}

		if (!result)
		{
			break;
		}

		setProgressValue(((float)(z) / end) * 20);
	}

	SAFE_DELETES(maskRaw);

	return result;
}

bool WorkAIContrastSynthesis::doPredict(int addValue)
{
	Translation::setExtension("mipx");

	qInfo() << "weight dir path : " << m_AIWeightDirPath << ", weight file name : " << m_AIWeightFileName;

	//Translation translation(m_AIWeightDirPath.toStdString(), m_AIWeightFileName.toStdString(), false, MEDIP_TRANS_SALT);
	Translation translation(m_AIWeightDirPath.toStdString(), m_AIWeightFileName.toStdString(), true, MEDIP_CONTRAST_SYNTHESIS_TRANS_SALT);

	CheckData check;

	int milestone = addValue;
	bool result = true;
	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();

	if (translation.medipPredict(m_inputRawSliceDirPath.toStdString(), m_outputPredictResultDirPath.toStdString(), m_useGPU, cy, cx))
	{
		while (true)
		{
			if (m_pVolumeData->threadStop)
			{
				translation.medipStop();
			}
			check = translation.getCheckData();
			if (check.status == Status::NORMAL)
			{
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (addValue < progress)
				{
					addValue = progress;//send signal
					setProgressValue(addValue);
				}
			}
			else if (check.status == Status::STOPPING)
			{
				//printf("Stopping\n");
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		check = translation.getCheckData();
		result = false;
		qWarning() << "fail to WorkAIContrastSynthesis medipPredict. check status = " << check.status;
	}

	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		result = false;
	}

	AIBase::clearCache();

	if (!result)
	{
		return false;
	}

	vector<vector<mint16>> AIResult = translation.getVectorResults();
	if (AIResult.empty())
	{
		qWarning() << "fail to WorkAIContrastSynthesis. result empty";
		return false;
	}

	m_translatedData = AIResult.at(0);

	int resultDataLength = (int)m_translatedData.size();
	int volumeDataLength = m_pVolumeData->getVolumeDataLength();

	if (resultDataLength != volumeDataLength)
	{
		qWarning() << QString("result data length(%1) not equal to volume data length(%2)").arg(resultDataLength).arg(volumeDataLength);
		return false;
	}

	return true;
}
