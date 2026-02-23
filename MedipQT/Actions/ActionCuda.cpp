#include "stdafx.h"
#include "Actions/ActionCuda.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"
#include "algorithm/Radiomics.h"

#include "DeepInsthink.h"
#include "Definitions.h"
#include "Metadata.h"
#include "Network/Network.h"
#include "LicenseManager.h"
#include "DataContext.h"

#include <ppl.h>
using namespace concurrency;


void WorkLiverAdd::setProgressValue(int value, bool init)
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

void WorkLiverAdd::threadRun()
{
	setProgressValue(0, true);
	_addValue = 1;

	QString liverPath = STRING_MANAGER->AIFilePath + "/" + QString::number((int)AI_LIVER);
	QString paramName[] = { "/w1.txt", "/b1.txt", "/g1.txt", "/t1.txt" };
	for (int i = 0; i < PARAM_COUNT; i++)
	{
		QString fileName = liverPath + paramName[i];
		QString ext = fileName.section('.', -1);
		QStringList str;
		if (!fileName.isEmpty())
		{
			str = fileName.split(QRegularExpression("[0-9]+.[a-z]{3,}$"));

			for (int j = 1;; j++)
			{
				QString fileCheck = str.at(0) + QString("%1.%2").arg(j).arg(ext);

				QFile _file(fileCheck);

				if (_file.exists())
					_series[i].push_back(fileCheck.toLocal8Bit().toStdString());
				else
					break;
			}
		}
	}//file auto load

	int height = _volumData->getCY();
	int width = _volumData->getCX();
	int cnt = _volumData->getCZ();


	bool res = _volumData->createTempMaskData();

	if (!res)
	{
		emit finished();
		return;
	}
	res = false;

	mask* _output = _volumData->getMaskTempDataPoint();

#ifdef AI_VER
	//	DeepLearningLiver(_output, _volumData->getHUDataPoint(),
	//		width, height, cnt, _ww, _wl, _start, _to, _series, this);
#endif
	_volumData->threadResult = 1;

	emit finished();

	return;
}

ActionLiverAdd::ActionLiverAdd(QUndoCommand* parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_CUDA_LIVER_ADD;
	m_id = s_id++;
	m_list_index = 0;
	m_first = false;
}

void ActionLiverAdd::undo()
{
	if (m_del)
	{
		bool r = DATA_CONTEXT->volume_data.delMaskInfo(m_list_index);
		if (r)
		{
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(false);
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			DATA_CONTEXT->volume_data.createTempMaskData();
			file.read((char*)DATA_CONTEXT->volume_data.pData3D_Mask_Temp, DATA_CONTEXT->volume_data.getVolumeDataLength());
			file.close();

			DATA_CONTEXT->volume_data.applyTempMask(m_maskIndex);
			WIN_MANAGER->renderLater_GridView(false);
		}
	}
	DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
	//	DATA_CONTEXT->volume_data.voxelCount[m_info.uid] = 0;
	//	DATA_CONTEXT->volume_data.boundingBoxROI[m_info.uid].reset(true);

	DATA_CONTEXT->volume_data.setVoxelCount(m_info.uid, 0);
	DATA_CONTEXT->volume_data.setBoundingBox(m_info.uid, BoundingBoxI(), true);



	WIN_MANAGER->applyVoxelToUI(m_info.uid);
	WIN_MANAGER->setSaveState(false);
}

void ActionLiverAdd::redo()
{
	if (m_first == false)
	{
		m_first = true;
		m_del = DATA_CONTEXT->volume_data.createMaskInfo();
		m_list_index = DATA_CONTEXT->volume_data.getCurrentMaskInfoID();
		m_info = *DATA_CONTEXT->volume_data.getCurrentMaskInfo();
		m_maskIndex = DATA_CONTEXT->volume_data.getCurrentMaskIndex();
	}
	else if (m_del)
	{
		DATA_CONTEXT->volume_data.insertMaskInfo(m_list_index, m_info);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			DATA_CONTEXT->volume_data.createTempMaskData();
			file.read((char*)DATA_CONTEXT->volume_data.pData3D_Mask_Temp, DATA_CONTEXT->volume_data.getVolumeDataLength());
			file.close();

			DATA_CONTEXT->volume_data.applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)DATA_CONTEXT->volume_data.getMaskDataPoint(m_maskIndex), DATA_CONTEXT->volume_data.getVolumeDataLength());
				file.close();
			}

			DATA_CONTEXT->volume_data.applyTempMaskBitChange(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex, 1);
			DATA_CONTEXT->volume_data.updateUIDBoundingBox(m_info.uid, true);
			//	m_orgbox = DATA_CONTEXT->volume_data.boundingBoxROI[m_info.uid];
			m_orgbox = DATA_CONTEXT->volume_data.getBoundingBox(m_info.uid);
			m_fillCount = DATA_CONTEXT->volume_data.fillMaskCount;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)DATA_CONTEXT->volume_data.getMaskDataPoint(m_maskIndex), DATA_CONTEXT->volume_data.getVolumeDataLength());
				file.close();
			}
		}
		//	DATA_CONTEXT->volume_data.voxelCount[m_info.uid] = m_fillCount;
		//	DATA_CONTEXT->volume_data.boundingBoxROI[m_info.uid] = m_orgbox;

		DATA_CONTEXT->volume_data.setVoxelCount(m_info.uid, m_fillCount);
		DATA_CONTEXT->volume_data.setBoundingBox(m_info.uid, m_orgbox);

		WIN_MANAGER->renderLater_GridView(false);
	}

	WIN_MANAGER->updateUI(true, m_list_index);
	DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

bool ActionLiverAdd::mergeWith(const QUndoCommand* command)
{
	return false;
}

WorkDeepTrain::WorkDeepTrain(VOLUME_DATA* pVolumeData, QString projName, QVector<MaskInfo*> projInfo, bool useGPU, bool isLoad)
{
	this->LprojName = this->projName = projName;
	this->useGPU = useGPU;
	this->isLoad = isLoad;
	this->projInfo = projInfo;

	m_pVolumeData = pVolumeData;
}

void WorkDeepTrain::setProgressValue(int value, bool init)
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

