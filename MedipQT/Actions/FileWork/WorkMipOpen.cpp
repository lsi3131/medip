#include "stdafx.h"
#include "WorkMipOpen.h"

#include "defineMipEncoder.h"
#include "medipmipencoder.h"
#include "ProductManager.h"
#include "windowManager.h"

#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/AnalVolumeView.h"

#include "Renderer/MeshTopology.h"

#define MIP_ENCODER_TEST


void WorkMipOpen::updateprogress(int value, void* data)
{
	WorkMipOpen* worker = (WorkMipOpen*)data;
	worker->setProgressValue(value / 10 * 3);
}

void WorkMipOpen::updateprogress2(int value, void* data)
{
	WorkMipOpen* worker = (WorkMipOpen*)data;
	worker->setProgressValue(value / 10 * 7 + 30);
}

WorkMipOpen::WorkMipOpen(const QString& filepath, WindowManager* pWinManager, VOLUME_DATA* pVolume, MeshData* pMeshData, ProductManager* pProduct)
{
	m_pWinManager = pWinManager;
	m_pMeshData = pMeshData;
	m_pVolumData = pVolume;
	m_filePath = filepath;
	m_pProduct = pProduct;
}

WorkMipOpen::~WorkMipOpen()
{
}

void WorkMipOpen::setProgressValue(int value, bool init)
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

