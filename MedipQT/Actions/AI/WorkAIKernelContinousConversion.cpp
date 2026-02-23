#include "stdafx.h"
#include "AI/WorkAIKernelContinousConversion.h"

#include "Windows/windowManager.h"
#include "Actions/ActionManager.h"
#include "stringManager.h"

#include "Windows/Tabwindow.h"

#include "mart.h"

WorkAIKernelContinousConversion::WorkAIKernelContinousConversion(VOLUME_DATA* pVolumeData, ActionManager* pActionManager) :
	m_pVolumeData(pVolumeData),
	m_pActionManager(pActionManager)
{
	setProgressValue(0, true);
	m_AIWeightDirPath = STRING_MANAGER->AIReconstructionPath;
	m_AIWeightFileName = QString("CT_Kernel_Cont.mipx");
}

WorkAIKernelContinousConversion::~WorkAIKernelContinousConversion()
{
	qInfo() << "WorkAIKernelContinousConversion deleted";
}

void WorkAIKernelContinousConversion::threadRun()
{
	if (m_pVolumeData->isValidate() == false)
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

	InsertActionThreadResult(m_pActionManager, ACTP_AI_KERNEL_CONTINOUS_CONVERSION_PREDICT, m_OutputData);

	emit finished();

}

bool WorkAIKernelContinousConversion::doPredict(int addValue)
{
	qInfo() << "weight dir path : " << m_AIWeightDirPath << ", weight file name : " << m_AIWeightFileName;


	Reconstruction recon(m_AIWeightDirPath.toStdString(), m_AIWeightFileName.toStdString());

	int volumeSize = m_pVolumeData->getVolumeDataLength();
	std::vector<mint16> intputData(m_pVolumeData->getVolumeDataLength(), 0);
	memcpy_s((char*)intputData.data(), volumeSize * sizeof(mint16), (char*)m_pVolumeData->getHUDataPoint(), volumeSize * sizeof(mint16));

	std::vector<mint16> dummyArray;
	std::string strDir_result;

	recon.medipPredict(intputData, dummyArray, strDir_result, true);

	StatusMonitor check;

	int milestone = addValue;

	while (true)
	{
		if (m_pVolumeData->threadStop)
		{
			recon.medipStop();
		}

		check = recon.getStatus();
		if (check.st_type == StatusType::NORMAL)
		{
			if (milestone < check.progress)
			{
				milestone = check.progress;
				setProgressValue(milestone);
				qDebug() << milestone;
			}
		}
		else if (check.st_type == StatusType::STOPPING)
		{
			qWarning() << "AI Stopping";
		}
		else
		{
			break;
		}
	}

	if (check.st_type == StatusType::EXCEPTION_TERMINATED)
	{
		qWarning() << check.exception.c_str();
	}
	else if (check.st_type == StatusType::DONE)
	{
		m_OutputData = recon.getVectorResults();
	}

	if (m_OutputData.empty())
	{
		qWarning() << "fail to WorkAIKernelContinousConversion. result empty";
		return false;
	}

	return true;
}