void WorkDeepTrain::threadRun()
{
#ifdef AI_VER
	const QString trainPath = STRING_MANAGER->LocalAISegPath + "/train";
	const QString dataPath = trainPath + "/data";
	const QString grPath = trainPath + "/groundtruth";
	const QString projPath = STRING_MANAGER->LocalAISegPath + "/weight";
	const QString LprojPath = STRING_MANAGER->LocalAISegPath + "/weight";


	QDir dir(trainPath);

	dir.mkdir(trainPath);
	dir.mkdir(dataPath);
	dir.mkdir(grPath);

	//1. mask raw export
	VOLUME_DATA* vol_dt = m_pVolumeData;

	int				WW, WL;
	muint32			cx, cy, cz;
	BoundingBoxI	_box;
	QVector<QPoint>	_mID;

	WW = WIN_MANAGER->getWindowWidth();
	WL = WIN_MANAGER->getWindowLevel();
	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	_box.reset(cx, cy, cz);

	_box.minX = _box.maxX;
	_box.minY = _box.maxY;
	_box.minZ = _box.maxZ;
	_box.maxZ = _box.maxY = _box.maxX = 0;

	unsigned char** _mask = new unsigned char* [projInfo.size()];

	memset(_mask, 0, sizeof(char*) * (projInfo.size()));

	for (int i = 0; i < projInfo.size(); i++)
	{
		_mask[i] = new unsigned char[cx * cy];
		BoundingBoxI tmpBox = vol_dt->getBoundingBox(projInfo.at(i)->uid);
		int mI = (projInfo.at(i)->uid >= MASK_SECOND_MAX) ? (projInfo.at(i)->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		mask _m = mI == 0 ? projInfo.at(i)->mask_id : projInfo.at(i)->mask_id2;

		_mID.push_back(QPoint(mI, _m));

		if (_box.minX > tmpBox.minX)
			_box.minX = tmpBox.minX;
		if (_box.minY > tmpBox.minY)
			_box.minY = tmpBox.minY;
		if (_box.minZ > tmpBox.minZ)
			_box.minZ = tmpBox.minZ;

		if (_box.maxX < tmpBox.maxX)
			_box.maxX = tmpBox.maxX;
		if (_box.maxY < tmpBox.maxY)
			_box.maxY = tmpBox.maxY;
		if (_box.maxZ < tmpBox.maxZ)
			_box.maxZ = tmpBox.maxZ;
	}

	QVector<int> sliceNum;
	QString fileName = grPath + "/deepdraw_slice_";

	setProgressValue(0, true);
	int _addValue = 1;

	sliceNum.clear();

	bool res = true;
	for (int z = _box.minZ; z <= _box.maxZ; z++)
	{
		bool chk = false;
		for (int i = 0; i < projInfo.size(); i++)
			memset(_mask[i], 0, cx * cy);

		for (int y = _box.minY; y <= _box.maxY; y++)
		{
			for (int x = _box.minX; x <= _box.maxX; x++)
			{
				for (int i = 0; i < projInfo.size(); i++)
				{
					int mI = _mID.at(i).x();
					mask _m = _mID.at(i).y();
					if (vol_dt->getMaskData(x, y, z, mI) & _m)
					{
						_mask[i][y * cx + x] = 1;
						if (!chk) chk = true;
					}
				}
			}
		}

		if (chk)
		{
			sliceNum.append(z);

			for (int i = 0; i < projInfo.size(); i++)
			{
				QString rawName = fileName + QString("%1_gt%2.raw").arg(cz - (z + 1)).arg(i + 1);
				QString tmpName = rawName + ".tmp_";

				QFile file(tmpName);

				if (file.exists())
					file.remove();

				if (!file.open(QIODevice::WriteOnly))
				{
					vol_dt->threadResult = 0;
					file.remove();
					for (int i = 0; i < projInfo.size(); i++)
						SAFE_DELETES(_mask[i]);
					SAFE_DELETES(_mask);
					emit finished();
					return;
				}

				file.write((const char*)_mask[i], (cx * cy));
				file.close();

				QFile orgFile(rawName);

				res = true;
				if (orgFile.exists())
					res = orgFile.remove();

				if (res)
					res = file.rename(rawName);

				if (!res)
					break;
			}
		}

		if (vol_dt->threadStop)
		{
			vol_dt->threadResult = 0;
			for (int i = 0; i < projInfo.size(); i++)
				SAFE_DELETES(_mask[i]);
			SAFE_DELETES(_mask);
			emit finished();
			return;
		}

		setProgressValue(((float)(z) / _box.maxZ) * 20);
	}//20

	for (int i = 0; i < projInfo.size(); i++)
		SAFE_DELETES(_mask[i]);
	SAFE_DELETES(_mask);

	if (!res)
	{
		vol_dt->threadResult = 0;
		emit finished();
		return;
	}

	_addValue = 20;

	//2. hu raw export
	fileName = dataPath + "/deepdraw_slice_";
	mint16* maskRaw = new mint16[cx * cy];

	for (int i = 0; i < sliceNum.size(); i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));
		int z = sliceNum.at(i);
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				maskRaw[y * cx + x] = vol_dt->getData(x, y, z);
			}
		}

		QString rawName = fileName + QString("%1.raw").arg(cz - (z + 1));
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(_addValue + ((float)(i) / sliceNum.size()) * 20);

		if (vol_dt->threadStop)
		{
			vol_dt->threadResult = 0;
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//40

	if (!res)
	{
		vol_dt->threadResult = 0;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	_addValue = 40;
	int milestone = _addValue;

#if !defined(COVID19_VER)
	// deepcatch
	std::vector< std::vector<int> > salt{
	{ 546857975, 1206038495, 168341876, 705786705, 2088930087, 915972608, 346285530,
	352315002, 1062402511, 256550641, 1107713622, 335710866, 268138010, 1826114301, 1830976666, 791110495,
	1486281456, 1178250288, 1817403054, 1358959822, 1363576248, 317338534, 1709500397, 1502044173,
	566675089, 1017128345, 555790810, 1088649745, 292833056, 1326424169, 1562707811, 786226257, }
	};

#else
	// covid
	std::vector< std::vector<int> > salt{
		{ 579300708, 726097416, 588116827, 604767390, 1478200418, 433635012, 852043623, 1916908445,
		1277928455, 71885609, 1270542588, 859198734, 1763850869, 1067084901, 816315519, 1763367066,
		151402783, 568824546, 1320021280, 1331021245, 1678907526, 1880583558, 1471904308, 1567611961,
		1032555155, 872860059, 1843027393, 1000896398, 1991387589, 1938167859, 581051838, 1945249014 , }
	};
#endif
	//cudaFree(0);
	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> _segment;
	if (isLoad) {
		_segment = std::make_shared<Segmentation>(LprojPath.toLocal8Bit().toStdString(),// + "/",
			LprojName.toLocal8Bit().toStdString(), true, salt);
		//LprojName.toLocal8Bit().toStdString() + ".mipx", true, salt);
	}
	else {
		vector<string> classes;
		for (int i = 0; i < projInfo.size(); ++i)
			classes.push_back("");//ROI 이름으로 바꿀 예정
		_segment = std::make_shared<Segmentation>(ClientType::MEDIP, WindowNormOptions(WW, WL), classes,
			SegmentationArch::UNET,
			SegmentationOptions().loss(LossFunction::Sigmoid).slice(1).init(64), salt);
	}

	if (projInfo.size() < _segment->getClasses().size()) {
		//projInfo.size(): 유저가 선택한 ROI 개수
		//class size: 필수 ROI 개수
		//Show "Not enough ROI chosen."

		/*
		vol_dt->threadResult = (*shmVal);
		vol_dt->threadResult = -vol_dt->threadResult;
		*/
		res = false;
	}
	else {
		int epochs = 1;
		int batch_size = 1;
		double lr = 0.1;

		CheckData check;
		if (_segment->medipTrain(dataPath.toLocal8Bit().toStdString(), grPath.toLocal8Bit().toStdString(), projPath.toLocal8Bit().toStdString(), projName.toLocal8Bit().toStdString(),
			useGPU, cy, cx, epochs, batch_size, lr)) {
			int _addValue = 0;
			while (true) {
				if (vol_dt->threadStop)
				{
					_segment->medipStop();
				}
				check = _segment->getCheckData();
				if (check.status == Status::NORMAL) {
					int progress = milestone + (check.progress * (100 - milestone) / 100);
					if (_addValue < progress) {
						_addValue = progress;//send signal
						setProgressValue(_addValue);
					}
				}
				else if (check.status == Status::STOPPING) {
					//printf("Stopping\n");
				}
				else {
					break;
				}
			}
		}
		else {
			check = _segment->getCheckData();
			res = false;
		}
		if (check.status == Status::EXCEPTION_TERMINATED) {
			//Show exception?
			QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/error.log";
			QFile file(filename);
			if (file.open(QIODevice::ReadWrite))
			{
				QTextStream stream(&file);
				stream << check.exception.c_str() << endl;
			}
			res = false;
		}
		//_segment->clearNet();
		auto aClassName = _segment->getClasses(); // roi name
		_segment.reset();
		AIBase::clearCache();
		//cudaDeviceReset();
		//cudaDeviceSynchronize();
	}

	if (!res)
	{
		vol_dt->threadResult = 0;
		emit finished();
		return;
	}
	/*
	const HANDLE hMutex = CreateMutex(NULL, FALSE, L"AIState");
	const HANDLE hFile = CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(char), L"AIState_shm");
	char *shmVal = (char*)MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	char shmRes = AI_MEDIP_DEFAULT;
	(*shmVal) = shmRes;
	DWORD exCode;
	QString trCommand, trFile;

	trFile = STRING_MANAGER->programPath + "/deepinsthink/deepinsthink.exe";

	QFile AIFile(trFile);

	if (!AIFile.exists())
	{
		vol_dt->threadResult = 2;
		emit finished();
		return;
	}
	SHELLEXECUTEINFO shInfo;

	memset(&shInfo, 0, sizeof(SHELLEXECUTEINFO));

	shInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shInfo.lpVerb = L"runas";
	shInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shInfo.lpFile = (const wchar_t*)trFile.utf16();
	trCommand = (QString("--deepdraw --train "));

	trCommand.append(QString("--train-dir \"%1\" ").arg(dataPath));
	trCommand.append(QString("--groundtruth-dir \"%1\" ").arg(grPath));
	trCommand.append(QString("--save-weight-dir \"%1\" ").arg(projPath));
	trCommand.append(QString("--save-weight-name \"%1\" ").arg(projName));
	trCommand.append(QString("--ww %1 ").arg(WW));
	trCommand.append(QString("--wl %1 ").arg(WL));
	trCommand.append(QString("--h %1 ").arg(cy));
	trCommand.append(QString("--w %1 ").arg(cx));
	trCommand.append(QString("--n-class %1 ").arg(projInfo.size()));
	trCommand.append(useGPU ? QString("--gpu ") : QString());
	trCommand.append(isLoad ? QString("--load --load-weight-dir \"%1\" --load-weight-name \"%2\" ")
		.arg(LprojPath).arg(LprojName) : QString());
	//	trCommand.append(QString("&& exit "));
	shInfo.lpParameters = (const wchar_t*)trCommand.utf16();
	shInfo.lpDirectory = (const wchar_t*)(STRING_MANAGER->programPath.utf16());
	shInfo.nShow = SW_HIDE;

	res = ShellExecuteEx(&shInfo); //deepinsthink execute

	if (!res)
	{
		vol_dt->threadResult = 0;
		emit finished();
		return;
	}
	_addValue = 0;
	while (true)
	{
		DWORD dwRes = WaitForSingleObject(hMutex, INFINITE);
		res = (WAIT_OBJECT_0 == dwRes);

		if (!res)
		{
			dwRes = GetLastError();
			(*shmVal) = AI_MEDIP_STOP;
			break;
		}

		if (vol_dt->threadStop)
		{
			if ((*shmVal) >= AI_MEDIP_DEFAULT)
				(*shmVal) = AI_MEDIP_STOP;
			else
				shmRes = AI_MEDIP_STOP;
		}

		if ((*shmVal) >= AI_MEDIP_DEFAULT)
		{
			switch (shmRes)
			{
			case AI_MEDIP_DEFAULT:
			{
				if ((*shmVal) == AI_SUCCESS)
				{
					shmRes = AI_SUCCESS;
					res = true;
				}
				else if (_addValue < (*shmVal))
				{
					_addValue = (*shmVal);
					setProgressValue(_addValue);
				}
			}
			break;
			case AI_MEDIP_N_REQUEST:
			{
				vol_dt->threadResult = (*shmVal);
				vol_dt->threadResult = -vol_dt->threadResult;
				res = false;
			}
			break;
			case AI_MEDIP_STOP:
				res = false;
				break;
			}

		}
		else
		{
			switch (shmRes)
			{
			case AI_MEDIP_DEFAULT:
				if ((*shmVal) == AI_MEDIP_STOP)
					res = true;
				break;
			case AI_MEDIP_STOP:
				res = false;
				break;
			case AI_MEDIP_N_REQUEST:
			{
				vol_dt->threadResult = 0;
				res = false;
			}
			break;
			default:
			{
				if ((*shmVal) == AI_N_MISMATCH)
				{
					shmRes = AI_MEDIP_N_REQUEST;
					(*shmVal) = shmRes;
					res = true;
				}
				else
					res = false;
			}
			break;
			}

		}

		ReleaseMutex(hMutex);

		if ((!res) || (shmRes == AI_SUCCESS))
			break;

		if (GetExitCodeProcess(shInfo.hProcess, &exCode))
		{
			if (exCode != STILL_ACTIVE)
			{
				res = false;
				break;
			}
		}
	}
	UnmapViewOfFile(shmVal);

	CloseHandle(hMutex);
	CloseHandle(hFile);

	if (GetExitCodeProcess(shInfo.hProcess, &exCode))
	{
		if (exCode == STILL_ACTIVE)
			TerminateProcess(shInfo.hProcess, 2);
	}
	CloseHandle(shInfo.hProcess);
	*/
	if (res)
	{
		QString filePath = projPath + QString("/%1.mipx").arg(projName);

		QFile file(filePath);

		res = file.exists();
	}

	if (res)
		vol_dt->threadResult = 1;
#endif

	emit finished();
	return;
}

ActionTranslationAdd::ActionTranslationAdd(QString copyPath, QUndoCommand* parent /*= nullptr*/)
	:QUndoCommand(parent),
	m_copyPath(copyPath)
{
	static int s_id = ACT_ID_TRANS_PREDICT;
	m_id = s_id++;

}

void ActionTranslationAdd::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		bool res;
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			{
				file.read((char*)DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getVolumeDataLength() * sizeof(mint16));
			}
			file.close();

			DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
			WIN_MANAGER->renderLater_GridView(true);
		}

		DATA_CONTEXT->volume_data.updateVolumeMinMax();
		Visualize2DTab* p2Dtab = WIN_MANAGER->GetTab()->get2DTab();
		if (p2Dtab)
		{
			emit p2Dtab->sig_autoClickSgl();
		}

		AITranslationTab* pAITransTab = WIN_MANAGER->GetTab()->getAITranslationTab();
		if (pAITransTab)
		{
			emit pAITransTab->sig_disablePredictBtn(false);
			emit pAITransTab->sig_checkedPredictBtn(false);
		}

		DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionTranslationAdd::redo()
{
	const QString outPath = STRING_MANAGER->AITranslationPath + "/predict/result";
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			{
				file.read((char*)DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getVolumeDataLength() * sizeof(mint16));
			}

			file.close();

			DATA_CONTEXT->volume_data.updateVolumeMinMax();

			Visualize2DTab* p2Dtab = WIN_MANAGER->GetTab()->get2DTab();
			if (p2Dtab)
			{
				emit p2Dtab->sig_autoClickSgl();
			}
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getVolumeDataLength() * sizeof(mint16));
				file.close();
			}
			////////////////////////////////////////////////////////
			//////////////////
			const QString outPath = STRING_MANAGER->AITranslationPath + "/predict/result";
			QString filePath = outPath + "/deepdraw_out_1.raw";

			QFile file(filePath);
			if (file.exists())
			{
				int nDataCX = DATA_CONTEXT->volume_data.getCX();
				int nDataCY = DATA_CONTEXT->volume_data.getCY();
				int nDataCZ = DATA_CONTEXT->volume_data.getCZ();
				int nNewDepth = nDataCZ + 1;
				int nOriginDataLen = nDataCX * nDataCY * nDataCZ;

				float fSpaceX = DATA_CONTEXT->volume_data.getSpaceX(true);
				float fSpaceY = DATA_CONTEXT->volume_data.getSpaceY(true);
				float fSpaceZ = DATA_CONTEXT->volume_data.getSpaceZ(true);


				std::vector<mint16> vNewData(nOriginDataLen, 0);
				//const mint16* pData3D_HU = DATA_CONTEXT->volume_data.getHUDataPoint();
				//vNewData.insert(vNewData.begin(), pData3D_HU, pData3D_HU + nOriginDataLen);

				if (file.open(QFile::ReadOnly))
				{
					//file.read(((char*)(&vNewData[0]) + nOriginDataLen * sizeof(mint16)), nOriginDataLen * sizeof(mint16));
					file.read(((char*)(&vNewData[0])), nOriginDataLen * sizeof(mint16));
					int a = 0;
					file.close();
				}

				mint16* pData3D_HU = DATA_CONTEXT->volume_data.getHUDataPoint();
				memcpy(pData3D_HU, &vNewData[0], nOriginDataLen * sizeof(mint16));


			}
			/////////////////////////////////////////////////////////

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getVolumeDataLength() * sizeof(mint16));
				file.close();
			}
		}

		DATA_CONTEXT->volume_data.updateVolumeMinMax();
		Visualize2DTab* p2Dtab = WIN_MANAGER->GetTab()->get2DTab();
		if (p2Dtab)
		{
			emit p2Dtab->sig_autoClickSgl();
		}

		AITranslationTab* pAITransTab = WIN_MANAGER->GetTab()->getAITranslationTab();
		if (pAITransTab)
		{
			emit pAITransTab->sig_disablePredictBtn(true);
			emit pAITransTab->sig_checkedPredictBtn(true);
		}

		DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->setSaveState(false);
	}

}