void WorkMipOpen::threadRun()
{
#ifdef MIP_ENCODER_TEST
	MIP_ENCODER::ProjectDataInfo proj;

	int progressPercent = 0;
	progressPercent += 10;//10
	setProgressValue(progressPercent, true);
	progressPercent += 10;//20

	setProgressValue(progressPercent);

	if (MIP_ENCODER::ERROR_MESSAGE err = MIP_ENCODER_SINGLTON->LoadProjectFile(m_filePath, &proj))
	{
		m_pVolumData->threadResult = err;
		emit finished();
		return;
	}

	if (!(m_pProduct->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProduct->m_strMedipDeepCatch) ||
		!(m_pProduct->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProduct->m_strMedipDeepCatchV2))
	{
		m_pWinManager->SetPredictComplete(proj.stPredictOptInfo.bIsPredictComplete);
	}
	else
	{
		m_pWinManager->SetPredictComplete(true);
	}

	DcmtkSeriesInfo info;
	std::memset(&info, 0x00, sizeof(DcmtkSeriesInfo));

	emit resetRC();

	progressPercent += 10;//30

	////////////////////////////////////////////////////////// chunk 
	bool ret = false;
	ret = m_pVolumData->createData(proj.dataCX, proj.dataCY, proj.dataCZ, proj.spaceX, proj.spaceY, proj.spaceZ, true, false);

	if (ret)
	{
		// mask1, PC_MASK
		int nMaskByteIdxCnt = (proj.maskInfoSize + 9) / 8;	// 0:1, 7:2, 15:3, 23:4
		for (int i = 0; i < nMaskByteIdxCnt; ++i)
		{
			if (proj.vecMaskData.empty() == false)
			{
				std::memcpy((char*)m_pVolumData->getMaskDataPoint(i), &proj.vecMaskData[i][0], proj.maskSize);
			}
		}

		setProgressValue(progressPercent);
		progressPercent += 10;//40
		// volume, HU, PC_VOLUME
		if (proj.right_hand_coord != 0)
		{
			std::memcpy((char*)m_pVolumData->getHUDataPoint(), &proj.vecHUData[0], proj.volumeSize);

			//m_pVolumData->updateMPRData();
		}
		else
		{
			std::memcpy((char*)m_pVolumData->getHUFlipDataPoint(), &proj.vecHUData[0], proj.volumeSize);
			int zVal, dx, dy, dz;
			dx = proj.dataCX;
			dy = proj.dataCY;
			dz = proj.dataCZ;
			for (int z = 0; z < dz; z++)
			{
				zVal = dz - z - 1;
				for (int y = 0; y < dy; y++)
				{
					for (int x = 0; x < dx; x++)
					{
						m_pVolumData->setData(x, y, z, m_pWinManager->Get16((zVal * dx * dy) + (y * dx) + x));
					}
				}
			}
			SAFE_DELETES(m_pVolumData->pData3D_HU_Flip);
		}
	}


	setProgressValue(progressPercent);
	progressPercent += 10;//50

	 // PC_ANNO_PATH
	m_pWinManager->aniLine.clear();
	m_pWinManager->aniCount.clear();

	int vecCnt = proj.vecAniLine.size();
	for (int i = 0; i < vecCnt; ++i)
	{
		if (proj.vecAniLine[i].nPoint > 0)
		{
			m_pWinManager->aniCount.push_back(proj.vecAniLine[i].nPoint);
			//m_pWinManager->aniLine.resize(vecCnt);
			//std::copy(proj.vecAniLine.begin(), proj.vecAniLine.end(), m_pWinManager->aniLine.begin());
			for (int j = 0; j < proj.vecAniLine[i].nPoint * 3; j += 3)
			{
				m_pWinManager->aniLine.append(QVector3D(proj.vecAniLine[i].vecPoints[j], proj.vecAniLine[i].vecPoints[j + 1], proj.vecAniLine[i].vecPoints[j + 2]));
			}
		}
	}

	if (vecCnt == 0)
	{
		m_pWinManager->aniCount.push_back(0);
	}

	/////////////////////////////
		//proj.setModalityType(proj.detail.modality_type, &info);
	info.modality_ = (MIP_ENCODER_SINGLTON->GetStringFromModalityType(proj.modality_type)).toLocal8Bit().toStdString();

	info.description_ = proj.strSeries_desc;
	info.studyDescription = proj.strStudy_desc;;

	//std::string strImgPos(std::to_string(proj.imgPos[0]) + "/" + std::to_string(proj.imgPos[1]) + "/" + std::to_string(proj.imgPos[2]));
	info.imagePosition = std::string(std::to_string(proj.imgPos[0]) + "/" + std::to_string(proj.imgPos[1]) + "/" + std::to_string(proj.imgPos[2]));

	info.kvp_ = std::to_string(proj.nKVP);
	info.ma_ = std::to_string(proj.nmA);
	info.Manufacturer = proj.strManufacturer;
	info.manufacturerModel = proj.strManufacturerModel;
	info.convolutionkernel = proj.strKernel;

	info.strStudyUID = proj.dicomElementInfo.StudyInstanceUID;
	info.strSeriesUID = proj.dicomElementInfo.SeriesInstanceUID;
	info.examID = proj.dicomElementInfo.ExamID;
	info.accessionNumber = proj.dicomElementInfo.AccessionNumber;
	info.studyDate_ = proj.dicomElementInfo.StudyDate;
	info.studyTime_ = proj.dicomElementInfo.StudyTime;
	info.patientId_ = proj.dicomElementInfo.PatientID;
	info.patientsName_ = proj.dicomElementInfo.PatientName;
	info.sex_ = proj.dicomElementInfo.PatientSex;
	info.age_ = proj.dicomElementInfo.PatientAge;
	info.patientWeight_ = proj.dicomElementInfo.PatientWeight;
	info.units_ = proj.dicomElementInfo.Units;
	info.radionuclideTotalDose_ = proj.dicomElementInfo.RadionuclideTotalDose;
	info.radionuclideHalfLife = proj.dicomElementInfo.RadionuclideHalfLife;
	info.radiopharmaceuticalStartTime_ = proj.dicomElementInfo.RadiopharmaceuticalStartTime;
	info.acquisitionTime_ = proj.dicomElementInfo.AcquisitionTime;
	info.frameOfReferenceUID_ = proj.dicomElementInfo.FrameOfReferenceUID;
	info.seriesDate = proj.dicomElementInfo.SeriesDate;

	// othrer init
	m_pWinManager->GetDicomInfo(&info);
	emit resetUI();

	if (m_pWinManager->mainSegmentWidget)
	{
		m_pWinManager->mainSegmentWidget->setWindowsZoomFactor(10 * proj.spaceX, 10 * proj.spaceY, 10 * proj.spaceZ);
	}
	if (m_pWinManager->mainTAWidget)
	{
		m_pWinManager->mainTAWidget->setWindowsZoomFactor(10 * proj.spaceX, 10 * proj.spaceY, 10 * proj.spaceZ);
	}

	m_pVolumData->setBoundingBox(proj.box);
	///////////////////////////////////////////////

	setProgressValue(progressPercent);
	progressPercent += 10;//60
	// MaskInfo
	if (proj.maskInfoSize)	m_pVolumData->clearMaskInfo();
	for (int n = 0; n < proj.maskInfoSize; n++)
	{
		//if (n < MASK_SECOND_MAX) break;

		MaskInfo info;

		info.color = proj.vecMaskInfo[n].color;
		info.color.a = 255;
		memcpy_s(info.maskName, sizeof(info.maskName), proj.vecMaskInfo[n].maskName, sizeof(proj.vecMaskInfo[n].maskName));
		info.mask_id = proj.vecMaskInfo[n].mask_id;
		info.uid = proj.vecMaskInfo[n].uid;
		if (info.uid >= MASK_SECOND_MAX && proj.vecMaskInfo[n].mask_id2 == 0)
			info.mask_id2 = VM_MASK0 << ((info.uid - (MASK_SECOND_MAX)) % 8);
		else
			info.mask_id2 = proj.vecMaskInfo[n].mask_id2;
		info.show = proj.vecMaskInfo[n].show;

		info.meshConnected = proj.vecMaskInfo[n].meshConnected;
		info.layerAlpha = proj.vecMaskInfo[n].layerCustom ? proj.vecMaskInfo[n].layerAlpha : 255;
		info.bIsUseCredit = proj.vecMaskInfo[n].reserve_bool[0];
		m_pVolumData->insertMaskInfo(n, info);
	}

	setProgressValue(progressPercent);
	progressPercent += 10;//70
	// 	PC_WINDOWINFO
	vecCnt = proj.vecWindowInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			WINDOW_TYPE type = (WINDOW_TYPE)proj.vecWindowInfo[n].windowType;
			MainSegmentWidget* mainSegmentWidget = m_pWinManager->mainSegmentWidget;

			if (mainSegmentWidget)
			{
				WindowBase* win = mainSegmentWidget->getWindow(type);
				if (win)
				{
					win->setSlicePosition(proj.vecWindowInfo[n].posX, proj.vecWindowInfo[n].posY);
					win->setZoomFactor(proj.vecWindowInfo[n].zoomFactorX, proj.vecWindowInfo[n].zoomFactorY);
					//return info.depth;
				}
			}

			if (m_pWinManager->mainTAWidget)
			{
				WindowBase* pRadiomics = m_pWinManager->mainTAWidget->getWindow(type);
				pRadiomics->setSlicePosition(proj.vecWindowInfo[n].posX, proj.vecWindowInfo[n].posY);
				pRadiomics->setZoomFactor(proj.vecWindowInfo[n].zoomFactorX, proj.vecWindowInfo[n].zoomFactorY);
				//return info.depth;
			}

			if (proj.vecWindowInfo[n].depth < 0.0f)
			{
				//file.close();
				emit finished();
				return;
			}
			else
			{
				emit setDepth((int)type, proj.vecWindowInfo[n].depth);
			}
		}
	}

	// wheel zoom 설정되어 있는데로 setWheelSliderFunc 을 호출
	emit wheelZoom(m_pWinManager->getWheelZoom());

	initMeshData(proj);

	setProgressValue(progressPercent);
	progressPercent += 10;//80
	// PC_ANNO_TEXT
	vecCnt = proj.vecAnnoTextInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			int isHidden = proj.vecAnnoTextInfo[n].isHidden;
			if (!(isHidden <= 1 && isHidden >= 0))
				proj.vecAnnoTextInfo[n].isHidden = 0;

			mip::VECTOR3 v_volume(
				proj.vecAnnoTextInfo[n].posX,
				proj.vecAnnoTextInfo[n].posY,
				proj.vecAnnoTextInfo[n].posZ
			);

			mip::VECTOR3 v_world = getVolumeToWorld(m_pVolumData, v_volume.x, v_volume.y, v_volume.z);


			AnnoString* annoString = new AnnoString(
				v_world,
				Annotation::DRAWING_FINISHED,
				QString((QChar*)proj.vecAnnoTextInfo[n].text),
				proj.vecAnnoTextInfo[n].size,
				COLOR(proj.vecAnnoTextInfo[n].color),
				proj.vecAnnoTextInfo[n].type,
				proj.vecAnnoTextInfo[n].isHidden);
			m_pWinManager->anotationList.push_back(annoString);

		}
	}

	// PC_ANNO_LEN
	vecCnt = proj.vecAnnoLengthInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			AnnoLength* annoLen = new AnnoLength(
				mip::VECTOR3(proj.vecAnnoLengthInfo[n].posX1, proj.vecAnnoLengthInfo[n].posY1, proj.vecAnnoLengthInfo[n].posZ1),
				mip::VECTOR3(proj.vecAnnoLengthInfo[n].posX2, proj.vecAnnoLengthInfo[n].posY2, proj.vecAnnoLengthInfo[n].posZ2),
				Annotation::EState::DRAWING_FINISHED,
				COLOR(proj.vecAnnoLengthInfo[n].color),
				proj.vecAnnoLengthInfo[n].isHidden);
			m_pWinManager->anotationList.push_back(annoLen);
		}
	}

	// PC_ANNO_ANGLE
	vecCnt = proj.vecAnnoAngleInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			AnnoAngle* annoAngle = new AnnoAngle(
				mip::VECTOR3(proj.vecAnnoAngleInfo[n].posX1, proj.vecAnnoAngleInfo[n].posY1, proj.vecAnnoAngleInfo[n].posZ1),
				mip::VECTOR3(proj.vecAnnoAngleInfo[n].posX2, proj.vecAnnoAngleInfo[n].posY2, proj.vecAnnoAngleInfo[n].posZ2),
				mip::VECTOR3(proj.vecAnnoAngleInfo[n].posX3, proj.vecAnnoAngleInfo[n].posY3, proj.vecAnnoAngleInfo[n].posZ3),
				Annotation::EState::DRAWING_FINISHED,
				COLOR(proj.vecAnnoAngleInfo[n].color),
				proj.vecAnnoAngleInfo[n].isHidden);
			m_pWinManager->anotationList.push_back(annoAngle);
		}
	}

	// PC_ANNO_OVAL
	vecCnt = proj.vecAnnoOvalInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			/*
				<이상일 대리>
				현재 사용되지 않아서 주석 처리
			*/
			//AnnoOval * annoOval = new AnnoOval(mip::VECTOR3(proj.vecAnnoOvalInfo[n].posX1, proj.vecAnnoOvalInfo[n].posY1, proj.vecAnnoOvalInfo[n].posZ1),
			//	mip::VECTOR3(proj.vecAnnoOvalInfo[n].posX2, proj.vecAnnoOvalInfo[n].posY2, proj.vecAnnoOvalInfo[n].posZ2)
			//	, COLOR(proj.vecAnnoOvalInfo[n].color), proj.vecAnnoOvalInfo[n].isHidden);
			//m_pWinManager->anotationList.push_back(annoOval);
		}
	}

	// PC_ANNO_ARROW
	vecCnt = proj.vecAnnoArrowInfo.size();
	if (vecCnt > 0)
	{
		for (int n = 0; n < vecCnt; ++n)
		{
			AnnoArrow* annoLen = new AnnoArrow(
				mip::VECTOR3(proj.vecAnnoArrowInfo[n].posX1, proj.vecAnnoArrowInfo[n].posY1, proj.vecAnnoArrowInfo[n].posZ1),
				mip::VECTOR3(proj.vecAnnoArrowInfo[n].posX2, proj.vecAnnoArrowInfo[n].posY2, proj.vecAnnoArrowInfo[n].posZ2),
				Annotation::EState::DRAWING_FINISHED,
				COLOR(proj.vecAnnoArrowInfo[n].color),
				proj.vecAnnoArrowInfo[n].isHidden);
			m_pWinManager->anotationList.push_back(annoLen);
		}
	}

	// PC_ANNO_NAME
	AnnoName* annoName = new AnnoName(COLOR(0, 0, 0, 0));
	m_pWinManager->anotationList.push_back(annoName);

	// PC_THUMBNAIL
	vecCnt = proj.vecThumbnail.size();
	if (vecCnt > 0)
	{
		int _size = vecCnt;

		QImage img(&proj.vecThumbnail[0], 100, 100, QImage::Format_RGBA8888);
		m_pWinManager->setThumbnail(img);
	}

	// PC_CAPTURE_IMG
	vecCnt = proj.vecCaptureImgInfo.size();
	if (vecCnt > 0)
	{
		for (int i = 0; i < vecCnt; i++)
		{
			int _size = proj.vecCaptureImgInfo[i].size;
			int w = proj.vecCaptureImgInfo[i].width;
			int h = proj.vecCaptureImgInfo[i].height;

			QImage drawImg = QImage(&proj.vecCaptureImgInfo[i].data[0], w, h, QImage::Format_RGBA8888);
			QImage* img = new QImage(w, h, QImage::Format_RGBA8888);
			QPainter p(img);
			p.drawImage(0, 0, drawImg);
			p.end();

			m_pWinManager->captureList.push_back(img);
		}
	}

	//  PC_STUDY_DESC
	vecCnt = proj.strStudy_desc.size();
	if (vecCnt > 0)
	{
		info.studyDescription = proj.strStudy_desc;
	}

	// PC_SERIES_DESC
	vecCnt = proj.strSeries_desc.size();
	if (vecCnt > 0)
	{
		info.description_ = proj.strSeries_desc;
	}

	// PC_REPORT
	vecCnt = proj.vecImageInfo.size();
	if (vecCnt > 0)
	{
		//Image Read
		/*if (!proj.updateReport)
		{
			proj.detail.reportInfo.imageCount = proj.detail.reserve[46];
			proj.detail.reportInfo.htmlSize = proj.detail.reserve[47];
		}*/

		int imageCnt = proj.reportInfo.imageCount;
		std::vector<std::pair<QImage, QString>> image_list;
		image_list.reserve(imageCnt);
		for (int i = 0; i < imageCnt; ++i)
		{
			//proj.reportImageRead(file, image_list);
			int _size = proj.vecImageInfo[i].size;
			int w = proj.vecImageInfo[i].width;
			int h = proj.vecImageInfo[i].height;
			int nameLength = proj.vecImageInfo[i].nameLength;
			//std::string str(proj.vecImageInfo[i].name.begin(), proj.vecImageInfo[i].name.end());
			QString imgName = QString::fromStdString(proj.vecImageInfo[i].name);
			// Qstring을 저장하는 방식에서 기존방법과 차이가남
			// char *text = new char();에서 Qstring strName = QString(text) 했을 때 제일 끝에 '\0' 값이 안들어감
			// QString의 '\0' 삭제
			imgName.remove(imgName.size() - 1, 1);

			QImage image(&proj.vecImageInfo[i].data[0], w, h, QImage::Format_ARGB32);
			image_list.push_back(std::make_pair(image.copy(), imgName));
		}

		//HTML Read
		QString html = QString::fromLocal8Bit(&proj.vecReportHtml[0]);
		if ((m_pProduct->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH)
			|| (m_pProduct->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)
			)
			m_pWinManager->setTempReportData(QFileInfo(m_filePath).baseName(), image_list, html);

		setProgressValue(progressPercent);
		progressPercent += 10;//90
	}

	ACTION_MANAGER->DeepcatchReportPredictedInfoSafeDelete();
	if (proj.stPredictOptInfo.bIsPredictComplete)
	{
		DEEPCATCH_REPORT_PREDICT_INFO* pPredictedData = ACTION_MANAGER->DeepcatchReportPredictedInfoSafeCreate();
		pPredictedData->nAxialDepth = proj.stPredictOptInfo.nL3SliceNum;
		pPredictedData->nStartAxialDepth = proj.stPredictOptInfo.nAWSliceStartNum;
		pPredictedData->nEndAxialDepth = proj.stPredictOptInfo.nAWSliceEndNum;

		pPredictedData->stPredictOpt.nProjectType = proj.stPredictOptInfo.nProjectType;
		pPredictedData->stPredictOpt.UNETType = proj.stPredictOptInfo.nUNET;
		pPredictedData->stPredictOpt.contrastType = proj.stPredictOptInfo.nContrast;
		pPredictedData->stPredictOpt.AWConfirmType = proj.stPredictOptInfo.nAWPredict;

		pPredictedData->stPredictOpt.genderType = proj.stPredictOptInfo.nGender;
		pPredictedData->stPredictOpt.strHeight = QString::number(proj.stPredictOptInfo.nHeight);
		pPredictedData->stPredictOpt.strWeight = QString::number(proj.stPredictOptInfo.nWeight);
		pPredictedData->stPredictOpt.singleSliceType = proj.stPredictOptInfo.nIsL3Auto;
		pPredictedData->stPredictOpt.multiSliceType = proj.stPredictOptInfo.nIsAWAuto;
		pPredictedData->stPredictOpt.preferenceType = proj.stPredictOptInfo.nReportSliceType;

		QString strDate(QString::number(proj.stPredictOptInfo.nPredictYear) + "-" + QString::number(proj.stPredictOptInfo.nPredictMonth) + "-" + QString::number(proj.stPredictOptInfo.nPredictDay));
		pPredictedData->stPredictOpt.strReportingDate = strDate;

		pPredictedData->stPredictOpt.AdditionalOptions = proj.stPredictOptInfo.nAdditionalOptions;
		pPredictedData->stPredictOpt.bIOClassificationPredict = proj.stPredictOptInfo.nIOClassification > 0 ? true : false;
		pPredictedData->stPredictOpt.bMuscleQualityMap = proj.stPredictOptInfo.nMuscleQualityMap > 0 ? true : false;

		// PC_PREDICT_UID
		vecCnt = proj.mapPredictResultUID.size();
		if (vecCnt > 0)
		{
			for (auto it = proj.mapPredictResultUID.begin(); it != proj.mapPredictResultUID.end(); ++it)
			{
				pPredictedData->mapDeepCatchPredictResultRoi[QString::fromStdString(it->first)] = it->second;
			}
		}
	}

	// PC_AI_RESULT
	vecCnt = proj.vecAIResultInfo.size();
	//printf("\n Medip Load vecAIResultInfo.size() _uid %d ", vecCnt);
	if (vecCnt > 0)
	{
		for (int ii = 0; ii < proj.vecAIResultInfo.size(); ii++)
		{
			if (proj.vecAIResultInfo[ii].AI_result.size() == 0) continue;

			m_pVolumData->m_vecAIResultData.emplace_back(proj.vecAIResultInfo[ii].uid, proj.vecAIResultInfo[ii].AI_result);
			m_pVolumData->m_vecAIOutset.emplace_back(proj.vecAIResultInfo[ii].uid, proj.vecAIResultInfo[ii].outSetVal);

			//printf("\n Medip Load proj.vecAIResultInfo[%d].uid %d ", ii, proj.vecAIResultInfo[ii].uid);
			//printf("\n Medip Load proj.vecAIResultInfo[%d].outSetVal %d ", ii, proj.vecAIResultInfo[ii].outSetVal);
			//printf("\n Medip Load proj.vecAIResultInfo[%d].AI_result_Size %d ", ii, proj.vecAIResultInfo[ii].AI_result_Size);
		}
	}

	/////////////////////////////////////////// detail

	// image position, orientation
	m_pVolumData->setImgPos(proj.imgPos[0], proj.imgPos[1], proj.imgPos[2]);
	bool init = proj.imgOrientation[0] == mip::VECTOR3().Zero;
	m_pVolumData->setImgOrientation(true, proj.imgOrientation[0], init);
	init = proj.imgOrientation[1] == mip::VECTOR3().Zero;
	m_pVolumData->setImgOrientation(false, proj.imgOrientation[1], init);


	// gamma
	bool rst = true;
	if (proj.fGamma > 0.0f)
		rst = false;
	m_pWinManager->setGamma(proj.fGamma);
	emit setGamma(rst ? rst : proj.isGamma, rst);


	// seed color
	for (int i = 0; i < 2; i++)
	{
		COLOR _col;
		_col.allcolor = proj.seedColor[i];
		m_pWinManager->setSeedColor(_col, i);
	}

	m_pWinManager->GetDicomInfo(&info);
	emit UpdateSummary();

	m_pVolumData->getAllMaskVoxel();
	//	m_pVolumData->getAllMaskBoundingBox();
	emit UpdateUI();	//	m_pWinManager->updateUI();

	m_pVolumData->setHuMin(proj.volumeHUMin);
	m_pVolumData->setHuMax(proj.volumeHUMax);
	m_pVolumData->setDownScaledCnt(proj.downScaleCnt);
	m_pWinManager->setDefaultLevel(proj.nDefaultLevel);
	m_pWinManager->setDefaultWidth(proj.nDefaultWidth);
	setProgressValue(progressPercent);
	progressPercent += 10;//100
	if (!proj.SelectedCustomPreset)
		proj.CuspreVal = -1;
	else
	{
		if (m_pWinManager->getPresetCount() <= proj.CuspreVal)
			proj.CuspreVal = -1;
	}

	if (!proj.SelectedCustomVolumePreset)
		proj.CusVolumepreVal = -1;
	else
	{
		if (m_pWinManager->getPresetCount() <= proj.CusVolumepreVal)
			proj.CusVolumepreVal = -1;
	}

	emit setPreset(true, proj.SelectedvolumePreSet, proj.CusVolumepreVal, proj.volumeWidth, proj.volumeLevel);
	emit setPreset(false, proj.SelectedPreSet, proj.CuspreVal, proj.windowWidth, proj.windowLevel);

	if (m_pWinManager->mainSegmentWidget)
	{
		VolumeView* volumeView = static_cast<VolumeView*>(m_pWinManager->mainSegmentWidget->getViewVolume());
		if (NULL != volumeView)
		{
			if (proj.volumeRotation.x != 0 || proj.volumeRotation.y != 0 ||
				proj.volumeRotation.z != 0 || proj.volumeRotation.w != 0)
			{
				volumeView->setVolRotation(proj.volumeRotation);
			}

			volumeView->setVolTranslation(proj.volumeTranslation);
			volumeView->setZoom(proj.volumeZoom);
		}
	}

	m_pVolumData->forceUpdateMaskVolume();
	//setProgressValue(_progress);
	//_progress += 10;//90
	ACTION_MANAGER->clear();

	emit resetRG();

	emit createHistogram();

	emit resetRG();

	emit resetWork();

	emit UpdateReport();

	if (m_pWinManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		if (m_pWinManager->mainAnalWidget)
		{
			AnalVolumeView* volumeView = (m_pWinManager->mainAnalWidget->getViewVolume());
			if (NULL != volumeView)
			{
				if (proj.volumeRotation.x != 0 || proj.volumeRotation.y != 0 ||
					proj.volumeRotation.z != 0 || proj.volumeRotation.w != 0)
				{
					volumeView->setVolRotation(proj.volumeRotation);
				}
				volumeView->setVolTranslation(proj.volumeTranslation);
				volumeView->setZoom(proj.volumeZoom);
			}
		}
		//if (m_pWinManager->mainTAWidget)
		//{
		//	AnalVolumeView *volumeView = (m_pWinManager->mainTAWidget->getViewVolume());
		//	if (NULL != volumeView)
		//	{
		//		if (proj.volumeRotation.x != 0 || proj.volumeRotation.y != 0 ||
		//			proj.volumeRotation.z != 0 || proj.volumeRotation.w != 0)
		//		{
		//			volumeView->setVolRotation(proj.volumeRotation);
		//		}
		//		volumeView->setVolTranslation(proj.volumeTranslation);
		//		volumeView->setZoom(proj.volumeZoom);
		//	}
		//}
	}

	//setProgressValue(_progress);
	//_progress += 10;//100
	emit updateAnno();

	m_pVolumData->threadResult = 1;
	setProgressValue(progressPercent);
	//	_progress += 10;