ActionOutsetApply::ActionOutsetApply(int nThreshold, int _uid, QUndoCommand* parent /*= NULL*/) :
	QUndoCommand(parent),
	m_nThreshold(nThreshold),
	m_uid(_uid)
{
	static int s_id = ACT_ID_DD_OUTSET;

	MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(m_uid, true);

	if (info)
	{
		m_nByteIndex = m_uid >= MASK_SECOND_MAX ? (m_uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		m_maskBit = m_nByteIndex == 0 ? info->mask_id : info->mask_id2;
		m_TAState = DATA_CONTEXT->volume_data.getTAState(m_uid);
		m_orgbox[0] = DATA_CONTEXT->volume_data.getBoundingBox(m_uid);
		m_orgbox[1].reset(true);

		AISegTab* pAItab = WIN_MANAGER->GetTab()->getAITab();
		if (pAItab && pAItab->m_tabSet)
		{
			m_nPreThreshold = pAItab->m_tabSet->getPreOutVal();
			//qDebug() << "contrunctor pre threshold : " << m_nPreThreshold;
		}

		for (int i = 0; i < DATA_CONTEXT->volume_data.m_vecAIResultData.size(); ++i)
		{
			if (DATA_CONTEXT->volume_data.m_vecAIResultData[i].first == m_uid)
			{
				m_nVectorIdx = i;
				break;
			}
		}
	}

	m_id = s_id++;
}

void ActionOutsetApply::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QFile::ReadOnly))
		{
			DATA_CONTEXT->volume_data.createTempMaskData();

			mask* _res = DATA_CONTEXT->volume_data.getMaskTempDataPoint();
			muint32 cx, cy, cz;
			DATA_CONTEXT->volume_data.getLengthForScreen(WT_AXIAL, cx, cy, cz);

			if (_res)
			{
				file.read((char*)_res, cx * cy * cz);
				file.close();
			}

			AISegTab* pAItab = WIN_MANAGER->GetTab()->getAITab();
			if (pAItab && pAItab->m_tabSet)
			{
				pAItab->m_tabSet->setSliderOutVal(m_nPreThreshold);
				//qDebug() << "pre threshold : " << m_nPreThreshold;
			}

			DATA_CONTEXT->volume_data.applyTempMaskBitChange(m_maskBit, m_nByteIndex);

			DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
			DATA_CONTEXT->volume_data.setBoundingBox(m_uid, m_orgbox[0]);
			DATA_CONTEXT->volume_data.setVoxelCount(m_uid, -m_fillCount, false);
			WIN_MANAGER->renderLater_GridView(true);
			DATA_CONTEXT->volume_data.setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->updateUI();
		}
	}

	WIN_MANAGER->setSaveState(false);
}

void ActionOutsetApply::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		muint32 cx, cy, cz;
		DATA_CONTEXT->volume_data.getLengthForScreen(WT_AXIAL, cx, cy, cz);

		if (file.exists() && file.open(QFile::ReadOnly))
		{
			DATA_CONTEXT->volume_data.applyTempMaskBitChange(m_maskBit, m_nByteIndex);

			AISegTab* pAItab = WIN_MANAGER->GetTab()->getAITab();
			if (pAItab && pAItab->m_tabSet)
			{
				pAItab->m_tabSet->setSliderOutVal(m_nThreshold);
				//qDebug() << "pre threshold : " << m_nThreshold;
			}
		}
		else
		{
			mask* _res = DATA_CONTEXT->volume_data.getMaskDataPoint(m_nByteIndex);
			filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);

			file.setFileName(filename);

			if (file.open(QFile::WriteOnly))
			{
				file.write((char*)_res, cx * cy * cz);
				file.close();
			}


			if (m_nVectorIdx >= DATA_CONTEXT->volume_data.m_vecAIResultData.size())
				return;

			m_fillCount = DATA_CONTEXT->volume_data.applyTempMaskBitOutset(DATA_CONTEXT->volume_data.m_vecAIResultData[m_nVectorIdx].second
				, m_maskBit, m_nByteIndex, m_nThreshold);

			_res = DATA_CONTEXT->volume_data.getMaskDataPoint(m_nByteIndex);
			filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

			file.setFileName(filename);

			if (file.open(QFile::WriteOnly))
			{
				file.write((char*)_res, cx * cy * cz);
				file.close();
			}

			DATA_CONTEXT->volume_data.updateUIDBoundingBox(m_uid, true);
			m_orgbox[1] = DATA_CONTEXT->volume_data.getBoundingBox(m_uid);

			m_fBurdenVal = getPneumoniaBurdenValue(DATA_CONTEXT->volume_data.getMaskDataPoint(m_nByteIndex)
				, DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getMask(m_uid)
				, DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ()
				, DATA_CONTEXT->volume_data.getSpaceX(true), DATA_CONTEXT->volume_data.getSpaceY(true), DATA_CONTEXT->volume_data.getSpaceZ(true));

		}

		DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
		DATA_CONTEXT->volume_data.setBoundingBox(m_uid, m_orgbox[1]);
		DATA_CONTEXT->volume_data.setBurdenVal(m_uid, m_fBurdenVal);
		DATA_CONTEXT->volume_data.setVoxelCount(m_uid, m_fillCount, true);
		WIN_MANAGER->renderLater_GridView(true);
		DATA_CONTEXT->volume_data.setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		//WIN_MANAGER->updateUI();
	}

	WIN_MANAGER->setSaveState(false);
}

WorkTranslationPredict::WorkTranslationPredict(VOLUME_DATA* pVolumeData, QString LprojName, int start, int end, bool useGPU)
{
	m_pVolumeData = pVolumeData;
	this->LprojName = LprojName;
	this->useGPU = useGPU;
	this->start = start;
	this->end = end;
}

void WorkTranslationPredict::setProgressValue(int value, bool init /*= false*/)
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

void WorkTranslationPredict::threadRun()
{
#ifdef AI_VER
	///////////////////////////////////////////

	const QString prePath = STRING_MANAGER->AITranslationPath + "/predict";
	const QString LprojPath = STRING_MANAGER->AITranslationPath + "/weight";
	QString copyPath = STRING_MANAGER->AITranslationPath + "/predict_result";
	const QString dataPath = prePath + "/data";//deepdraw_slice_1.raw
	const QString resPath = prePath + "/result";//deepdraw_out_1.raw 

	VOLUME_DATA* vol_dt = m_pVolumeData;

	copyPath.append(QString("/%1").arg(LprojName));

	QDir dir(prePath);

	dir.mkdir(prePath);
	dir.mkdir(dataPath);
	dir.mkdir(resPath);
	dir.mkdir(copyPath);

	muint32 cx, cy, cz;

	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	setProgressValue(0, true);


	int tmpVal = cz - (start + 1);
	start = end;
	end = tmpVal;
	start = cz - (start + 1);

	mint16* maskRaw = new mint16[cx * cy];
	bool res = false;
	for (int i = start; i <= end; i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
			}
		}

		QString rawName = dataPath + QString("/Translation_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(((float)(i) / end) * 20);

		if (vol_dt->threadStop)
		{
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//20

	if (!res)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	int _addValue = 20;


	std::vector< std::vector<int> > salt{
		{ 546857975, 1206038495, 168341876, 705786705, 2088930087, 915972608, 346285530,
		352315002, 1062402511, 256550641, 1107713622, 335710866, 268138010, 1826114301, 1830976666, 791110495,
		1486281456, 1178250288, 1817403054, 1358959822, 1363576248, 317338534, 1709500397, 1502044173,
		566675089, 1017128345, 555790810, 1088649745, 292833056, 1326424169, 1562707811, 786226257, }
	};

#ifdef COVID19_VER
	salt = {
		{ 579300708, 726097416, 588116827, 604767390, 1478200418, 433635012, 852043623, 1916908445,
		1277928455, 71885609, 1270542588, 859198734, 1763850869, 1067084901, 816315519, 1763367066,
		151402783, 568824546, 1320021280, 1331021245, 1678907526, 1880583558, 1471904308, 1567611961,
		1032555155, 872860059, 1843027393, 1000896398, 1991387589, 1938167859, 581051838, 1945249014 , }
	};
#endif

	Translation::setExtension("mipx");
	std::shared_ptr<Translation> _segment;
	_segment = std::make_shared<Translation>(LprojPath.toLocal8Bit().toStdString(), LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;
	if (_segment->medipPredict(dataPath.toLocal8Bit().toStdString(), resPath.toLocal8Bit().toStdString() + "/", useGPU, cy, cx)) {

		while (true) {
			if (vol_dt->threadStop)
			{
				_segment->medipStop();
			}
			check = _segment->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}
		//get results directly in form of OpenCV matrix
		//if (check.status == Status::DONE) {results = s.getResults();}

		//get classes name (for example Skin, Bone, Muscle) to label the ROI
		//auto classes = s.getClasses();
	}
	else {
		check = _segment->getCheckData();
		res = false;
	}
	if (check.status == Status::EXCEPTION_TERMINATED) {
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}
	_segment.reset();


	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		//vol_dt->threadResult = 1;
		emit finished();
		return;
	}

	///////////////////////////////////////////////////
		/*
		const HANDLE hMutex = CreateMutex(NULL, FALSE, L"AIState");
		const HANDLE hFile = CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(char), L"AIState_shm");
		char *shmVal = (char*)MapViewOfFile(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		char shmRes = 0;
		(*shmVal) = shmRes;
		QString trCommand, trFile;
		DWORD exCode;

		trFile = STRING_MANAGER->programPath + "/deepinsthink/deepinsthink.exe";

		QFile AIFile(trFile);

		if (!AIFile.exists())
		{
		vol_dt->threadResult = -2;
		emit finished();
		return;
		}

		SHELLEXECUTEINFO shInfo;

		memset(&shInfo, 0, sizeof(SHELLEXECUTEINFO));

		shInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shInfo.lpVerb = L"runas";
		shInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shInfo.lpFile = (const wchar_t*)trFile.utf16();
		trCommand = (QString("--deepdraw --predict "));
		trCommand.append(QString("--predict-dir \"%1\" ").arg(dataPath));
		trCommand.append(QString("--result-dir \"%1\" ").arg(resPath));
		trCommand.append(QString("--load --load-weight-dir \"%1\" ").arg(LprojPath));
		trCommand.append(QString("--load-weight-name \"%1\" ").arg(LprojName));
		trCommand.append(QString("--h %1 ").arg(cy));
		trCommand.append(QString("--w %1 ").arg(cx));
		trCommand.append(QString("--d %1 ").arg(cz));
		trCommand.append(useGPU ? QString("--gpu ") : QString());
		trCommand.append(QString("&& exit "));
		shInfo.lpParameters = (const wchar_t*)trCommand.utf16();

		shInfo.lpDirectory = (const wchar_t*)(STRING_MANAGER->programPath.utf16());
		shInfo.nShow = SW_HIDE;

		res = ShellExecuteEx(&shInfo); //deepinsthink execute

		if (!res)
		{
		vol_dt->threadResult = -1;
		emit finished();
		return;
		}
		int _addValue = 0;
		*/
		/*
		while (true)
		{
		DWORD dwRes = WaitForSingleObject(hMutex, INFINITE);
		res = (WAIT_OBJECT_0 == dwRes);

		if (!res)
		{
		dwRes = GetLastError();
		(*shmVal) = AI_MEDIP_STOP;
		break;
		}

		if (vol_dt->threadStop)
		{
		if ((*shmVal) >= AI_MEDIP_DEFAULT)
		(*shmVal) = AI_MEDIP_STOP;
		else
		shmRes = AI_MEDIP_STOP;
		}

		if ((*shmVal) >= AI_MEDIP_DEFAULT)
		{
		switch (shmRes)
		{
		case AI_MEDIP_DEFAULT:
		{
		if ((*shmVal) == AI_SUCCESS)
		{
		shmRes = AI_SUCCESS;
		res = true;
		}
		else if (_addValue < (*shmVal))
		{
		_addValue = (*shmVal);
		setProgressValue(_addValue);
		}
		}
		break;
		case AI_MEDIP_N_REQUEST:
		{
		vol_dt->threadResult = (*shmVal);
		vol_dt->threadResult = -vol_dt->threadResult;
		res = false;
		}
		break;
		case AI_MEDIP_STOP:
		res = false;
		break;
		}

		}
		else
		{
		switch (shmRes)
		{
		case AI_MEDIP_DEFAULT:
		if ((*shmVal) == AI_MEDIP_STOP)
		res = true;
		break;
		case AI_MEDIP_STOP:
		res = false;
		break;
		case AI_MEDIP_N_REQUEST:
		{
		vol_dt->threadResult = 0;
		res = false;
		}
		break;
		default:
		{
		if ((*shmVal) == AI_N_MISMATCH)
		{
		shmRes = AI_MEDIP_N_REQUEST;
		(*shmVal) = shmRes;
		res = true;
		}
		else
		res = false;
		}
		break;
		}

		}

		ReleaseMutex(hMutex);

		if ((!res) || (shmRes == AI_SUCCESS))
		break;

		if (GetExitCodeProcess(shInfo.hProcess, &exCode))
		{
		if (exCode != STILL_ACTIVE)
		{
		res = false;
		break;
		}
		}
		}

		UnmapViewOfFile(shmVal);

		CloseHandle(hMutex);
		CloseHandle(hFile);

		if (GetExitCodeProcess(shInfo.hProcess, &exCode))
		{
		if (exCode == STILL_ACTIVE)
		{
		TerminateProcess(shInfo.hProcess, 2);
		}
		}
		CloseHandle(shInfo.hProcess);
		*/


	vol_dt->threadResult = 1;

	// 	dir.rmdir(dataPath);
	// 	dir.rmdir(resPath);
	// 	dir.rmdir(copyPath);
#endif
	emit finished();
	return;
}


WorkCOVIDPredict::WorkCOVIDPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, QString LprojName, int start, int end, bool useGPU)
{
	m_pVolumeData = pVolumeData;
	m_strInputPath = strInputPath;
	m_strWeightPath = strWieghtPath;
	m_LprojName = LprojName;
	m_start = start;
	m_end = end;
	m_useGPU = useGPU;
}

void WorkCOVIDPredict::setProgressValue(int value, bool init /*= false*/)
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