#else

	bool r = false;
	QFile file(m_filePath);
	DcmtkSeriesInfo info;

	memset(&info, 0x00, sizeof(DcmtkSeriesInfo));
	if (file.exists() == false)
	{
		emit finished();
		return;
	}

	if (!file.open(QFile::ReadOnly))
	{
		emit finished();
		return;
	}
	int _progress = 0;
	setProgressValue(_progress, true);
	_progress += 10;//10
	ProjectFile proj;
	mint32 _AllSize = file.size();
	file.read((char*)&(proj.head), sizeof(ProjectHead));

	if (proj.head.version != MIP20_FILE_VERSION && proj.head.version != OLD_FILE_VERSION)
	{
		file.close();
		emit finished();
		return;
	}


	//	if (proj.head.version == FILE_VERSION)
	//	{
	//#ifndef DEV_VER
	//		if (_AllSize != proj.head.fileSize)
	//		{
	//			file.close();
	//			emit finished();
	//			return;
	//		}
	//#endif
	//	}


	if (m_pVolumData->isValidate() == false && proj.head.filetype == PT_MASK_ONLY)
	{
		file.close();
		emit finished();
		return;
	}

	file.read((char*)&(proj.detail), proj.head.detailSize);

	emit resetRC();

	r = m_pVolumData->createData(proj.detail.dataCX, proj.detail.dataCY, proj.detail.dataCZ,
		proj.detail.spaceX, proj.detail.spaceY, proj.detail.spaceZ, true, false);

	//if (proj.detail.chunkInfo[PROJ_CHUNK::PC_IMG_POSITION]) // 필요없는 코드, 필요없는 enum
	//{
	m_pVolumData->setImgPos(proj.detail.imgPos[0], proj.detail.imgPos[1], proj.detail.imgPos[2]);
	/*std::string strImgPos = std::to_string(proj.detail.imgPos[0])+ std::string("/")
		+ std::to_string(proj.detail.imgPos[1]) + std::string("/")
		+ std::to_string(proj.detail.imgPos[2]);

	info.imagePosition = strImgPos;*/
	//}

	setProgressValue(_progress);
	_progress += 10;//20
	if (r == false) return;

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_MASK])
	{
		if (proj.detail.right_hand_coord != 0)
			file.read((char*)m_pVolumData->getMaskDataPoint(), proj.detail.maskSize);
		else
		{
			mask* buff = NULL;
			try
			{
				buff = new mask[proj.detail.dataLenth];
			}
			catch (...)
			{
				emit clearVolume();
				file.close();
				emit finished();
				return;
			}

			memset(buff, 0, proj.detail.maskSize);
			mask* orgBuff = m_pVolumData->getMaskDataPoint();
			mask val;
			file.read((char*)orgBuff, proj.detail.maskSize);
			int zVal, dx, dy, dz;
			int index;
			dx = proj.detail.dataCX;
			dy = proj.detail.dataCY;
			dz = proj.detail.dataCZ;
			for (int z = 0; z < dz; z++)
			{
				zVal = dz - z - 1;
				for (int y = 0; y < dy; y++)
				{
					for (int x = 0; x < dx; x++)
					{
						index = (z * dx * dy) + (y * dx) + x;
						if ((val = orgBuff[index]) != 0)
						{
							index = (zVal * dx * dy) + (y * dx) + x;
							buff[index] = val;
						}
					}
				}
			}
			memcpy(m_pVolumData->getMaskDataPoint(), buff,
				proj.detail.maskSize);
			SAFE_DELETES(buff);
		}
	}
	setProgressValue(_progress);
	_progress += 10;//30

	//DATA_CONTEXT->volume_data.createTestTexture();

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_VOLUME])
	{
		if (proj.detail.right_hand_coord != 0)
		{
			file.read((char*)m_pVolumData->getHUDataPoint(), proj.detail.volumeSize);
			m_pVolumData->updateMPRData();
		}
		else
		{
			file.read((char*)m_pVolumData->getHUFlipDataPoint(), proj.detail.volumeSize);
			int zVal, dx, dy, dz;
			dx = proj.detail.dataCX;
			dy = proj.detail.dataCY;
			dz = proj.detail.dataCZ;
			for (int z = 0; z < dz; z++)
			{
				zVal = dz - z - 1;
				for (int y = 0; y < dy; y++)
				{
					for (int x = 0; x < dx; x++)
					{
						m_pVolumData->setData(x, y, z, m_pWinManager->Get16((zVal * dx * dy) + (y * dx) + x));
					}
				}
			}
			SAFE_DELETES(m_pVolumData->pData3D_HU_Flip);
		}
	}

	setProgressValue(_progress);
	_progress += 10;//40
	m_pWinManager->aniLine.clear();
	m_pWinManager->aniCount.clear();

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_PATH])
	{
		for (int i = 0; i <= proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_PATH]; i++)
			proj.annoPathRead(file);
	}
	else
	{
		m_pWinManager->aniCount.push_back(0);
	}

	proj.setModalityType(proj.detail.modality_type, &info);


	if (proj.detail.series_desc)
	{
		info.description_.assign(20, 0);
		std::memcpy(&info.description_[0], &proj.detail.series_desc[0], 20);
		//info.description_ = proj.detail.series_desc; // 버그성임 문자열 맨 뒤에 "\0" 없음
	}

	if (proj.detail.study_desc)
	{
		info.studyDescription.assign(20, 0);
		std::memcpy(&info.studyDescription[0], &proj.detail.study_desc[0], 20);
		//info.studyDescription = proj.detail.study_desc;
	}

	// othrer init
	m_pWinManager->GetDicomInfo(&info);
	emit resetUI();

	if (m_pWinManager->mainSegmentWidget)
	{
		m_pWinManager->mainSegmentWidget->setWindowsZoomFactor(10 * proj.detail.spaceX, 10 * proj.detail.spaceY, 10 * proj.detail.spaceZ);
	}

	m_pVolumData->setBoundingBox(proj.detail.box);

	if (proj.detail.maskInfoSize)	m_pVolumData->clearMaskInfo();
	for (int n = 0; n < proj.detail.maskInfoSize; n++)
	{
		if (n >= MASK_SECOND_MAX) break;

		MaskInfo info;

		info.color = proj.detail.maskInfo[n].color;
		info.color.a = 255;
		memcpy_s(info.maskName, sizeof(info.maskName), proj.detail.maskInfo[n].maskName, sizeof(proj.detail.maskInfo[n].maskName));
		info.mask_id = proj.detail.maskInfo[n].mask_id;
		info.uid = proj.detail.maskInfo[n].uid;
		if (info.uid >= MASK_SECOND_MAX && proj.detail.maskInfo[n].mask_id2 == 0)
			info.mask_id2 = VM_MASK0 << ((info.uid - (MASK_SECOND_MAX)) % 8);
		else
			info.mask_id2 = proj.detail.maskInfo[n].mask_id2;
		info.show = proj.detail.maskInfo[n].show;

		info.meshConnected = proj.detail.maskInfo[n].meshConnected;
		info.layerAlpha = proj.detail.maskInfo[n].layerCustom ? proj.detail.maskInfo[n].layerAlpha : 255;
		m_pVolumData->insertMaskInfo(n, info);
	}

	setProgressValue(_progress);
	_progress += 10;//50
	if (proj.detail.chunkInfo[PC_WINDOWINFO])
	{
		int count = proj.detail.chunksubInfo[PC_WINDOWINFO];
		for (int n = 0; n < count; ++n)
		{
			WINDOW_TYPE type;
			float depth = proj.windowInfoRead(file, type);

			if (depth < 0.0f)
			{
				file.close();
				emit finished();
				return;
			}
			else
			{
				emit setDepth((int)type, depth);
			}
		}
	}

	setProgressValue(_progress);
	_progress += 10;//60
	emit wheelZoom(m_pWinManager->getWheelZoom());
	if (proj.detail.chunkInfo[PC_ANNO_TEXT])
	{
		int count = proj.detail.chunksubInfo[PC_ANNO_TEXT];
		for (int n = 0; n < count; ++n)
		{
			proj.annoTextRead(file);
		}
	}
	if (proj.detail.maskInfoSize > MASK_SECOND_MAX)
	{
		int count = proj.detail.maskInfoSize - MASK_SECOND_MAX;
		proj.maskInfoRead(file, count);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE])
	{
		proj.surfaceInfoRead(file, proj.detail.SurfaceCount);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_LEN])
	{
		int count = proj.detail.chunksubInfo[PC_ANNO_LEN];
		for (int n = 0; n < count; ++n)
		{
			proj.annoLengthRead(file);
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ANGLE])
	{
		int count = proj.detail.chunksubInfo[PC_ANNO_ANGLE];
		for (int n = 0; n < count; ++n)
		{
			proj.annoAngleRead(file);
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_OVAL])
	{
		int count = proj.detail.chunksubInfo[PC_ANNO_OVAL];
		for (int n = 0; n < count; ++n)
		{
			proj.annoOvalRead(file);
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ARROW])
	{
		int count = proj.detail.chunksubInfo[PC_ANNO_ARROW];
		for (int n = 0; n < count; ++n)
		{
			proj.annoArrowRead(file);
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_THUMBNAIL])
	{
		int _size = proj.detail.ThumbSize;

		proj.thumbnailRead(file, _size);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_CAPTURE_IMG])
	{
		int count = proj.detail.chunksubInfo[PROJ_CHUNK::PC_CAPTURE_IMG];

		for (int i = 0; i < count; i++)
			proj.captureImgRead(file);
	}

	setProgressValue(_progress);
	_progress += 10;//70
	bool rst = true;
	if (proj.detail.fGamma > 0.0f)
		rst = false;
	m_pWinManager->setGamma(proj.detail.fGamma);
	emit setGamma(rst ? rst : proj.detail.isGamma, rst);


	rst = false;
	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_STUDY_DESC])
	{
		rst = true;

		char* desc = new char[proj.detail.subStudySize + 1];

		memset(desc, 0, proj.detail.subStudySize);

		file.read(desc, proj.detail.subStudySize);
		desc[proj.detail.subStudySize] = '\0';

		info.studyDescription.append(desc);

		SAFE_DELETES(desc);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SERIES_DESC])
	{
		rst = true;

		char* desc = new char[proj.detail.subSeriesSize + 1];

		memset(desc, 0, proj.detail.subSeriesSize);

		file.read(desc, proj.detail.subSeriesSize);
		desc[proj.detail.subSeriesSize] = '\0';

		info.description_.append(desc);

		SAFE_DELETES(desc);
	}


	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_REPORT])
	{
		//Image Read
		if (!proj.detail.updateReport)
		{
			proj.detail.reportInfo.imageCount = proj.detail.reserve[46];
			proj.detail.reportInfo.htmlSize = proj.detail.reserve[47];
		}

		int imageCnt = proj.detail.reportInfo.imageCount;
		std::vector<std::pair<QImage, QString>> image_list;
		image_list.reserve(imageCnt);
		for (int i = 0; i < imageCnt; ++i)
		{
			proj.reportImageRead(file, image_list);
		}

		//HTML Read
		char* buf = new char[proj.detail.reportInfo.htmlSize];

		memset(buf, 0, proj.detail.reportInfo.htmlSize);

		file.read((char*)buf, proj.detail.reportInfo.htmlSize);

		QString html = QString::fromLocal8Bit(buf);
#if !defined(COVID19_VER)
		if ((m_pProduct->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH))
			m_pWinManager->setTempReportData(QFileInfo(m_filePath).baseName(), image_list, html);
#endif

		SAFE_DELETES(buf);

		setProgressValue(_progress);
		_progress += 10;//70
	}


	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE_SUB])
	{
		proj.surfaceSubInfoRead(file, proj.detail.chunksubInfo[PROJ_CHUNK::PC_SURFACE_SUB]);
	}

	//UpdateSummary
	file.close();


	for (int i = 0; i < 2; i++)
	{
		COLOR _col;
		_col.allcolor = proj.detail.seedColor[i];
		m_pWinManager->setSeedColor(_col, i);
	}

	if (rst)
	{
		m_pWinManager->GetDicomInfo(&info);

		emit UpdateSummary();
	}


	m_pVolumData->getAllMaskVoxel();
	//	m_pVolumData->getAllMaskBoundingBox();
	emit UpdateUI();	//	m_pWinManager->updateUI();

	m_pVolumData->setHuMin(proj.detail.volumeHUMin);
	m_pVolumData->setHuMax(proj.detail.volumeHUMax);
	m_pVolumData->setDownScaledCnt(proj.detail.downScaleCnt);
	m_pWinManager->setDefaultLevel(proj.detail.nDefaultLevel);
	m_pWinManager->setDefaultWidth(proj.detail.nDefaultWidth);
	setProgressValue(_progress);
	_progress += 10;//80
	if (!proj.detail.SelectedCustomPreset)
		proj.detail.CuspreVal = -1;
	else
	{
		if (m_pWinManager->getPresetCount() <= proj.detail.CuspreVal)
			proj.detail.CuspreVal = -1;
	}

	if (!proj.detail.SelectedCustomVolumePreset)
		proj.detail.CusVolumepreVal = -1;
	else
	{
		if (m_pWinManager->getPresetCount() <= proj.detail.CusVolumepreVal)
			proj.detail.CusVolumepreVal = -1;
	}

	emit setPreset(true, proj.detail.SelectedvolumePreSet, proj.detail.CusVolumepreVal, proj.detail.volumeWidth, proj.detail.volumeLevel);
	emit setPreset(false, proj.detail.SelectedPreSet, proj.detail.CuspreVal, proj.detail.windowWidth, proj.detail.windowLevel);
	VolumeView* volumeView = static_cast<VolumeView*>(m_pWinManager->mainSegmentWidget->getViewVolume());
	if (NULL != volumeView)
	{
		if (proj.detail.volumeRotation.x != 0 || proj.detail.volumeRotation.y != 0 ||
			proj.detail.volumeRotation.z != 0 || proj.detail.volumeRotation.w != 0)
		{
			volumeView->setVolRotation(proj.detail.volumeRotation);
		}

		volumeView->setVolTranslation(proj.detail.volumeTranslation);
		volumeView->setZoom(proj.detail.volumeZoom);
	}

	m_pVolumData->forceUpdateMaskVolume();
	setProgressValue(_progress);
	_progress += 10;//90
	ACTION_MANAGER->clear();

	emit resetRG();

	emit createHistogram();

	emit resetRG();

	emit resetWork();

	emit UpdateReport();

	if (m_pWinManager->mainTabType == MAINTAB_MEASUREMENT)
	{
		if (m_pWinManager->mainAnalWidget)
		{
			AnalVolumeView* volumeView = (m_pWinManager->mainAnalWidget->getViewVolume());
			if (NULL != volumeView)
			{
				if (proj.detail.volumeRotation.x != 0 || proj.detail.volumeRotation.y != 0 ||
					proj.detail.volumeRotation.z != 0 || proj.detail.volumeRotation.w != 0)
				{
					volumeView->setVolRotation(proj.detail.volumeRotation);
				}
				volumeView->setVolTranslation(proj.detail.volumeTranslation);
				volumeView->setZoom(proj.detail.volumeZoom);
			}
		}
		if (m_pWinManager->mainTAWidget)
		{
			AnalVolumeView* volumeView = (m_pWinManager->mainTAWidget->getViewVolume());
			if (NULL != volumeView)
			{
				if (proj.detail.volumeRotation.x != 0 || proj.detail.volumeRotation.y != 0 ||
					proj.detail.volumeRotation.z != 0 || proj.detail.volumeRotation.w != 0)
				{
					volumeView->setVolRotation(proj.detail.volumeRotation);
				}
				volumeView->setVolTranslation(proj.detail.volumeTranslation);
				volumeView->setZoom(proj.detail.volumeZoom);
			}
		}
	}

	setProgressValue(_progress);
	_progress += 10;//100
	emit updateAnno();

	m_pVolumData->threadResult = 1;
	setProgressValue(_progress);
	_progress += 10;