void WorkCOVIDPredict::threadRun()
{
#ifdef AI_VER
	// 	const QString prePath = STRING_MANAGER->AISegmentationPath + "/predict";
	// 	const QString LprojPath = STRING_MANAGER->AISegmentationPath + "/weight_covid19";
	const QString prePath = m_strInputPath;// STRING_MANAGER->AISegmentationPath + "/predict";
	const QString LprojPath = m_strWeightPath;// STRING_MANAGER->AISegmentationPath + "/weight";


	//QString copyPath = STRING_MANAGER->AISegmentationPath + "/predict_result";
	const QString dataPath = prePath + "/data";//deepdraw_slice_1.raw
	//const QString resPath = prePath + "/result";//deepdraw_out_1.raw 

	VOLUME_DATA* vol_dt = m_pVolumeData;

	//copyPath.append(QString("/%1").arg(m_LprojName));

	QDir dir(prePath);

	qDebug() << "make prePath:" << dir.mkdir(prePath);
	qDebug() << "make datapath:" << dir.mkdir(dataPath);
	// 	qDebug() << "make resPath:" << dir.mkdir(resPath);
	// 	qDebug() << "make copyPath:" << dir.mkdir(copyPath);

	muint32 cx, cy, cz;

	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	setProgressValue(0, true);


	int tmpVal = cz - (m_start + 1);
	m_start = m_end;
	m_end = tmpVal;
	m_start = cz - (m_start + 1);

	mint16* maskRaw = new mint16[cx * cy];
	bool res = false;

	MaskInfo* pMaskInfo = vol_dt->getAtLastMaskInfo();
	qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);
	mask* pMask3D = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pMaskInfo->uid));
	//mask maskBit = vol_dt->getMask(pMaskInfo->uid);

	for (int i = m_start; i <= m_end; i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));
		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				if (pMask3D[i * cy * cx + y * cx + x])
				{
					maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
				}
				else
				{
					maskRaw[y * cx + x] = -3024;
				}
			}
		}

		QString rawName = dataPath + QString("/deepdraw_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(((float)(i) / m_end) * 20);

		if (vol_dt->threadStop)
		{
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//20

	if (!res)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	int _addValue = 20;

	std::vector< std::vector<int> >	salt{
			{ 579300708, 726097416, 588116827, 604767390, 1478200418, 433635012, 852043623, 1916908445,
			1277928455, 71885609, 1270542588, 859198734, 1763850869, 1067084901, 816315519, 1763367066,
			151402783, 568824546, 1320021280, 1331021245, 1678907526, 1880583558, 1471904308, 1567611961,
			1032555155, 872860059, 1843027393, 1000896398, 1991387589, 1938167859, 581051838, 1945249014 , }
	};

	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> _segment;
	_segment = std::make_shared<Segmentation>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;
	//if (_segment->medipPredict(dataPath.toLocal8Bit().toStdString(), resPath.toLocal8Bit().toStdString() + "/", m_useGPU, cy, cx, cz, m_start)) {
	if (_segment->medipPredict(dataPath.toLocal8Bit().toStdString(), "", m_useGPU, cy, cx, cz, m_start)) {

		while (true) {
			if (vol_dt->threadStop)
			{
				_segment->medipStop();
			}
			check = _segment->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}
		//get results directly in form of OpenCV matrix
		//if (check.status == Status::DONE) {results = s.getResults();}

		//get classes name (for example Skin, Bone, Muscle) to label the ROI
		//auto classes = s.getClasses();

		std::vector<std::string>  vecClassNames = _segment->getClasses();
		//DATA_CONTEXT->volume_data.m_vecTmpMaskName = vecClassNames;

	}
	else {
		check = _segment->getCheckData();
		res = false;
	}
	if (check.status == Status::EXCEPTION_TERMINATED) {
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}

	//DATA_CONTEXT->volume_data.m_vec2dClassData = _segment->getVectorResults();	
// 	std::vector<std::vector<unsigned char>> vClassAll = _segment->getVectorResults();
// 	std::vector<pair<int, std::vector<unsigned char>>>().swap(DATA_CONTEXT->volume_data.m_vec2dClassData);
// 	for (int i = 0; i < vClassAll.size(); ++i)
// 	{
// 		DATA_CONTEXT->volume_data.m_vec2dClassData.push_back(pair<int, std::vector<unsigned char>>(i, std::move(vClassAll[i])));
// 	}

	//

	_segment.reset();


	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = DATA_CONTEXT->volume_data.m_vecAIResultData.size();
	}


	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}

	// 	vol_dt->threadResult = -1;
	// 	if (res)// out file count check
	// 	{
	// 		QDir dir(resPath);
	// 
	// 		if (dir.exists())
	// 		{
	// 			QStringList filters;
	// 			filters << "*.raw" << "*.RAW";
	// 			dir.setNameFilters(filters);
	// 
	// 			QStringList files = dir.entryList();
	// 
	// 			vol_dt->threadResult = files.count();
	// 
	// 			if (vol_dt->threadResult > 0)
	// 			{
	// 				for each (auto f in files)
	// 				{
	// 					QString orgPath = resPath + "/" + f;
	// 					QFile _file(orgPath);
	// 
	// 					if (_file.exists())
	// 					{
	// 						QString copyfile = copyPath + "/" + f;
	// 						_file.copy(copyfile);
	// 
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}


	QDir(dataPath).removeRecursively();
	// 	dir.rmdir(dataPath);
	// 	dir.rmdir(resPath);
	// 	dir.rmdir(copyPath);

#endif

	emit finished();
	return;
}

WorkClassificationPredict::WorkClassificationPredict(VOLUME_DATA* pVolumeData, QString LprojName, int start, int end, bool useGPU)
{
	m_pVolumeData = pVolumeData;
	m_LprojName = LprojName;
	m_start = start;
	m_end = end;
	m_useGPU = useGPU;
}

void WorkClassificationPredict::setProgressValue(int value, bool init /*= false*/)
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


void WorkClassificationPredict::threadRun()
{
#ifdef AI_VER
	const QString prePath = STRING_MANAGER->AIClassificationPath + "/predict";
	const QString LprojPath = STRING_MANAGER->AIClassificationPath + "/weight";

	QString copyPath = STRING_MANAGER->AIClassificationPath + "/predict_result";
	const QString dataPath = prePath + "/data";//deepdraw_slice_1.raw
	const QString resPath = prePath + "/result";//deepdraw_out_1.raw 

	VOLUME_DATA* vol_dt = m_pVolumeData;

	copyPath.append(QString("/%1").arg(m_LprojName));

	QDir dir(prePath);

	qDebug() << "make prePath:" << dir.mkdir(prePath);
	qDebug() << "make datapath:" << dir.mkdir(dataPath);
	qDebug() << "make resPath:" << dir.mkdir(resPath);
	qDebug() << "make copyPath:" << dir.mkdir(copyPath);

	/////////////////////////////////////
	int _addValue = 20;

	std::vector< std::vector<int> > salt{
		{ 546857975, 1206038495, 168341876, 705786705, 2088930087, 915972608, 346285530,
		352315002, 1062402511, 256550641, 1107713622, 335710866, 268138010, 1826114301, 1830976666, 791110495,
		1486281456, 1178250288, 1817403054, 1358959822, 1363576248, 317338534, 1709500397, 1502044173,
		566675089, 1017128345, 555790810, 1088649745, 292833056, 1326424169, 1562707811, 786226257, }
	};
#ifdef COVID19_VER
	salt = {
		{ 579300708, 726097416, 588116827, 604767390, 1478200418, 433635012, 852043623, 1916908445,
		1277928455, 71885609, 1270542588, 859198734, 1763850869, 1067084901, 816315519, 1763367066,
		151402783, 568824546, 1320021280, 1331021245, 1678907526, 1880583558, 1471904308, 1567611961,
		1032555155, 872860059, 1843027393, 1000896398, 1991387589, 1938167859, 581051838, 1945249014 , }
	};
#endif

	Segmentation::setExtension("mipx");
	std::shared_ptr<Classification> refClassification;
	refClassification = std::make_shared<Classification>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);


	ClassificationMeta meta;
	refClassification->readMeta(meta, LprojPath.toLocal8Bit().toStdString(), QString(m_LprojName + ".mipx").toLocal8Bit().toStdString(), true, salt);
	for (int i = 0; i < meta.classes.size(); i++)
	{
		qDebug() << i << "-classname : " << meta.classes[i].c_str();
	}


	/////////////////////
	muint32 cx, cy, cz;

	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	setProgressValue(0, true);


	int tmpVal = cz - (m_start + 1);
	m_start = m_end;
	m_end = tmpVal;
	m_start = cz - (m_start + 1);

	mint16* maskRaw16 = nullptr;
	mint8* maskRaw8 = nullptr;

	// mode test
	// HU mode
	if (meta.input_hu == true)
	{
		qDebug() << "HU mode" << endl;
		maskRaw16 = new mint16[cx * cy];
	}
	// Mask mode
	else
	{
		qDebug() << "Mask mode" << endl;
		maskRaw8 = new mint8[cx * cy];
	}

	bool res = false;

	MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo("Bone");

	qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);
	mask* pMask3D = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pMaskInfo->uid));
	mask maskBit = vol_dt->getMask(pMaskInfo->uid);

	for (int i = m_start; i <= m_end; i++)
	{
		if (meta.input_hu == true)
			memset(maskRaw16, 0, sizeof(mint16) * (cx * cy));
		else
			memset(maskRaw8, 0, sizeof(mint8) * (cx * cy));

		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				// HU mode
				if (meta.input_hu == true)
				{
					if (pMask3D[i * cy * cx + y * cx + x] & maskBit)
					{
						maskRaw16[y * cx + x] = vol_dt->getData(x, y, i);
					}
				}
				// Mask mode
				else
				{
					if (pMask3D[i * cy * cx + y * cx + x] & maskBit)
					{
						maskRaw8[y * cx + x] = 1;
					}
				}
			}
		}

		QString rawName = dataPath + QString("/classification_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		if (meta.input_hu == true)
			file.write((const char*)maskRaw16, (cx * cy) * sizeof(mint16));
		else
			file.write((const char*)maskRaw8, (cx * cy) * sizeof(mint8));

		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(((float)(i) / m_end) * 20);

		if (vol_dt->threadStop)
		{
			SAFE_DELETES(maskRaw16);
			emit finished();
			return;
		}

	}//20

	if (!res)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw16);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw16);

	/////////////////////////////
	CheckData check;
	int milestone = _addValue;

	if (refClassification->medipPredict(dataPath.toLocal8Bit().toStdString(), m_useGPU, cy, cx))
	{
		while (true) {
			if (vol_dt->threadStop)
			{
				refClassification->medipStop();
			}
			check = refClassification->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}
		//get results directly in form of OpenCV matrix
		//if (check.status == Status::DONE) {results = s.getResults();}

		//get classes name (for example Skin, Bone, Muscle) to label the ROI
		//auto classes = s.getClasses();

		/// find L3 mid slice
		vol_dt->threadResult = 0;

		int nStartSlice = -1, nEndSlice = -1, nMidSlice = -1;
		qDebug() << "Start result ";
		std::vector<int>  vecClassResults = refClassification->getResults();
		for (int k = 0; k < vecClassResults.size(); k++)
		{
			qDebug() << k << " -class : " << vecClassResults[k];
			// 3 : L3
			// 9 : L3 in waist
			// 15 : L3 with umbilicus
			int nTmp = -1;
			if (vecClassResults[k] == 3 || vecClassResults[k] == 9 || vecClassResults[k] == 15)
			{
				if (nStartSlice == -1)
				{
					nStartSlice = k;
				}

				if (nEndSlice == -1)
				{
					nTmp = k;
					nEndSlice = k;
				}

				if (nEndSlice != -1 && nTmp < k)
				{
					nEndSlice = k;
				}
			}
		}
		qDebug() << "End result ";
		nMidSlice = nStartSlice + ((nEndSlice - nStartSlice - 1) * 0.5);
		qDebug() << "L3 START : " << nStartSlice;
		qDebug() << "L3 END : " << nMidSlice;
		qDebug() << "L3 mid : " << nMidSlice;
		ACTION_MANAGER->m_qThreadNext.front().pTempData = make_shared<int>(nMidSlice);

	}
	else
	{
		check = refClassification->getCheckData();
		res = false;
	}

	if (check.status == Status::EXCEPTION_TERMINATED) {
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}





	refClassification.reset();




	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}

	vol_dt->threadResult = -1;
	// 	if (res)// out file count check
	// 	{
	// 		QDir dir(resPath);
	// 
	// 		if (dir.exists())
	// 		{
	// 			QStringList filters;
	// 			filters << "*.raw" << "*.RAW";
	// 			dir.setNameFilters(filters);
	// 
	// 			QStringList files = dir.entryList();
	// 
	// 			vol_dt->threadResult = files.count();
	// 
	// 			if (vol_dt->threadResult > 0)
	// 			{
	// 				for each (auto f in files)
	// 				{
	// 					QString orgPath = resPath + "/" + f;
	// 					QFile _file(orgPath);
	// 
	// 					if (_file.exists())
	// 					{
	// 						QString copyfile = copyPath + "/" + f;
	// 						_file.copy(copyfile);
	// 
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}

	// 	dir.rmdir(dataPath);
	// 	dir.rmdir(resPath);
	// 	dir.rmdir(copyPath);

#endif
	emit finished();
	return;
}

WorkL3Predict::WorkL3Predict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName, BoundingBoxI box, bool useGPU)
{
	m_pVolumeData = pVolumeData;
	m_strInputPath = strInputPath;
	m_strWeightPath = strWieghtPath;
	m_nWeightType = nWeightType;
	this->m_LprojName = LprojName;
	this->m_useGPU = useGPU;
	m_BoundingBox = box;
	this->m_start = m_BoundingBox.getMinZ();
	this->m_end = m_BoundingBox.getMaxZ();

}

void WorkL3Predict::threadRun()
{
#ifdef AI_VER
	bool res = true;
	const QString LprojPath = m_strWeightPath;
	VOLUME_DATA* vol_dt = m_pVolumeData;

	muint32 cx, cy, cz;
	vol_dt->getLengthForScreen(WT_SAGITTAL, cx, cy, cz);

	// jhc [2021.09.06] - L3 성능 개선 -> cy값이 L3 입력 raw 파일 사이즈와 동일하게 조정.
	cx = m_BoundingBox.getMaxY() - m_BoundingBox.getMinY() + 1;
	cy = m_BoundingBox.getMaxZ() - m_BoundingBox.getMinZ() + 1;
	//	cx = m_BoundingBox.getLimitY();
	//	cy = m_BoundingBox.getLimitZ();

	setProgressValue(0, true);

	int _addValue = 20;
	std::vector< std::vector<int> >salt;

	//if (!((PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_MEDIP_DEEPCATCH)))
	if (m_nWeightType == MFL_Product_DeepCatch || m_nWeightType == MFL_Product_DeepCatch_DeepCatchV2)
	{
		salt = {
			{ 1237511163, 1838418459, 1093931361, 1105727818, 735048887, 772392164, 1310096029, 1921377113,
			863180821, 1687579508, 1370944241, 1562928510, 1634900440, 554518886, 731955677, 1603490642,
			341951092, 377537154, 1206264283, 814105533, 1611262396, 1025494219, 1187428308, 1307761987,
			890281657, 1493280231, 524789833, 156392073, 1651402060, 1521714240, 1544494579, 1854541861 }
		};
	}

	Segmentation::setExtension("mipx");
	std::shared_ptr<Regression> shared_regression;
	shared_regression = std::make_shared<Regression>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;

	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "m_LprojName" << m_LprojName;
	qDebug() << "L3 input path" << m_strInputPath;
	qDebug() << "cy" << cy;
	qDebug() << "cx" << cx;
	std::vector<float>  vecL3Scope;

	if (shared_regression->medipPredict(m_strInputPath.toLocal8Bit().toStdString(), m_useGPU, cy, cx))
	{
		while (true)
		{
			if (vol_dt->threadStop)
			{
				shared_regression->medipStop();
			}
			check = shared_regression->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
				//qDebug() << check.progress;
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}

		vecL3Scope = shared_regression->getResults();
		for (int i = 0; i < vecL3Scope.size(); ++i)
		{
			qDebug() << "L3 Predict dat: " << vecL3Scope[i];
		}

		if (vecL3Scope.size() > 0)
		{
			int nL3Slicenum = (vecL3Scope[0]); // +vecL3Scope[1]) / 2.0;

			if (!ACTION_MANAGER->m_qThreadNext.empty())
				ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<int>(nL3Slicenum);
		}
	}
	else
	{
		check = shared_regression->getCheckData();
		res = false;
	}

	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/AI_error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}

	//

	setProgressValue(100);

	shared_regression.reset();


	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = vecL3Scope.size();
	}

#if !defined(DEV_VER)
	QDir(m_strInputPath).removeRecursively();
#endif

#endif
	emit finished();

	return;
}


WorkAWPredict::WorkAWPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName, BoundingBoxI box, bool useGPU)
{
	m_pVolumeData = pVolumeData;
	m_strInputPath = strInputPath;
	m_strWeightPath = strWieghtPath;
	m_nWeightType = nWeightType;
	this->m_LprojName = LprojName;
	this->m_useGPU = useGPU;
	m_BoundingBox = box;
	this->m_start = m_BoundingBox.getMinZ();
	this->m_end = m_BoundingBox.getMaxZ();

}

void WorkAWPredict::threadRun()
{
#ifdef AI_VER
	bool res = true;
	const QString LprojPath = m_strWeightPath;
	VOLUME_DATA* vol_dt = m_pVolumeData;

	muint32 cx, cy, cz;
	vol_dt->getLengthForScreen(WT_SAGITTAL, cx, cy, cz);

	cx = m_BoundingBox.getLimitY();
	cy = m_BoundingBox.getLimitZ();
	setProgressValue(0, true);

	int _addValue = 20;
	std::vector< std::vector<int> >salt;

	if (m_nWeightType == MFL_Product_DeepCatch || m_nWeightType == MFL_Product_DeepCatch_DeepCatchV2)
	{
		salt = {
			{ 1237511163, 1838418459, 1093931361, 1105727818, 735048887, 772392164, 1310096029, 1921377113,
			863180821, 1687579508, 1370944241, 1562928510, 1634900440, 554518886, 731955677, 1603490642,
			341951092, 377537154, 1206264283, 814105533, 1611262396, 1025494219, 1187428308, 1307761987,
			890281657, 1493280231, 524789833, 156392073, 1651402060, 1521714240, 1544494579, 1854541861 }
		};
	}

	// 	Segmentation::setExtension("mipx");
	// 	std::shared_ptr<Regression> shared_regression;
	// 	shared_regression = std::make_shared<Regression>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> _segment;
	_segment = std::make_shared<Segmentation>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;

	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "m_LprojName" << m_LprojName;
	qDebug() << "AW input path" << m_strInputPath;
	qDebug() << "cy" << cy;
	qDebug() << "cx" << cx;
	//	std::vector<float>  vecL3Scope;

		//if (_segment->medipPredict(m_strInputPath.toLocal8Bit().toStdString(), m_useGPU, cy, cx))
	if (_segment->medipPredict(m_strInputPath.toLocal8Bit().toStdString(), "", m_useGPU, 512, 512, 1, m_start))
	{
		while (true)
		{
			if (vol_dt->threadStop)
			{
				_segment->medipStop();
			}
			check = _segment->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
				//qDebug() << check.progress;
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}

		// 		vecL3Scope = _segment->getResults();
		// 		std::vector<int>  vecL3ScopeInt;
		// 		for (int i = 0; i < vecL3Scope.size(); ++i)
		// 		{
		// 			vecL3ScopeInt.push_back(vecL3Scope[i]);
		// 			qDebug() << "AW Predict dat: " << vecL3Scope[i];
		// 		}
		// 
		// 		if (vecL3ScopeInt.size() > 1)
		// 		{
		// 			if (!ACTION_MANAGER->m_qThreadNext.empty())
		// 				ACTION_MANAGER->m_qThreadNext.front().pThreadResult = std::make_shared<std::vector<int>>(vecL3ScopeInt);
		// 		}
	}
	else
	{
		check = _segment->getCheckData();
		res = false;
	}


	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/AI_error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}

	std::vector<std::vector<unsigned char>> vClassAll = _segment->getVectorResults();
	if (!ACTION_MANAGER->m_qThreadNext.empty())
		ACTION_MANAGER->m_qThreadNext.front().pTempData = std::make_shared<std::vector<std::vector<unsigned char>>>(vClassAll);


	qDebug() << "vClassAll: " << vClassAll.size();

	setProgressValue(100);

	_segment.reset();


	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = vClassAll.size();
	}

#if !defined(DEV_VER)
	QDir(m_strInputPath).removeRecursively();
#endif

#endif
	emit finished();

	return;
}