#endif
	emit finished();
}


void WorkMipOpen::initMeshData(MIP_ENCODER::ProjectDataInfo& proj)
{
	// PC_SURFACE, PC_SURFACE_SUB
	// Mesh 기능이 부여되지 않는 경우에 Mesh data 입력을 안하도록 설정
	m_pMeshData->ClearMeshInfo();

	if (m_pProduct->IsAvailableFunctionLevel(MFL_Common_Rendering_MeshTabList))
	{
		int surfaceInfoListCount = proj.vecSurfaceInfo.size();
		if (surfaceInfoListCount > 0)
		{
			for (int i = 0; i < surfaceInfoListCount; i++)
			{
				mip::MeshTopology* pMesh = new mip::MeshTopology(g_Renderer);

				if (pMesh == NULL)
				{
					break;
				}

				if (proj.vecSurfaceInfo[i].nPoint > 0)
				{
					for (int j = 0; j < proj.vecSurfaceInfo[i].nPoint * 3; j++)
					{
						pMesh->m_verts.push_back(proj.vecSurfaceInfo[i].verts[j]);
						pMesh->m_tris.push_back(proj.vecSurfaceInfo[i].tris[j]);
						pMesh->m_normals.push_back(proj.vecSurfaceInfo[i].normals[j]);
					}
				}
				else if (proj.vecSurfaceInfo[i].nPoint < 0)
				{
					int nPointCnt = (-proj.vecSurfaceInfo[i].nPoint);
					pMesh->m_verts.reserve(nPointCnt);
					pMesh->m_verts.assign(nPointCnt, mip::VECTOR3());
					std::memcpy((char*)pMesh->m_verts.data(), &proj.vecSurfaceInfo[i].verts[0], sizeof(mip::VECTOR3) * nPointCnt);
				}

				MeshInfo* pMeshInfo = nullptr;
				//if (proj.vecSurfaceInfo[i].subInfo)
				{
					pMeshInfo = new MeshInfo;
					std::memset(pMeshInfo, 0, sizeof(MeshInfo));
					pMeshInfo->uid = proj.vecSurfaceInfo[i].infoIndex;
					pMeshInfo->color = proj.vecSurfaceInfo[i].color;
					pMeshInfo->selected = proj.vecSurfaceInfo[i].selected;
					pMeshInfo->show = !(proj.vecSurfaceInfo[i].isHidden);
					pMeshInfo->NameLength = proj.vecSurfaceInfo[i].NameLength;
					pMeshInfo->upScale = proj.vecSurfaceInfo[i].upScale;
					pMeshInfo->TrisCount = proj.vecSurfaceInfo[i].TrisCount;
					pMeshInfo->NorCount = proj.vecSurfaceInfo[i].NorCount;

					std::wstring wide_string = std::wstring(proj.vecSurfaceInfo[i].wstrName.begin(), proj.vecSurfaceInfo[i].wstrName.end());
					const wchar_t* result = wide_string.c_str();

					memcpy_s(pMeshInfo->MeshName, sizeof(pMeshInfo->MeshName), &wide_string[0], sizeof(WCHAR) * wide_string.size());

					m_pMeshData->AddMeshInfo(i, (*pMeshInfo));
					m_pMeshData->SetConnectMesh(i, proj.vecSurfaceInfo[i].infoIndex);
				}

				//muint32 shaderType = proj.vecSurfaceInfo[i].nShaderType; //20210615_byPHS_add_ShaderType

				if (proj.vecSurfaceInfo[i].textureInfoCount)
				{
					int _cnt = proj.vecSurfaceInfo[i].uv.size();
					pMesh->m_uv.reserve(_cnt);
					pMesh->m_uv.assign(proj.vecSurfaceInfo[i].uv.begin(), proj.vecSurfaceInfo[i].uv.end());

					_cnt = proj.vecSurfaceInfo[i].textureID.size();
					pMesh->m_textureID.reserve(_cnt);
					pMesh->m_textureID.assign(proj.vecSurfaceInfo[i].textureID.begin(), proj.vecSurfaceInfo[i].textureID.end());

					for (int ii = 0; ii < proj.vecSurfaceInfo[i].textureInfo.size(); ii++)
					{
						int width = proj.vecSurfaceInfo[i].textureInfo[ii].width;
						int height = proj.vecSurfaceInfo[i].textureInfo[ii].height;
						int Channel = proj.vecSurfaceInfo[i].textureInfo[ii].nrChannels;
						int tileStyle_1 = proj.vecSurfaceInfo[i].textureInfo[ii].tileStyle_1;
						//int tileStyle_2		= proj.vecSurfaceInfo[i].textureInfo[ii].tileStyle_2;
						muint32 shaderType = proj.vecSurfaceInfo[i].textureInfo[ii].tileStyle_2; //20210615_byPHS_ShaderType으로 사용
						int dataSize = proj.vecSurfaceInfo[i].textureInfo[ii].dataSize;

						std::wstring wsName(pMeshInfo->MeshName);
						std::string strName(wsName.begin(), wsName.end());

						mip::TEXTURE texture(strName + "_" + std::to_string(ii), width, height, 0, Channel, tileStyle_1);

						unsigned char* pTxtData = new unsigned char[dataSize];
						std::copy(proj.vecSurfaceInfo[i].textureInfo[ii].data.begin(), proj.vecSurfaceInfo[i].textureInfo[ii].data.end(), pTxtData);
						texture.setTextureData(pTxtData);

						m_pWinManager->makeCurrent();
						int textId = pMesh->createTexture(pTxtData, width, height, Channel, tileStyle_1);
						m_pWinManager->doneCurrent();

						std::string key = "Texture_" + std::to_string(ii + 1);
						pMesh->addTextureId(textId, key);
						pMesh->m_TextureDataList.push_back(texture);
						if (shaderType >= 0 && shaderType < mip::SHADERTYPE::SHADER_MAX)
						{
							pMesh->initShader(shaderType);
						}
						else
						{
							pMesh->initShader(mip::SHADERTYPE::SHADER_3MF);
						}
					}
				}

				int matsize = 16;
				bool loadMat = false;
				for (int ii = 0; ii < matsize; ii++)
				{
					if (proj.vecSurfaceInfo[i].worldMat[ii] != 0)
					{
						loadMat = true;
					}
				}

				if (loadMat)
				{
					mip::MATRIX44 mat = proj.vecSurfaceInfo[i].worldMat;

					pMesh->setScale(mat.extractScale());
					pMesh->setRotate(mat.getQuaternion());
					pMesh->setTranslate(mat.getOrigin());
				};

				m_pMeshData->InsertMesh(i, pMesh);
				m_pMeshData->MeshRenderUpdate(proj.vecSurfaceInfo[i].UID);
			}
		}
	}

	int n_mesh = m_pMeshData->GetMeshCount();

	m_pMeshData->ClearMeshInfoSelectMode();
	m_pMeshData->SetMeshInfoModeSelectMode(n_mesh - 1, true);

	m_pMeshData->SetCurrentMeshIndex(m_pMeshData->GetMeshCount() - 1);
}