WorkVBPredict::WorkVBPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName
	, QString targetMaskName, int process, BoundingBoxI box, bool useGPU, int nFilterIdx)
{
	//SUPPORT_DEEPCATCH_VERSION_2
	m_pVolumeData = pVolumeData;
	m_strInputPath = strInputPath;
	m_strWeightPath = strWieghtPath;
	m_nWeightType = nWeightType;
	this->m_LprojName = LprojName;
	this->m_useGPU = useGPU;
	this->m_TargetMaskName = targetMaskName;
	m_BoundingBox = box;
	m_nFilterIdx = nFilterIdx;
	this->m_start = m_BoundingBox.getLimitZ() - m_BoundingBox.getMaxZ() - 1;
	this->m_end = m_BoundingBox.getLimitZ() - m_BoundingBox.getMinZ() - 1;
	m_enumProcessing = process;
}

void WorkVBPredict::setProgressValue(int value, bool init)
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

bool WorkVBPredict::removeMaskList(std::vector<std::string>& nameList, std::vector<std::vector<unsigned char>>& classAll, int process)
{
	std::vector<std::string>& vecClassNames = nameList;
	std::vector<std::vector<unsigned char>>& vClassAll = classAll;

	if (vecClassNames.size() != vClassAll.size()) return false;

	bool res = true;
	std::vector<bool> vecAIResultPass(vecClassNames.size(), false);

#if 1
	if (m_enumProcessing == ACTP_DEEPCATCH_VB2IND_PREDICT)
	{
		const auto num_core = std::thread::hardware_concurrency();
		int n_core = 1;//
		if (num_core >= 3) n_core = 2;
		printf_s("\n ACTP_DEEPCATCH_VB2IND_PREDICT USE PPL Core - %d", n_core);

		int size = vClassAll.size();
		int interval = size / n_core;
		int remain = size % n_core;

		concurrency::parallel_for(0, n_core, [&](int i)
			{
				int start = i * interval;
				int finish = (i + 1) * interval;

				if (i == n_core - 1)
					finish += remain;

				for (int ii = start; ii < finish; ii++)
				{
					for (int jj = 0; jj < vClassAll[ii].size(); jj++)
					{
						if (vClassAll[ii][jj] != 0)
						{
							vecAIResultPass[ii] = true;
							break;
						}
					}
				}
			});

		for (int ii = 0; ii < vecAIResultPass.size(); ii++)
		{
			if (!vecAIResultPass[ii])
			{
				printf_s("\n vecAIResultPass delete - %s \n", vecClassNames[ii].c_str());

				vecClassNames.erase(vecClassNames.begin() + ii);
				vClassAll.erase(vClassAll.begin() + ii);
				vecAIResultPass.erase(vecAIResultPass.begin() + ii);
				ii = 0;
			}
		}

		res = vecAIResultPass.size();
	}
#endif

	if (m_enumProcessing == ACTP_DEEPCATCH_IO_LIVER_SPLEEN)
	{
		for (int ii = 0; ii < vecClassNames.size(); ii++)
		{
			std::string strName = vecClassNames[ii];
			printf_s("\n vecAIName -  %s\n", strName.c_str());

			if (!(strName == DEEPCATCH_WHOLEBODY_MASKNAME_IO_LIVER || strName == DEEPCATCH_WHOLEBODY_MASKNAME_IO_SPLEEN))
			{
				vecAIResultPass[ii] = true;
				printf_s("\n PASS vecAIName -  %s\n", strName.c_str());
			}
		}

		for (int ii = 0; ii < vecAIResultPass.size(); ii++)
		{
			if (vecAIResultPass[ii])
			{
				vecClassNames.erase(vecClassNames.begin() + ii);
				vClassAll.erase(vClassAll.begin() + ii);
				vecAIResultPass.erase(vecAIResultPass.begin() + ii);
				ii = 0;
			}
		}

		res = vecAIResultPass.size();
	}
	return res;
}

void WorkVBPredict::threadRun()
{
	//SUPPORT_DEEPCATCH_VERSION_2
#ifdef AI_VER
	qDebug() << "<<<<<<<<<<<<<<<<<<<< AI thread Run";
	////////////////////////////// 경로 설정
	const QString prePath = m_strInputPath;// STRING_MANAGER->AISegmentationPath + "/predict";
	const QString LprojPath = m_strWeightPath;// STRING_MANAGER->AISegmentationPath + "/weight";

	const QString dataPath = prePath + "/data";//deepdraw_slice_1.raw

	VOLUME_DATA* vol_dt = m_pVolumeData;

	//QDir dir(prePath);
	QDir().mkpath(prePath);
	QDir().mkpath(dataPath);

	qDebug() << "prePath" << prePath;
	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "dataPath" << dataPath;

	printf_s("\n[  ----- WorkVBPredict::threadRun() start = %s ----  ] \n", m_LprojName.toLocal8Bit().toStdString().c_str());

	muint32 cx, cy, cz;
	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);
	setProgressValue(0, true);

	float xSpacing, ySpacing, zSpacing;
	xSpacing = vol_dt->getSpaceX(true);
	ySpacing = vol_dt->getSpaceY(true);
	zSpacing = vol_dt->getSpaceZ(true);

	///////////////////////////////////// make input data
	mint16* maskRaw = new mint16[cx * cy];

	qDebug() << "ACTION_MANAGER->m_vecReservationWork.size() = " << ACTION_MANAGER->GetReservationWorkList().size();
	qDebug() << "m_nFilterIdx" << m_nFilterIdx;
	qDebug() << "m_start" << m_start;
	qDebug() << "m_end" << m_end;

	bool bMEDIP = !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP);
	bool bDeepCatch = !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH);

	//
	bool bDeepCatch_QCTMode = false;
	mask* pMask3D_QCTTrunk = nullptr;
	mask maskBit_QCTTrunk;
	bool bDeepCatch_IOClassificationPredict = false;
	mask* pMask3D_IO = nullptr;
	mask maskBit_IO;
	if (bDeepCatch)
	{
		//// deepcatch && qct mode인 경우 처리 건.
		//if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == ACTION_MANAGER->m_eCurThread)
		//{
		//	bDeepCatch_QCTMode = true;
		//	MaskInfo *pTrunkMask = vol_dt->findMaskInfo(DEEPCATCH_MASKNAME_QCT_TRUNK);
		//	if (pTrunkMask)
		//	{
		//		pMask3D_QCTTrunk = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pTrunkMask->uid));
		//		maskBit_QCTTrunk = vol_dt->getMask(pTrunkMask->uid);
		//	}
		//}
		//// deepcatch && io classification 분류.
		//else if (THREAD_IO_CLASSIFICATION_PREDICT == ACTION_MANAGER->m_eCurThread ||
		//	THREAD_IO_CLASSIFICATION_PREDICT2 == ACTION_MANAGER->m_eCurThread)
		//{
		//	bDeepCatch_IOClassificationPredict = true;
		//	MaskInfo *pIOMask = vol_dt->findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_IO);
		//	if (pIOMask)
		//	{
		//		pMask3D_IO = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pIOMask->uid));
		//		maskBit_IO = vol_dt->getMask(pIOMask->uid);
		//	}

		//}
	}

	//추가
	MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo(m_TargetMaskName);
	if (pMaskInfo == nullptr)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);
	mask* pMask3D = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pMaskInfo->uid));
	mask maskBit = vol_dt->getMask(pMaskInfo->uid);

	bool res = false;
	for (int i = m_start; i <= m_end; i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));

		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				maskRaw[y * cx + x] = -1024; // filter value

				if (pMask3D[i * cy * cx + y * cx + x] & maskBit)
				{
					maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
				}
			}
		}


		QString rawName = dataPath + QString("/deepdraw_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(((float)(i) / m_end) * 20);

		qDebug() << "mask data save complete";

		if (vol_dt->threadStop)
		{
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//20

	qDebug() << ">>>>>>>>>>>>> predit data start";
	qDebug() << "m_start" << m_start;
	qDebug() << "m_end" << m_end;
	qDebug() << "cx" << cx;
	qDebug() << "cy" << cy;
	qDebug() << "cz" << cz;
	qDebug() << "dataPath" << dataPath;
	qDebug() << "prePath" << prePath;
	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "m_LprojName" << m_LprojName;
	qDebug() << ">>>>>>>>>>>>> predit data end";


	if (!res)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	printf_s("\n[  ----- WorkVBPredict::threadRun() predit start = %s ----  ] \n", LprojPath.toLocal8Bit().toStdString().c_str());

	// salt 값 설정
	int _addValue = 20;
	std::vector< std::vector<int> >salt;
	if (m_nWeightType == MFL_Product_COVID19)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::COVID19];
	}
	else if (m_nWeightType == MFL_Product_DeepCatch || m_nWeightType == MFL_Product_DeepCatch_DeepCatchV2)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::DEEPCATCH];
	}
	else if (m_nWeightType == MFL_Common_AI_AIPredict_Predict)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::MEDIP];
	}
	else if (m_nWeightType == MFL_Common_AITrainning_CustomTrainningTool)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::USER_CREATED];
	}
	else if (m_nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
	{
		// 일단 고정 salt 값, 서버 처리되면 salt 값 받아서 처리, salt값은 개인별로 줘야 할듯 함
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::MEDIP_CREDIT];
	}

	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> _segment;
	_segment = std::make_shared<Segmentation>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;
	if (_segment->medipPredict(dataPath.toLocal8Bit().toStdString(), "", m_useGPU, cy, cx, cz, m_start, xSpacing, ySpacing, zSpacing))
	{

		while (true)
		{
			if (vol_dt->threadStop)
			{
				_segment->medipStop();
			}
			check = _segment->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
				//qDebug() << check.progress;
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}

	}
	else
	{
		check = _segment->getCheckData();
		res = false;
	}

	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/AI_error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}

	std::vector<std::string>  vecClassNames = _segment->getClasses();
	std::vector<std::vector<unsigned char>> vClassAll = _segment->getVectorResults();

	res = removeMaskList(vecClassNames, vClassAll, m_enumProcessing);

	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<std::vector<std::string>>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name")
		, std::make_shared<std::vector<std::string>>(vecClassNames)));

	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<std::vector<std::vector<unsigned char>>>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result")
		, std::make_shared<std::vector<std::vector<unsigned char>>>(vClassAll)));

	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<int>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType")
		, std::make_shared<int>(m_nWeightType)));

	int nClassCount = vClassAll.size();
	_segment.reset();

	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = nClassCount;
	}

	QDir(dataPath).removeRecursively();

	printf_s("\n[  ----- WorkVBPredict::threadRun() predit end = %s ----  ] \n", LprojPath.toLocal8Bit().toStdString().c_str());

	qDebug() << "End LprojPath - " << LprojPath;
#endif
	emit finished();

	return;
}



WorkIOClassificationAortaPredict::WorkIOClassificationAortaPredict(VOLUME_DATA* pVolumeData, QString strInputPath, QString strWieghtPath, int nWeightType, QString LprojName
	, BoundingBoxI box, bool useGPU, int nFilterIdx)
{
	//SUPPORT_DEEPCATCH_VERSION_2
	m_pVolumeData = pVolumeData;
	m_strInputPath = strInputPath;
	m_strWeightPath = strWieghtPath;
	m_nWeightType = nWeightType;
	this->m_LprojName = LprojName;
	this->m_useGPU = useGPU;
	//this->m_TargetMaskName = targetMaskName;
	m_BoundingBox = box;
	m_nFilterIdx = nFilterIdx;
	this->m_start = m_BoundingBox.getLimitZ() - m_BoundingBox.getMaxZ() - 1;
	this->m_end = m_BoundingBox.getLimitZ() - m_BoundingBox.getMinZ() - 1;
}

void WorkIOClassificationAortaPredict::setProgressValue(int value, bool init)
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



void WorkIOClassificationAortaPredict::threadRun()
{
	//SUPPORT_DEEPCATCH_VERSION_2
#ifdef AI_VER
	qDebug() << "<<<<<<<<<<<<<<<<<<<< AI thread Run";
	////////////////////////////// 경로 설정
	const QString prePath = m_strInputPath;// STRING_MANAGER->AISegmentationPath + "/predict";
	const QString LprojPath = m_strWeightPath;// STRING_MANAGER->AISegmentationPath + "/weight";

	const QString dataPath = prePath + "/data";//deepdraw_slice_1.raw

	VOLUME_DATA* vol_dt = m_pVolumeData;

	printf_s("\n[  ----- WorkMuscleAbdominalPredict::threadRun() start = %s ----  ] \n", m_LprojName.toLocal8Bit().toStdString().c_str());
	//QDir dir(prePath);
	QDir().mkpath(prePath);
	QDir().mkpath(dataPath);

	qDebug() << "prePath" << prePath;
	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "dataPath" << dataPath;

	muint32 cx, cy, cz;
	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);
	setProgressValue(0, true);

	float xSpacing, ySpacing, zSpacing;
	xSpacing = vol_dt->getSpaceX(true);
	ySpacing = vol_dt->getSpaceY(true);
	zSpacing = vol_dt->getSpaceZ(true);

	///////////////////////////////////// make input data
	mint16* maskRaw = new mint16[cx * cy];

	qDebug() << "ACTION_MANAGER->m_vecReservationWork.size() = " << ACTION_MANAGER->GetReservationWorkList().size();
	qDebug() << "m_nFilterIdx" << m_nFilterIdx;
	qDebug() << "m_start" << m_start;
	qDebug() << "m_end" << m_end;

	bool bMEDIP = !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP);
	bool bDeepCatch = !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2);

	//
	bool bDeepCatch_QCTMode = false;
	mask* pMask3D_QCTTrunk = nullptr;
	mask maskBit_QCTTrunk;
	bool bDeepCatch_IOClassificationPredict = false;
	mask* pMask3D_IO = nullptr;
	mask maskBit_IO;
	if (bDeepCatch)
	{
		//// deepcatch && qct mode인 경우 처리 건.
		//if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == ACTION_MANAGER->m_eCurThread)
		//{
		//	bDeepCatch_QCTMode = true;
		//	MaskInfo *pTrunkMask = vol_dt->findMaskInfo(DEEPCATCH_MASKNAME_QCT_TRUNK);
		//	if (pTrunkMask)
		//	{
		//		pMask3D_QCTTrunk = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pTrunkMask->uid));
		//		maskBit_QCTTrunk = vol_dt->getMask(pTrunkMask->uid);
		//	}
		//}
		//// deepcatch && io classification 분류.
		//else if (THREAD_IO_CLASSIFICATION_PREDICT == ACTION_MANAGER->m_eCurThread ||
		//	THREAD_IO_CLASSIFICATION_PREDICT2 == ACTION_MANAGER->m_eCurThread)
		//{
		//	bDeepCatch_IOClassificationPredict = true;
		//	MaskInfo *pIOMask = vol_dt->findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_IO);
		//	if (pIOMask)
		//	{
		//		pMask3D_IO = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pIOMask->uid));
		//		maskBit_IO = vol_dt->getMask(pIOMask->uid);
		//	}

		//}
	}

	//추가
	//MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo(m_TargetMaskName);
	//if (pMaskInfo == nullptr)
	//{
	//	vol_dt->threadResult = -1;
	//	SAFE_DELETES(maskRaw);
	//	emit finished();
	//	return;
	//}
	printf_s("\n[  ----- WorkMuscleAbdominalPredict::threadRun() ing = %s ----  ] \n", m_LprojName.toLocal8Bit().toStdString().c_str());

	//qDebug() << "mask name:" << QString::fromWCharArray(pMaskInfo->maskName);
	//mask* pMask3D = vol_dt->getMaskDataPoint(vol_dt->GetMaskByteIndex(pMaskInfo->uid));
	//mask maskBit = vol_dt->getMask(pMaskInfo->uid);

	bool res = false;
	for (int i = m_start; i <= m_end; i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));

		for (int y = 0; y < cy; y++)
		{
			for (int x = 0; x < cx; x++)
			{
				//maskRaw[y*cx + x] = -1024; // filter value

				//if (pMask3D[i*cy*cx + y*cx + x] & maskBit)
				{
					maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
				}
			}
		}


		QString rawName = dataPath + QString("/deepdraw_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
			file.remove();

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
			res = file.rename(rawName);

		if (!res)
			break;

		setProgressValue(((float)(i) / m_end) * 20);

		qDebug() << "mask data save complete";

		if (vol_dt->threadStop)
		{
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//20

	qDebug() << ">>>>>>>>>>>>> predit data start";
	qDebug() << "m_start" << m_start;
	qDebug() << "m_end" << m_end;
	qDebug() << "cx" << cx;
	qDebug() << "cy" << cy;
	qDebug() << "cz" << cz;
	qDebug() << "dataPath" << dataPath;
	qDebug() << "prePath" << prePath;
	qDebug() << "LprojPath" << LprojPath;
	qDebug() << "m_LprojName" << m_LprojName;
	qDebug() << ">>>>>>>>>>>>> predit data end";


	if (!res)
	{
		vol_dt->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	// salt 값 설정
	int _addValue = 20;
	std::vector< std::vector<int> >salt;
	if (m_nWeightType == MFL_Product_COVID19)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::COVID19];
	}
	else if (m_nWeightType == MFL_Product_DeepCatch || m_nWeightType == MFL_Product_DeepCatch_DeepCatchV2)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::DEEPCATCH];
	}
	else if (m_nWeightType == MFL_Common_AI_AIPredict_Predict)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::MEDIP];
	}
	else if (m_nWeightType == MFL_Common_AITrainning_CustomTrainningTool)
	{
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::USER_CREATED];
	}
	else if (m_nWeightType == MFL_Common_AI_PredictUsableCount_Credit)
	{
		// 일단 고정 salt 값, 서버 처리되면 salt 값 받아서 처리, salt값은 개인별로 줘야 할듯 함
		salt = PRODUCT_MANAGER->m_mapAI_Salt[(unsigned int)ClientType::MEDIP_CREDIT];
	}

	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> _segment;
	_segment = std::make_shared<Segmentation>(LprojPath.toLocal8Bit().toStdString(), m_LprojName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = _addValue;
	if (_segment->medipPredict(dataPath.toLocal8Bit().toStdString(), "", m_useGPU, cy, cx, cz, m_start, xSpacing, ySpacing, zSpacing))
	{

		while (true)
		{
			if (vol_dt->threadStop)
			{
				_segment->medipStop();
			}
			check = _segment->getCheckData();
			if (check.status == Status::NORMAL) {
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (_addValue < progress) {
					_addValue = progress;//send signal
					setProgressValue(_addValue);
				}
				//qDebug() << check.progress;
			}
			else if (check.status == Status::STOPPING) {
				//printf("Stopping\n");
			}
			else {
				break;
			}
		}

	}
	else
	{
		check = _segment->getCheckData();
		res = false;
	}

	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		//Show exception?
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/AI_error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		res = false;
	}

	std::vector<std::string>  vecClassNames = _segment->getClasses();
	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<std::vector<std::string>>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name")
		, std::make_shared<std::vector<std::string>>(vecClassNames)));

	std::vector<std::vector<unsigned char>> vClassAll = _segment->getVectorResults();
	int nClassCount = vClassAll.size();
	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<std::vector<std::vector<unsigned char>>>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result")
		, std::make_shared<std::vector<std::vector<unsigned char>>>(vClassAll)));

	ACTION_MANAGER->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType"));
	ACTION_MANAGER->m_hashThreadResult.insert(pair<std::string, shared_ptr<int>>(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType")
		, std::make_shared<int>(m_nWeightType)));


	_segment.reset();

	for (int ii = 0; ii < vecClassNames.size(); ii++)
	{
		printf_s("\n [0] - %s", vecClassNames[ii].c_str());
	}

	AIBase::clearCache();
	if (!res)
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		vol_dt->threadResult = nClassCount;
	}

	printf_s("\n[  ----- WorkMuscleAbdominalPredict::threadRun() end = %s ----  ] \n", m_LprojName.toLocal8Bit().toStdString().c_str());

	QDir(dataPath).removeRecursively();

#endif

	emit finished();
	return;
}

