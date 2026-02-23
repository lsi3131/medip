#include "stdafx.h"
#include "WorkSaveMIP.h"

#include "defineMipEncoder.h"
#include "medipmipencoder.h"
#include "DicomReader.h"

#include "Windows/WindowBase.h"
#include "windowManager.h"

#include "Windows/VolumeView.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "ActionManager.h"

#include "graphics/Annotation/AnnoLength.h"
#include "graphics/Annotation/AnnoArrow.h"
#include "graphics/Annotation/AnnoString.h"
#include "graphics/Annotation/AnnoAngle.h"
#include "graphics/Annotation/AnnoRectangle.h"

#include "CustomHistogram.h"
#include "Renderer/MeshTopology.h"

#define MIP_ENCODER_TEST

WorkSaveMIP::WorkSaveMIP(const QString& mipFilePath, WindowManager* pWinManager, VOLUME_DATA* pVolumeData, MeshData* pMeshData) :
	m_mipFilePath(mipFilePath),
	m_pWinManager(pWinManager),
	m_pVolumeData(pVolumeData),
	m_pMeshData(pMeshData),
	m_addValue(0)
{
}

void WorkSaveMIP::setProgressValue(int value, bool init /*= false*/)
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

void WorkSaveMIP::threadRun()
{
	setProgressValue(0, true);

#ifdef MIP_ENCODER_TEST	
	if (m_pVolumeData->isValidate() == false)
	{
		emit finished();
		return;
	}

	MIP_ENCODER::ProjectDataInfo proj = {};

	/////////////////////////////////////// detail ///////////////////////////////////////
	proj.dataLenth = m_pVolumeData->getVolumeDataLength();
	proj.dataCX = m_pVolumeData->getCX();
	proj.dataCY = m_pVolumeData->getCY();
	proj.dataCZ = m_pVolumeData->getCZ();
	proj.spaceX = m_pVolumeData->getSpaceX();
	proj.spaceY = m_pVolumeData->getSpaceY();
	proj.spaceZ = m_pVolumeData->getSpaceZ();

	proj.maskSize = sizeof(mask) * proj.dataLenth;
	proj.volumeSize = sizeof(short) * proj.dataLenth;

	proj.right_hand_coord = 1;

	proj.maskInfoSize = m_pVolumeData->getMaskInfoListCnt();

	DcmtkSeriesInfo* pDcmInfo = m_pWinManager->GetDicomInfo();
	proj.dicomElementInfo.StudyInstanceUID = pDcmInfo->strStudyUID;
	proj.dicomElementInfo.SeriesInstanceUID = pDcmInfo->strSeriesUID;
	proj.dicomElementInfo.ExamID = pDcmInfo->examID;
	proj.dicomElementInfo.AccessionNumber = pDcmInfo->accessionNumber;
	proj.dicomElementInfo.StudyDate = pDcmInfo->studyDate_;
	proj.dicomElementInfo.StudyTime = pDcmInfo->studyTime_;
	proj.dicomElementInfo.PatientID = pDcmInfo->patientId_;
	/* 저장할때만 반영되지 않도록 수정 */
	//proj.dicomElementInfo.PatientName = pDcmInfo->patientsName_;
	proj.dicomElementInfo.PatientSex = pDcmInfo->sex_;
	proj.dicomElementInfo.PatientAge = pDcmInfo->age_;
	proj.dicomElementInfo.PatientWeight = pDcmInfo->patientWeight_;
	proj.dicomElementInfo.Units = pDcmInfo->units_;
	proj.dicomElementInfo.RadionuclideTotalDose = pDcmInfo->radionuclideTotalDose_;
	proj.dicomElementInfo.RadionuclideHalfLife = pDcmInfo->radionuclideHalfLife;
	proj.dicomElementInfo.RadiopharmaceuticalStartTime = pDcmInfo->radiopharmaceuticalStartTime_;
	proj.dicomElementInfo.FrameOfReferenceUID = pDcmInfo->frameOfReferenceUID_;
	proj.dicomElementInfo.SeriesDate = pDcmInfo->seriesDate;

	proj.vecMaskInfo.assign(proj.maskInfoSize, MIP_ENCODER::ProjectMaskInfo());
	for (int n = 0; n < proj.maskInfoSize; n++)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(n);
		if (nullptr != info)
		{
			proj.vecMaskInfo[n].color.a = 255;
			proj.vecMaskInfo[n].color = info->color;
			proj.vecMaskInfo[n].uid = info->uid;
			proj.vecMaskInfo[n].layerCustom = true;
			proj.vecMaskInfo[n].layerAlpha = info->layerAlpha;
			memcpy_s(proj.vecMaskInfo[n].maskName, sizeof(proj.vecMaskInfo[n].maskName), info->maskName, sizeof(info->maskName));
			proj.vecMaskInfo[n].mask_id = info->mask_id;
			if (info->uid >= MASK_SECOND_MAX && info->mask_id2 == 0)
			{
				proj.vecMaskInfo[n].mask_id2 = VM_MASK0 << ((info->uid - (MASK_SECOND_MAX)) % 8);
			}
			else
			{
				proj.vecMaskInfo[n].mask_id2 = info->mask_id2;
			}
			proj.vecMaskInfo[n].show = info->show;
			proj.vecMaskInfo[n].meshConnected = info->meshConnected;
			proj.vecMaskInfo[n].reserve_bool[0] = info->bIsUseCredit;

		}
	}

	proj.windowLevel = m_pWinManager->getWindowLevel();
	proj.windowWidth = m_pWinManager->getWindowWidth();
	proj.SelectedPreSet = m_pWinManager->getSelectedPreset();
	proj.nDefaultLevel = m_pWinManager->getDefaultLevel();
	proj.nDefaultWidth = m_pWinManager->getDefaultWidth();
	proj.volumeLevel = m_pWinManager->getVolumeLevel();
	proj.volumeWidth = m_pWinManager->getVolumeWidth();
	proj.SelectedvolumePreSet = m_pWinManager->getSelectedVolumePreset();
	proj.box = m_pVolumeData->getBoundingBox();
	proj.volumeHUMin = m_pVolumeData->getHuMin();
	proj.volumeHUMax = m_pVolumeData->getHuMax();
	proj.CuspreVal = m_pWinManager->getSelectedCustomPreset(CL_2D);
	proj.CusVolumepreVal = m_pWinManager->getSelectedCustomPreset(CL_3D);
	proj.SelectedCustomPreset = -1 < proj.CuspreVal;
	proj.SelectedCustomVolumePreset = -1 < proj.CusVolumepreVal;
	// DeepCatch 전용
	proj.stPredictOptInfo.bIsPredictComplete = m_pWinManager->IsPredictComplete();

	if (m_pWinManager->aniLine.count() != 0)
	{
		int val = 0;
		m_pWinManager->aniCount.removeAll(val);
		//proj.vecAniLine.reserve(m_pWinManager->aniCount.count() - 1);
		//proj.aniSize = m_pWinManager->aniCount.count()*(sizeof(mint32) * 11)	+ (sizeof(float) * (m_pWinManager->aniLine.count() * 3)); // 이 다음 windows info
	}

	// window Info
	//proj.detail.chunkInfo[PROJ_CHUNK::PC_WINDOWINFO] = 1;
	//proj.detail.chunksubInfo[PROJ_CHUNK::PC_WINDOWINFO] = 3;

	//proj.detail.WindowAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
	//proj.head.fileSize += sizeof(WindowInfo) * 3;

	if (m_pWinManager->anotationList.size() > 0)
	{
		int annoTextCount = 0;
		int annoLengthCount = 0;
		int annoAngleCount = 0;
		int annoOvalCount = 0;
		int annoArrowCount = 0;

		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];
			if (a->getType() == AT_TEXT) annoTextCount++;
			else if (a->getType() == AT_LEN) annoLengthCount++;
			else if (a->getType() == AT_ANGLE) annoAngleCount++;
			else if (a->getType() == AT_OVAL) annoOvalCount++;
			else if (a->getType() == AT_ARROW) annoArrowCount++;
		}
	}

	int len = m_pWinManager->GetDicomInfo()->studyDescription.length();
	if (len > 1)
	{
		proj.strStudy_desc = m_pWinManager->GetDicomInfo()->studyDescription;
		if (proj.strStudy_desc.size() > 20)
		{
			proj.subStudySize = proj.strStudy_desc.size() - 20; // 20 = detail 기본 저장 길이
		}
		else
		{
			proj.subStudySize = 0;
		}
	}

	setProgressValue(20);
	len = m_pWinManager->GetDicomInfo()->description_.length();
	if (len > 1)
	{
		proj.strSeries_desc = m_pWinManager->GetDicomInfo()->description_;
		if (proj.strSeries_desc.size() > 20)
		{
			proj.subSeriesSize = proj.strSeries_desc.size() - 20;
		}
		else
		{
			proj.subSeriesSize = 0;
		}
	}

	if (m_pWinManager->GetDicomInfo()->kvp_.size() > 0)
	{
		proj.nKVP = std::stoi(m_pWinManager->GetDicomInfo()->kvp_);
	}
	else
	{
		proj.nKVP = 0;
	}

	if (m_pWinManager->GetDicomInfo()->ma_.size() > 0)
	{
		proj.nmA = std::stoi(m_pWinManager->GetDicomInfo()->ma_);
	}
	else
	{
		proj.nmA = 0;
	}

	proj.strManufacturer = m_pWinManager->GetDicomInfo()->Manufacturer;;
	proj.strManufacturerModel = m_pWinManager->GetDicomInfo()->manufacturerModel;
	proj.strKernel = m_pWinManager->GetDicomInfo()->convolutionkernel;

	// image position
	//mip::VECTOR3 vImgPos(m_pVolumeData->getImgPosX(), m_pVolumeData->getImgPosY(), m_pVolumeData->getImgPosZ());
	//if (vImgPos.x != 0 && vImgPos.y != 0 && vImgPos.z != 0)
	//{
	float test = m_pVolumeData->getImgPosX();;
	proj.imgPos[0] = m_pVolumeData->getImgPosX();
	proj.imgPos[1] = m_pVolumeData->getImgPosY();
	proj.imgPos[2] = m_pVolumeData->getImgPosZ();
	//}
	// image orientation
	float ImgOrientation[3];

	// xAxis
	std::memset(&ImgOrientation, 0.f, 3);
	m_pVolumeData->getImgOrientation(true, ImgOrientation);
	proj.imgOrientation[0].x = ImgOrientation[0];
	proj.imgOrientation[0].y = ImgOrientation[1];
	proj.imgOrientation[0].z = ImgOrientation[2];

	// yAxis
	std::memset(&ImgOrientation, 0.f, 3);
	m_pVolumeData->getImgOrientation(false, ImgOrientation);
	proj.imgOrientation[1].x = ImgOrientation[0];
	proj.imgOrientation[1].y = ImgOrientation[1];
	proj.imgOrientation[1].z = ImgOrientation[2];

	QImage img = m_pWinManager->getThumbnail();

	if (img != QImage())
	{
		proj.ThumbSize = img.byteCount();
	}

	if (m_pWinManager->captureList.count() != 0)
	{
		for (int i = 0; i < m_pWinManager->captureList.count(); i++)
		{
			QImage* Capimg = m_pWinManager->captureList.at(i);
			proj.CaptureSize += Capimg->byteCount();
			proj.CaptureSize += sizeof(int) * 3;
		}
	}

	proj.updateReport = true;
	std::vector<std::pair<QImage, QString>> image_list = m_pWinManager->tempReportData.getImageList();
	for (int i = 0; i < image_list.size(); )
	{
		if (image_list[i].first.isNull())
		{
			image_list.erase(image_list.begin() + i);
		}
		else
		{
			++i;
		}
	}

	if (!m_pWinManager->tempReportData.isEmpty())
	{
		proj.reportInfo.imageCount = image_list.size();
		if (proj.reportInfo.imageCount > 0)
		{
			proj.vecImageInfo.assign(proj.reportInfo.imageCount, MIP_ENCODER::ImageInfo());

			for (int i = 0; i < proj.reportInfo.imageCount; ++i)
			{
				proj.vecImageInfo[i].width = image_list[i].first.width();
				proj.vecImageInfo[i].height = image_list[i].first.height();
				proj.vecImageInfo[i].size = image_list[i].first.byteCount();
				proj.vecImageInfo[i].nameLength = image_list[i].second.toLocal8Bit().size() + 1;	// +1 : ??????
				proj.vecImageInfo[i].name = image_list[i].second.toLocal8Bit().toStdString();

				//이미지 저장
				//uchar *dt = image_list[i].first.bits();
				//file.write((char*)dt, repImgInfo[i].size);
				proj.vecImageInfo[i].data.assign(proj.vecImageInfo[i].size, 0);
				std::memcpy(&proj.vecImageInfo[i].data[0], image_list[i].first.bits(), proj.vecImageInfo[i].size);
			}
		}
		proj.reportInfo.htmlSize = m_pWinManager->tempReportData.getHTMLSize();
	}

	proj.downScaleCnt = m_pVolumeData->getDownScaledCnt();

	if (m_pWinManager->mainSegmentWidget)
	{
		VolumeView* volumeView = static_cast<VolumeView*>(m_pWinManager->mainSegmentWidget->getViewVolume());
		if (nullptr != volumeView)
		{
			proj.volumeRotation = volumeView->getVolRotation();
			proj.volumeTranslation = volumeView->getVolTranslation();
			proj.volumeZoom = volumeView->getZoom();
		}
	}

	proj.isGamma = m_pWinManager->isGammaMode();
	proj.fGamma = m_pWinManager->getGamma();
	proj.modality_type = (MIP_ENCODER_SINGLTON->GetModalityTypeFromString(QString::fromLocal8Bit(m_pWinManager->GetDicomInfo()->modality_.c_str())));

	for (int i = 0; i < 2; i++)
	{
		COLOR _col = m_pWinManager->getSeedColor(i);
		proj.seedColor[i] = _col.allcolor;
	}

	/////////////////////////////////////// chunk ///////////////////////////////////////

	//file.write((char*)m_pVolumeData->getMaskDataPoint(), proj.maskSize);

	//proj.vecMaskData

	// mask data, mask info
	proj.vecMaskData.emplace_back(std::vector<mask>(proj.maskSize, 0));
	mask* pMaskPoint = m_pVolumeData->getMaskDataPoint(0);
	int maskDataIndex = proj.vecMaskData.size() - 1;
	if (pMaskPoint)
	{
		std::memcpy(&proj.vecMaskData[maskDataIndex][0], pMaskPoint, proj.maskSize);
	}

	for (int n = MASK_SECOND_MAX; n < proj.maskInfoSize; n++)
	{
		if ((n - MASK_SECOND_MAX) % 8 == 0)
		{
			proj.vecMaskData.emplace_back(std::vector<mask>(proj.maskSize, 0));
			mask* pMaskPoint = m_pVolumeData->getMaskDataPoint(1 + (n - MASK_SECOND_MAX) / 8);
			int maskDataIndex = proj.vecMaskData.size() - 1;
			if (pMaskPoint)
			{
				std::memcpy(&proj.vecMaskData[maskDataIndex][0], pMaskPoint, proj.maskSize);
			}
		}
		//	
		//MIP_ENCODER::ProjectMaskInfo fileInfo;

		//MaskInfo *info = m_pVolumeData->getMaskInfo(n);
		//if (nullptr != info)
		//{
		//	fileInfo.color = info->color;
		//	fileInfo.uid = info->uid;
		//	memcpy_s(fileInfo.maskName, sizeof(fileInfo.maskName),
		//		info->maskName, sizeof(info->maskName));
		//	fileInfo.mask_id = info->mask_id;
		//	fileInfo.show = info->show;
		//	fileInfo.meshConnected = info->meshConnected;
		//	fileInfo.mask_id2 = info->mask_id2;
		//	fileInfo.layerCustom = true;
		//	fileInfo.layerAlpha = info->layerAlpha;
		//}
		//proj.vecMaskInfo.push_back(fileInfo);
	}

	proj.vecHUData.assign(proj.volumeSize, 0);
	std::memcpy(&proj.vecHUData[0], m_pVolumeData->getHUDataPoint(), proj.volumeSize);

	setProgressValue(60);

	// Animation info	
	int nAniTotalCnt = 0;
	for (int i = 0; i < m_pWinManager->aniCount.count(); i++)
	{
		MIP_ENCODER::AnimationInfo aniInfo;
		aniInfo.nPoint = m_pWinManager->aniCount.at(i);
		if (aniInfo.nPoint == 0)
			continue;

		for (int j = 0; j < aniInfo.nPoint; ++j)
		{
			aniInfo.vecPoints.push_back(m_pWinManager->aniLine.at(nAniTotalCnt + j).x());
			aniInfo.vecPoints.push_back(m_pWinManager->aniLine.at(nAniTotalCnt + j).y());
			aniInfo.vecPoints.push_back(m_pWinManager->aniLine.at(nAniTotalCnt + j).z());
		}
		nAniTotalCnt += aniInfo.nPoint;
		proj.vecAniLine.push_back(aniInfo);
	}

	// window info
	MIP_ENCODER::WindowInfo info;
	std::memset(&info, 0, sizeof(MIP_ENCODER::WindowInfo));

	proj.vecWindowInfo.resize(3);

	WindowBase* win = nullptr;
	if (m_pWinManager->mainTabType == MAINTAB_TA)
	{
		if (m_pWinManager->mainTAWidget)
		{
			win = m_pWinManager->mainTAWidget->getWindow(WT_AXIAL);
		}
	}
	else
	{
		if (m_pWinManager->mainSegmentWidget)
		{
			win = m_pWinManager->mainSegmentWidget->getWindow(WT_AXIAL);
		}
	}

	if (win)
	{
		info.windowType = WT_AXIAL;
		info.posX = win->getSlicePositionX();
		info.posY = win->getSlicePositionY();
		info.zoomFactorX = win->getZoomFactorX();
		info.zoomFactorY = win->getZoomFactorY();
		info.factorX = win->getFactorX();
		info.factorY = win->getFactorY();
		info.depth = win->getDepth();
		//file.write((char*)&info, sizeof(WindowInfo));
		proj.vecWindowInfo[0] = info;
	}

	std::memset(&info, 0, sizeof(MIP_ENCODER::WindowInfo));

	if (m_pWinManager->mainTabType == MAINTAB_TA)
	{
		if (m_pWinManager->mainTAWidget)
		{
			win = m_pWinManager->mainTAWidget->getWindow(WT_CORONAL);
		}
	}
	else
	{
		if (m_pWinManager->mainSegmentWidget)
		{
			win = m_pWinManager->mainSegmentWidget->getWindow(WT_CORONAL);
		}
	}

	if (win)
	{
		info.windowType = WT_CORONAL;
		info.posX = win->getSlicePositionX();
		info.posY = win->getSlicePositionY();
		info.zoomFactorX = win->getZoomFactorX();
		info.zoomFactorY = win->getZoomFactorY();
		info.factorX = win->getFactorX();
		info.factorY = win->getFactorY();
		info.depth = win->getDepth();
		//file.write((char*)&info, sizeof(WindowInfo));
		proj.vecWindowInfo[1] = info;
	}

	std::memset(&info, 0, sizeof(MIP_ENCODER::WindowInfo));
	if (m_pWinManager->mainTabType == MAINTAB_TA)
	{
		if (m_pWinManager->mainTAWidget)
		{
			win = m_pWinManager->mainTAWidget->getWindow(WT_SAGITTAL);
		}
	}
	else
	{
		if (m_pWinManager->mainSegmentWidget)
		{
			win = m_pWinManager->mainSegmentWidget->getWindow(WT_SAGITTAL);
		}
	}

	if (win)
	{
		info.windowType = WT_SAGITTAL;
		info.posX = win->getSlicePositionX();
		info.posY = win->getSlicePositionY();
		info.zoomFactorX = win->getZoomFactorX();
		info.zoomFactorY = win->getZoomFactorY();
		info.factorX = win->getFactorX();
		info.factorY = win->getFactorY();
		info.depth = win->getDepth();
		proj.vecWindowInfo[2] = info;
	}


	if (m_pWinManager->anotationList.size() > 0)
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* anno = m_pWinManager->anotationList[n];

			switch (anno->getType())
			{
			case AT_TEXT:
			{
				AnnoString* annoString = static_cast<AnnoString*>(anno);
				MIP_ENCODER::AnnoTextInfo info;
				std::memset(&info, 0, sizeof(info));
				mip::VECTOR3 v_volume = annoString->getVolumePos();
				info.posX = v_volume.x;
				info.posY = v_volume.y;
				info.posZ = v_volume.z;
				::StringCbPrintf(info.text, TEXT_LENGTH_MAX * sizeof(WCHAR), annoString->getText().toStdWString().c_str());
				info.size = annoString->getFontSize();
				info.color = annoString->getColor().allcolor;
				info.type = annoString->getTextType();
				info.isHidden = annoString->isAnnoHidden();

				proj.vecAnnoTextInfo.push_back(info);
			}
			break;
			case AT_LEN:
			{
				AnnoLength* annoLen = static_cast<AnnoLength*>(anno);
				MIP_ENCODER::AnnoLengthInfo info;
				std::memset(&info, 0, sizeof(MIP_ENCODER::AnnoLengthInfo));
				info.posX1 = annoLen->getV1().x;
				info.posY1 = annoLen->getV1().y;
				info.posZ1 = annoLen->getV1().z;

				info.posX2 = annoLen->getV2().x;
				info.posY2 = annoLen->getV2().y;
				info.posZ2 = annoLen->getV2().z;

				info.color = annoLen->getColor().allcolor;
				info.isHidden = annoLen->isAnnoHidden();

				proj.vecAnnoLengthInfo.push_back(info);
			}
			break;
			case AT_ANGLE:
			{
				AnnoAngle* annoAngle = static_cast<AnnoAngle*>(anno);
				MIP_ENCODER::AnnoAngleInfo info;
				std::memset(&info, 0, sizeof(MIP_ENCODER::AnnoAngleInfo));
				info.posX1 = annoAngle->getV1().x;
				info.posY1 = annoAngle->getV1().y;
				info.posZ1 = annoAngle->getV1().z;

				info.posX2 = annoAngle->getV2().x;
				info.posY2 = annoAngle->getV2().y;
				info.posZ2 = annoAngle->getV2().z;

				info.posX3 = annoAngle->getV3().x;
				info.posY3 = annoAngle->getV3().y;
				info.posZ3 = annoAngle->getV3().z;

				info.color = annoAngle->getColor().allcolor;
				info.isHidden = annoAngle->isAnnoHidden();

				proj.vecAnnoAngleInfo.push_back(info);
				break;
			}
			case AT_OVAL:
			{
				/*
					이상일 대리
					- 현재 사용되지 않아 주석 처리
				*/
				//AnnoOval * annoOval = static_cast<AnnoOval *>(anno);
				//MIP_ENCODER::AnnoOvalInfo info;
				//std::memset(&info, 0, sizeof(MIP_ENCODER::AnnoOvalInfo));
				//info.posX1 = annoOval->_centerP.x;
				//info.posY1 = annoOval->_centerP.y;
				//info.posZ1 = annoOval->_centerP.z;

				//info.posX2 = annoOval->_radius.x;
				//info.posY2 = annoOval->_radius.y;
				//info.posZ2 = annoOval->_radius.z;

				//info.color = annoOval->getColor().allcolor;
				//info.isHidden = annoOval->isAnnoHidden();
				//
				//proj.vecAnnoOvalInfo.push_back(info);
				break;
			}
			case AT_ARROW:
			{
				AnnoArrow* annoArrow = static_cast<AnnoArrow*>(anno);
				MIP_ENCODER::AnnoArrowInfo info;
				std::memset(&info, 0, sizeof(MIP_ENCODER::AnnoArrowInfo));
				info.posX1 = annoArrow->getV1().x;
				info.posY1 = annoArrow->getV1().y;
				info.posZ1 = annoArrow->getV1().z;

				info.posX2 = annoArrow->getV2().x;
				info.posY2 = annoArrow->getV2().y;
				info.posZ2 = annoArrow->getV2().z;

				info.color = annoArrow->getColor().allcolor;
				info.isHidden = annoArrow->isAnnoHidden();

				proj.vecAnnoArrowInfo.push_back(info);
				break;
			}
			case AT_RECTANGLE:
			{
				/* TODO : Rectangle 관련 파일 Load 기능 추가할 것 */
				AnnoRectangle* annoRect = static_cast<AnnoRectangle*>(anno);
				break;
			}
			}
		}
	}

	setProgressValue(80);

	//	if (proj.chunkInfo[PROJ_CHUNK::PC_SURFACE])
	//		proj.surfaceInfoWrite(file);

		//SurfaceInfo *info = new SurfaceInfo;
	MeshInfo* mInfo = nullptr;

	int count = m_pMeshData->GetMeshCount();
	if (count > 0)
	{
		proj.vecSurfaceInfo.assign(count, MIP_ENCODER::SurfaceInfo());

		int UID = 0;
		for (int i = 0; i < count; i++)
		{
			mInfo = m_pMeshData->GetMeshInfo(i);

			if (mInfo)
			{
				proj.vecSurfaceInfo[i].InfoType = mInfo->uid < 0;

				/*LAYER UID*/
				proj.vecSurfaceInfo[i].infoIndex = mInfo->uid;

				proj.vecSurfaceInfo[i].subInfo = true;
				proj.vecSurfaceInfo[i].UID = i;
				proj.vecSurfaceInfo[i].selected = mInfo->selected;
				proj.vecSurfaceInfo[i].isHidden = !(mInfo->show);
				proj.vecSurfaceInfo[i].color = mInfo->color;

				QString str = m_pMeshData->GetMeshName(i);
				proj.vecSurfaceInfo[i].wstrName = str.toStdWString();
				proj.vecSurfaceInfo[i].NameLength = str.length();

				/*MESH MAP*/

				mip::MeshTopology* pMesh = m_pMeshData->GetMesh(i);
				if (pMesh)
				{
					proj.vecSurfaceInfo[i].TrisCount = pMesh->m_tris.size();
					proj.vecSurfaceInfo[i].NorCount = pMesh->m_normals.size();
					//proj.vecSurfaceInfo[i].nShaderType = k->getShaderType();
					proj.vecSurfaceInfo[i].nUVCount = pMesh->m_uv.size();
					proj.vecSurfaceInfo[i].textureIDCount = pMesh->m_textureID.size();
					proj.vecSurfaceInfo[i].textureInfoCount = pMesh->m_TextureDataList.size();

					int nVertSize = pMesh->m_verts.size();
					int nTrisSize = pMesh->m_tris.size();
					int nNormalsSize = pMesh->m_normals.size();
					int nUVCoordSize = 0; int nTextureIdSzie = 0; int nTextureInfoSize = 0;

					// 220802 허건 과장 주석처리
					// mip save 수행시, 사이즈가 0으로 인하여 프로그램 강제종료 됨
					//if (k->getShaderType() == mip::SHADERTYPE::SHADER_3MF)
					{
						nUVCoordSize = pMesh->m_uv.size();
						nTextureIdSzie = pMesh->m_textureID.size();
						nTextureInfoSize = pMesh->m_TextureDataList.size();
					}

					if (nVertSize % 3 == 0)
					{
						proj.vecSurfaceInfo[i].nPoint = nVertSize / 3;

						proj.vecSurfaceInfo[i].verts.assign(nVertSize, mip::VECTOR3());
						//memcpy(&proj.vecSurfaceInfo[i].verts[0], &k->m_verts[0], nVertSize);
						std::copy(pMesh->m_verts.begin(), pMesh->m_verts.end(), proj.vecSurfaceInfo[i].verts.begin());

						proj.vecSurfaceInfo[i].tris.assign(nTrisSize, 0);
						//memcpy(&proj.vecSurfaceInfo[i].tris[0], &k->m_tris[0], nTrisSize);
						std::copy(pMesh->m_tris.begin(), pMesh->m_tris.end(), proj.vecSurfaceInfo[i].tris.begin());

						proj.vecSurfaceInfo[i].normals.assign(nNormalsSize, mip::VECTOR3());
						//memcpy(&proj.vecSurfaceInfo[i].normals[0], &k->m_normals[0], nNormalsSize);
						std::copy(pMesh->m_normals.begin(), pMesh->m_normals.end(), proj.vecSurfaceInfo[i].normals.begin());

						//texture Data
						proj.vecSurfaceInfo[i].uv.assign(nUVCoordSize, mip::VECTOR2());
						std::copy(pMesh->m_uv.begin(), pMesh->m_uv.end(), proj.vecSurfaceInfo[i].uv.begin());

						proj.vecSurfaceInfo[i].textureID.assign(nTextureIdSzie, 0.0f);
						std::copy(pMesh->m_textureID.begin(), pMesh->m_textureID.end(), proj.vecSurfaceInfo[i].textureID.begin());

						for (int ii = 0; ii < nTextureInfoSize; ii++)
						{
							MIP_ENCODER::MeshTextureInfo info;

							info.width = pMesh->m_TextureDataList[ii].getWidth();
							info.height = pMesh->m_TextureDataList[ii].getHeight();
							info.nrChannels = pMesh->m_TextureDataList[ii].getChannels();
							info.tileStyle_1 = pMesh->m_TextureDataList[ii].getTileStyle();
							//info.tileStyle_2 = 0;
							info.tileStyle_2 = pMesh->getShaderType(); //20210615_byPHS_ShaderType으로 사용

							unsigned char* pTxtData = pMesh->m_TextureDataList[ii].getTextureData();
							info.dataSize = info.width * info.height * info.nrChannels;
							std::vector<unsigned char> vector(pTxtData, pTxtData + info.dataSize);
							info.data.swap(vector);

							proj.vecSurfaceInfo[i].textureInfo.push_back(info);
						}

						mip::MATRIX44 mat = pMesh->getMatrix();
						proj.vecSurfaceInfo[i].worldMat = mat;
					}
					else
					{
						proj.vecSurfaceInfo[i].nPoint = -nVertSize;
					}
				}
				else
				{
					continue;
				}
			}
		}
	}

	// PC_THUMBNAIL
	//if (proj.detail.chunkInfo[PROJ_CHUNK::PC_THUMBNAIL])
	//{
	//	uchar * dt = img.bits();
	//	file.write((char*)dt, img.byteCount());
	//}
	if (img.byteCount() > 0)
	{
		proj.vecThumbnail.assign(img.byteCount(), 0);
		std::memcpy(&proj.vecThumbnail[0], img.bits(), img.byteCount());
	}

	if (m_pWinManager->captureList.count() > 0)
	{
		proj.vecCaptureImgInfo.assign(m_pWinManager->captureList.count(), MIP_ENCODER::CaptureImgInfo());
	}

	for (int i = 0; i < m_pWinManager->captureList.count(); i++)
	{
		QImage* Capimg = m_pWinManager->captureList.at(i);
		uchar* dt = Capimg->bits();
		proj.vecCaptureImgInfo[i].width = Capimg->width();
		proj.vecCaptureImgInfo[i].height = Capimg->height();
		proj.vecCaptureImgInfo[i].size = Capimg->byteCount();

		proj.vecCaptureImgInfo[i].data.assign(proj.vecCaptureImgInfo[i].size, 0);
		std::memcpy(&proj.vecCaptureImgInfo[i].data[0], dt, proj.vecCaptureImgInfo[i].size);
	}

	setProgressValue(90);

	//QString strHtml = m_pWinManager->tempReportData.getHTML();
	std::string strHtml = (m_pWinManager->tempReportData.getHTML()).toLocal8Bit().toStdString();

	if (strHtml.size() > 0)
	{
		//QByteArray& text = strHtml.toLocal8Bit();
		int texSize = strHtml.size();
		proj.vecReportHtml.assign(texSize, 0);
		std::memcpy((char*)&proj.vecReportHtml[0], (char*)&strHtml[0], texSize);
	}


	m_pWinManager->tempReportData.clear();

	//if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE_SUB])
	//	proj.surfaceSubInfoWrite(file);


	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedData = ACTION_MANAGER->getDeepCatch_PredictedInfo();
	if (pPredictedData)
	{
		for (auto it = pPredictedData->mapDeepCatchPredictResultRoi.begin(); it != pPredictedData->mapDeepCatchPredictResultRoi.end(); ++it)
		{
			proj.mapPredictResultUID[it.key().toLocal8Bit().toStdString()] = it.value();
			qDebug() << "DeepCatch predict key :" << it.key();
			qDebug() << "value UID :" << it.value();
		}
	}

	if (m_pWinManager->IsPredictComplete() && pPredictedData)
	{
		proj.stPredictOptInfo.bIsPredictComplete = m_pWinManager->IsPredictComplete();

		proj.stPredictOptInfo.nL3SliceNum = pPredictedData->nAxialDepth;
		proj.stPredictOptInfo.nAWSliceStartNum = pPredictedData->nStartAxialDepth;
		proj.stPredictOptInfo.nAWSliceEndNum = pPredictedData->nEndAxialDepth;

		proj.stPredictOptInfo.nProjectType = pPredictedData->stPredictOpt.nProjectType;
		proj.stPredictOptInfo.nUNET = pPredictedData->stPredictOpt.UNETType;
		proj.stPredictOptInfo.nContrast = pPredictedData->stPredictOpt.contrastType;
		proj.stPredictOptInfo.nAWPredict = pPredictedData->stPredictOpt.AWConfirmType;

		proj.stPredictOptInfo.nGender = pPredictedData->stPredictOpt.genderType;
		proj.stPredictOptInfo.nHeight = pPredictedData->stPredictOpt.strHeight.toInt();
		proj.stPredictOptInfo.nWeight = pPredictedData->stPredictOpt.strWeight.toInt();
		proj.stPredictOptInfo.nIsL3Auto = pPredictedData->stPredictOpt.singleSliceType;
		proj.stPredictOptInfo.nIsAWAuto = pPredictedData->stPredictOpt.multiSliceType;
		proj.stPredictOptInfo.nReportSliceType = pPredictedData->stPredictOpt.preferenceType;

		QStringList strListDate = pPredictedData->stPredictOpt.strReportingDate.split("-");
		proj.stPredictOptInfo.nPredictYear = strListDate[0].toInt();
		proj.stPredictOptInfo.nPredictMonth = strListDate[1].toInt();
		proj.stPredictOptInfo.nPredictDay = strListDate[2].toInt();

		proj.stPredictOptInfo.nAdditionalOptions = pPredictedData->stPredictOpt.AdditionalOptions;
		proj.stPredictOptInfo.nIOClassification = pPredictedData->stPredictOpt.bIOClassificationPredict ? 1 : 0;
		proj.stPredictOptInfo.nMuscleQualityMap = pPredictedData->stPredictOpt.bMuscleQualityMap ? 1 : 0;

		qDebug() << " pPredictedData->stPredictOpt.strHeight" << pPredictedData->stPredictOpt.strHeight;
		qDebug() << " pPredictedData->stPredictOpt.strWeight" << pPredictedData->stPredictOpt.strWeight;

		qDebug() << "proj.stPredictOptInfo.nHeight" << proj.stPredictOptInfo.nHeight;
		qDebug() << "proj.stPredictOptInfo.nWeight" << proj.stPredictOptInfo.nWeight;
	}


#if 1 //20210729_byPHS_ADD_AI_Result
	mint32 _maskInfoSize = m_pVolumeData->getMaskInfoListCnt();
	proj.vecAIResultInfo.assign(_maskInfoSize, MIP_ENCODER::AI_ResultInfo());

	for (int n = 0; n < _maskInfoSize; n++)
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(n);

		if (nullptr != info)
		{
			muint32 _uid = info->uid;
			for (auto i = 0; i < m_pVolumeData->m_vecAIResultData.size(); ++i)
			{
				if (m_pVolumeData->m_vecAIResultData[i].first == _uid)
				{
					int nOutsetVal = 256 / 2 - 1;
					std::vector<unsigned char>& AIResultData = m_pVolumeData->m_vecAIResultData[i].second;
					if (AIResultData.size() == 0)
					{
						proj.vecAIResultInfo[n].nUse_AIInfo = 0;
						//printf_s("\n Medip SAVE PASS [%d] nUse_AIInfo %d ", n, proj.vecAIResultInfo[n].nUse_AIInfo);
						continue;
					}

					muint32 AIOutSet_uid = m_pVolumeData->m_vecAIOutset[i].first;
					if (AIOutSet_uid == _uid)
						nOutsetVal = m_pVolumeData->m_vecAIOutset[i].second;

					proj.vecAIResultInfo[n].nUse_AIInfo = 1;
					proj.vecAIResultInfo[n].uid = _uid;
					proj.vecAIResultInfo[n].outSetVal = nOutsetVal;
					proj.vecAIResultInfo[n].AI_result_Size = AIResultData.size();

					proj.vecAIResultInfo[n].AI_result.reserve(AIResultData.size());
					proj.vecAIResultInfo[n].AI_result.assign(AIResultData.begin(), AIResultData.end());

					//printf("\n Medip SAVE [%d] nUse_AIInfo %d ", n, proj.vecAIResultInfo[n].nUse_AIInfo);
					//printf("\n Medip SAVE [%d] AI_result Size %d ", n, proj.vecAIResultInfo[n].AI_result.size());
					//printf("\n Medip SAVE [%d] nOutsetVal Size %d ", n, nOutsetVal);
					//printf("\n Medip SAVE [%d] _uid %d ", n, _uid);
					break;
				}

			}
		}
	}
#endif


	bool res = false;

	if (MIP_ENCODER::ERROR_MESSAGE err = MIP_ENCODER_SINGLTON->SaveProjectFile(m_mipFilePath, &proj))
	{
		m_pVolumeData->threadResult = err;
		emit finished();
	}
	else
	{
		res = true;
	}

	m_pWinManager->setSaveState(res);
	m_pVolumeData->threadResult = res;

	setProgressValue(100);

#else
	if (m_pVolumeData->isValidate() == false)
	{
		emit finished();
		return;
	}

	QSaveFile file(m_mipFilePath);

	if (!file.open(QSaveFile::WriteOnly))
	{
		emit finished();
		return;
	}

	setProgressValue(0, true);
	ProjectFile proj;

#ifndef MIP_OLD_VER
	proj.head.version = MIP20_FILE_VERSION;
#else
	proj.head.version = OLD_FILE_VERSION;
#endif
	proj.head.filetype = _type;
	proj.head.detailSize = sizeof(proj.detail);

	proj.head.fileSize = sizeof(proj.head) + sizeof(proj.detail);

	proj.detail.maskAddress = 0;
	proj.detail.dataLenth = m_pVolumeData->getVolumeDataLength();
	proj.detail.dataCX = m_pVolumeData->getCX();
	proj.detail.dataCY = m_pVolumeData->getCY();
	proj.detail.dataCZ = m_pVolumeData->getCZ();
	proj.detail.spaceX = m_pVolumeData->getSpaceX();
	proj.detail.spaceY = m_pVolumeData->getSpaceY();
	proj.detail.spaceZ = m_pVolumeData->getSpaceZ();
	proj.detail.right_hand_coord = 1;

	proj.detail.maskInfoSize = m_pVolumeData->getMaskInfoListCnt();
	for (int n = 0; n < proj.detail.maskInfoSize; n++)
	{
		if (n >= MASK_SECOND_MAX)
			break;
		MaskInfo* info = m_pVolumeData->getMaskInfo(n);
		if (nullptr != info)
		{
			proj.detail.maskInfo[n].color.a = 255;
			proj.detail.maskInfo[n].color = info->color;
			proj.detail.maskInfo[n].uid = info->uid;
			proj.detail.maskInfo[n].layerCustom = true;
			proj.detail.maskInfo[n].layerAlpha = info->layerAlpha;
			memcpy_s(proj.detail.maskInfo[n].maskName, sizeof(proj.detail.maskInfo[n].maskName),
				info->maskName, sizeof(info->maskName));
			proj.detail.maskInfo[n].mask_id = info->mask_id;
			if (info->uid >= MASK_SECOND_MAX && info->mask_id2 == 0)
				proj.detail.maskInfo[n].mask_id2 = VM_MASK0 << ((info->uid - (MASK_SECOND_MAX)) % 8);
			else
				proj.detail.maskInfo[n].mask_id2 = info->mask_id2;
			proj.detail.maskInfo[n].show = info->show;
			proj.detail.maskInfo[n].meshConnected = info->meshConnected;
		}

		if (m_pVolumeData->threadStop == true)
		{
			file.cancelWriting();
			emit finished();
			return;
		}
	}

	proj.detail.windowLevel = m_pWinManager->getWindowLevel();
	proj.detail.windowWidth = m_pWinManager->getWindowWidth();
	proj.detail.SelectedPreSet = m_pWinManager->getSelectedPreset();
	proj.detail.nDefaultLevel = m_pWinManager->getDefaultLevel();
	proj.detail.nDefaultWidth = m_pWinManager->getDefaultWidth();
	proj.detail.volumeLevel = m_pWinManager->getVolumeLevel();
	proj.detail.volumeWidth = m_pWinManager->getVolumeWidth();
	proj.detail.SelectedvolumePreSet = m_pWinManager->getSelectedVolumePreset();
	proj.detail.box = m_pVolumeData->getBoundingBox();
	proj.detail.volumeHUMin = m_pVolumeData->getHuMin();
	proj.detail.volumeHUMax = m_pVolumeData->getHuMax();
	proj.detail.CuspreVal = m_pWinManager->getSelectedCustomPreset(CL_2D);
	proj.detail.CusVolumepreVal = m_pWinManager->getSelectedCustomPreset(CL_3D);
	proj.detail.SelectedCustomPreset = -1 < proj.detail.CuspreVal;
	proj.detail.SelectedCustomVolumePreset = -1 < proj.detail.CusVolumepreVal;

	if (!(proj.head.filetype == PT_VOLUME_ROI_ONLY || proj.head.filetype == PT_VOLUME_ONLY)) // mask
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_MASK] = 1;

		proj.detail.maskSize = sizeof(mask) * proj.detail.dataLenth;

		proj.head.fileSize += proj.detail.maskSize;
	}

	if (proj.head.filetype != PT_MASK_ONLY) // volume
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_VOLUME] = 1;
		proj.detail.volumeSize = sizeof(short) * proj.detail.dataLenth;

		proj.detail.volumeAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.volumeSize;
	}

	if (m_pVolumeData->threadStop == true)
	{
		file.cancelWriting();
		file.commit();
		emit finished();
		return;
	}

	if (m_pWinManager->aniLine.count() != 0)
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_PATH] = 1;
		int val = 0;
		m_pWinManager->aniCount.removeAll(val);
		proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_PATH] = m_pWinManager->aniCount.count() - 1; // 최악임, 실제 Animation count보다 1 적음
		proj.detail.aniSize = m_pWinManager->aniCount.count() * (sizeof(mint32) * 11)
			+ (sizeof(float) * (m_pWinManager->aniLine.count() * 3)); // 이 다음 windows info
		proj.detail.AnimationAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.aniSize;
	}

	proj.detail.chunkInfo[PROJ_CHUNK::PC_WINDOWINFO] = 1;
	proj.detail.chunksubInfo[PROJ_CHUNK::PC_WINDOWINFO] = 3;

	proj.detail.WindowAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
	proj.head.fileSize += sizeof(WindowInfo) * 3;


	if (m_pWinManager->anotationList.size() > 0)
	{
		int annoTextCount = 0;
		int annoLengthCount = 0;
		int annoAngleCount = 0;
		int annoOvalCount = 0;
		int annoArrowCount = 0;

		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];
			if (a->getType() == AT_TEXT) annoTextCount++;
			else if (a->getType() == AT_LEN) annoLengthCount++;
			else if (a->getType() == AT_ANGLE) annoAngleCount++;
			else if (a->getType() == AT_OVAL) annoOvalCount++;
			else if (a->getType() == AT_ARROW) annoArrowCount++;
		}

		if (annoTextCount > 0)
		{
			proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_TEXT] = annoTextCount;
			proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_TEXT] = 1;
		}

		if (annoLengthCount > 0)
		{
			proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_LEN] = annoLengthCount;
			proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_LEN] = 1;
		}

		if (annoAngleCount > 0)
		{
			proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_ANGLE] = annoAngleCount;
			proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ANGLE] = 1;
		}

		if (annoOvalCount > 0)
		{
			proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_OVAL] = annoOvalCount;
			proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_OVAL] = 1;
		}

		if (annoArrowCount > 0)
		{
			proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_ARROW] = annoArrowCount;
			proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ARROW] = 1;
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_TEXT])
	{
		proj.detail.TxtAnnoAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += sizeof(AnnoTextInfo) * proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_TEXT];
	}

	if (proj.detail.maskInfoSize > MASK_SECOND_MAX)
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_MASK_2] = 1;

		proj.detail.maskSize2 = proj.detail.maskSize * m_pVolumeData->GetMaskPointCount();

		for (int i = MASK_SECOND_MAX; i < proj.detail.maskInfoSize; i++)
		{
			MaskInfo* info = m_pVolumeData->getMaskInfo(i);
			if (nullptr != info)
				proj.detail.maskSize2 += sizeof(ProjectMaskInfo);
		}

		proj.detail.mask2Address = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.maskSize2;
	}

	if ((proj.detail.SurfaceCount = m_pVolumeData->GetMeshCount()) != 0)
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE] = 1;
		proj.detail.SurfaceSize = proj.detail.SurfaceCount * (sizeof(mint32) * 12 + sizeof(mint8)) + proj.getsurfaceInfoSize();
		proj.detail.SurfaceAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.SurfaceSize;
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_LEN])
	{
		proj.detail.LenAnnoAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += sizeof(AnnoLengthInfo) * proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_LEN];
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ANGLE])
	{
		proj.detail.AngleAnnoAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += sizeof(AnnoAngleInfo) * proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_ANGLE];
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_OVAL])
	{
		proj.detail.OvalAnnoAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += sizeof(AnnoOvalInfo) * proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_OVAL];
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ARROW])
	{
		proj.detail.ArrowAnnoAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += sizeof(AnnoArrowInfo) * proj.detail.chunksubInfo[PROJ_CHUNK::PC_ANNO_ARROW];
	}

	int len = m_pWinManager->GetDicomInfo()->studyDescription.length();
	if (len > 1)
	{
		bool subWrite = len > sizeof(proj.detail.study_desc);
		len = subWrite ? len - sizeof(proj.detail.study_desc) : sizeof(proj.detail.study_desc);
		memcpy_s(proj.detail.study_desc, sizeof(proj.detail.study_desc),
			m_pWinManager->GetDicomInfo()->studyDescription.c_str(), sizeof(proj.detail.study_desc));

		if (subWrite)
		{
			proj.detail.chunkInfo[PROJ_CHUNK::PC_STUDY_DESC] = 1;
			proj.detail.subStudySize = len;
			proj.head.fileSize += len;
		}
	}
	setProgressValue(20);
	len = m_pWinManager->GetDicomInfo()->description_.length();
	if (len > 1)
	{
		bool subWrite = len > sizeof(proj.detail.series_desc);
		len = subWrite ? len - sizeof(proj.detail.series_desc) : sizeof(proj.detail.series_desc);
		memcpy_s(proj.detail.series_desc, sizeof(proj.detail.series_desc),
			m_pWinManager->GetDicomInfo()->description_.c_str(), sizeof(proj.detail.series_desc));


		if (subWrite)
		{
			proj.detail.chunkInfo[PROJ_CHUNK::PC_SERIES_DESC] = 1;
			proj.detail.subSeriesSize = len;
			proj.head.fileSize += len;
		}

	}
	//len = m_pWinManager->GetDicomInfo()->imagePosition.length();
	mip::VECTOR3 vImgPos = m_pVolumeData->getImgPos();
	if (vImgPos.x != 0 && vImgPos.y != 0 && vImgPos.z != 0)
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_IMG_POSITION] = 1;

		proj.detail.imgPos[0] = vImgPos.x;
		proj.detail.imgPos[1] = vImgPos.y;
		proj.detail.imgPos[2] = vImgPos.z;


		/*QString tmpStr = m_pWinManager->GetDicomInfo()->imagePosition.c_str();
		QStringList tmpList = tmpStr.split("/");

		if (tmpList.count() == 3)
		{
			for (int i = 0; i < 3; i++)
				proj.detail.imgPos[i] = tmpList.at(i).toFloat();

			proj.detail.chunkInfo[PROJ_CHUNK::PC_IMG_POSITION] = 1;
		}*/
	}

	QImage img = m_pWinManager->getThumbnail();

	if (img != QImage())
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_THUMBNAIL] = 1;
		proj.detail.ThumbSize = img.byteCount();
		proj.detail.ThumbnailAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.ThumbSize;
	}

	if (m_pWinManager->captureList.count() != 0)
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_CAPTURE_IMG] = 1;

		for (int i = 0; i < m_pWinManager->captureList.count(); i++)
		{
			QImage* Capimg = m_pWinManager->captureList.at(i);
			proj.detail.CaptureSize += Capimg->byteCount();
			proj.detail.CaptureSize += sizeof(int) * 3;
		}

		proj.detail.CaptureImgAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += proj.detail.CaptureSize;
		proj.detail.chunksubInfo[PROJ_CHUNK::PC_CAPTURE_IMG] = m_pWinManager->captureList.count();
	}

	proj.detail.updateReport = true;
	std::vector<std::pair<QImage, QString>> image_list = m_pWinManager->tempReportData.getImageList();
	ImageInfo* repImgInfo = nullptr;
	if (!m_pWinManager->tempReportData.isEmpty())
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_REPORT] = 1;
		proj.detail.reportInfo.imageCount = image_list.size();
		repImgInfo = new ImageInfo[proj.detail.reportInfo.imageCount];
		for (int i = 0; i < proj.detail.reportInfo.imageCount; ++i)
		{
			repImgInfo[i].width = image_list[i].first.width();
			repImgInfo[i].height = image_list[i].first.height();
			repImgInfo[i].size = image_list[i].first.byteCount();
			repImgInfo[i].nameLength = image_list[i].second.toLocal8Bit().size() + 1;
			proj.head.fileSize += repImgInfo[i].nameLength;
			proj.head.fileSize += repImgInfo[i].size;
		}
		proj.head.fileSize += sizeof(ImageInfo) * proj.detail.reportInfo.imageCount;
		proj.detail.reportInfo.htmlSize = m_pWinManager->tempReportData.getHTMLSize();
		proj.head.fileSize += proj.detail.reportInfo.htmlSize;
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE])
	{
		proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE_SUB] = 1;
		proj.detail.chunksubInfo[PROJ_CHUNK::PC_SURFACE_SUB] = proj.detail.SurfaceCount;

		int rBytes = proj.surfaceSubInfoWrite(file, true);

		proj.detail.SurfaceSubAddress = proj.head.fileSize - (sizeof(proj.head) + sizeof(proj.detail));
		proj.head.fileSize += rBytes;
	}

	if (m_pVolumeData->threadStop == true)
	{
		file.cancelWriting();
		file.commit();
		emit finished();
		return;
	}


	file.write((char*)&(proj.head), sizeof(ProjectHead));

	proj.detail.downScaleCnt = m_pVolumeData->getDownScaledCnt();

	VolumeView* volumeView = static_cast<VolumeView*>(m_pWinManager->mainSegmentWidget->getViewVolume());
	if (nullptr != volumeView)
	{
		//proj.detail.volumeRotation = volumeView->getVolRotation();
		proj.detail.volumeRotation = volumeView->getVolRotation();
		proj.detail.volumeTranslation = volumeView->getVolTranslation();
		proj.detail.volumeZoom = volumeView->getZoom();
	}

	proj.detail.isGamma = m_pWinManager->isGammaMode();
	proj.detail.fGamma = m_pWinManager->getGamma();
	proj.detail.modality_type = proj.getModalityType();

	for (int i = 0; i < 2; i++)
	{
		COLOR _col = m_pWinManager->getSeedColor(i);
		proj.detail.seedColor[i] = _col.allcolor;
	}





	file.write((char*)&(proj.detail), sizeof(ProjectDetail));
	setProgressValue(40);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////CHUNKINFO WRITE////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_MASK])
	{
		file.write((char*)m_pVolumeData->getMaskDataPoint(), proj.detail.maskSize);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_VOLUME])
	{
		file.write((char*)m_pVolumeData->getHUDataPoint(), proj.detail.volumeSize);
	}
	setProgressValue(60);

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_PATH])
	{
		for (int i = 0; i < m_pWinManager->aniCount.count(); i++)
			proj.annoPathWrite(file, i);
	}

	if (m_pVolumeData->threadStop == true)
	{
		file.cancelWriting();
		file.commit();
		emit finished();
		return;
	}
	proj.windowInfoWrite(file, WT_AXIAL);
	proj.windowInfoWrite(file, WT_CORONAL);
	proj.windowInfoWrite(file, WT_SAGGITAL);

	if (m_pWinManager->anotationList.size() > 0 && proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_TEXT])
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];

			if (a->getType() != AT_TEXT) continue;

			proj.annoTextWrite(file, a, proj.head.fileSize);

			if (m_pVolumeData->threadStop == true)
			{
				file.cancelWriting();
				file.commit();
				emit finished();
				return;
			}
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_MASK_2])
	{
		proj.maskInfoWrite(file);
	}
	setProgressValue(80);

	if (m_pVolumeData->threadStop == true)
	{
		file.cancelWriting();
		file.commit();
		emit finished();
		return;
	}
	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE])
		proj.surfaceInfoWrite(file);

	if (m_pWinManager->anotationList.size() > 0 && proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_LEN])
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];

			if (a->getType() != AT_LEN) continue;

			proj.annoLengthWrite(file, a);

			if (m_pVolumeData->threadStop == true)
			{
				file.cancelWriting();
				file.commit();
				emit finished();
				return;
			}
		}
	}
	if (m_pWinManager->anotationList.size() > 0 && proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ANGLE])
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];

			if (a->getType() != AT_ANGLE) continue;

			proj.annoAngleWrite(file, a);
		}
	}

	if (m_pWinManager->anotationList.size() > 0 && proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_OVAL])
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];

			if (a->getType() != AT_OVAL) continue;

			proj.annoOvalWrite(file, a);

			if (m_pVolumeData->threadStop == true)
			{
				file.cancelWriting();
				file.commit();
				emit finished();
				return;
			}
		}
	}

	if (m_pWinManager->anotationList.size() > 0 && proj.detail.chunkInfo[PROJ_CHUNK::PC_ANNO_ARROW])
	{
		for (int n = 0; n < m_pWinManager->anotationList.size(); n++)
		{
			Annotation* a = m_pWinManager->anotationList[n];

			if (a->getType() != AT_ARROW) continue;

			proj.annoArrowWrite(file, a);

			if (m_pVolumeData->threadStop == true)
			{
				file.cancelWriting();
				file.commit();
				emit finished();
				return;
			}
		}
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_THUMBNAIL])
	{
		uchar* dt = img.bits();
		file.write((char*)dt, img.byteCount());
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_CAPTURE_IMG])
	{
		for (int i = 0; i < m_pWinManager->captureList.count(); i++)
		{
			QImage* Capimg = m_pWinManager->captureList.at(i);
			uchar* dt = Capimg->bits();
			int _num = 0;
			_num = Capimg->width();
			file.write((char*)&_num, sizeof(int));
			_num = Capimg->height();
			file.write((char*)&_num, sizeof(int));
			_num = Capimg->byteCount();
			file.write((char*)&_num, sizeof(int));
			file.write((char*)dt, _num);
		}
	}

	setProgressValue(90);
	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_STUDY_DESC])
	{
		const char* desc = m_pWinManager->GetDicomInfo()->studyDescription.c_str();

		file.write(desc + sizeof(proj.detail.study_desc), proj.detail.subStudySize);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SERIES_DESC])
	{
		const char* desc = m_pWinManager->GetDicomInfo()->description_.c_str();

		file.write(desc + sizeof(proj.detail.series_desc), proj.detail.subSeriesSize);
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_REPORT])
	{
		for (int i = 0; i < image_list.size(); ++i)
		{
			file.write((char*)&repImgInfo[i].width, sizeof(mint32));
			file.write((char*)&repImgInfo[i].height, sizeof(mint32));
			file.write((char*)&repImgInfo[i].size, sizeof(mint32));
			file.write((char*)&repImgInfo[i].nameLength, sizeof(mint32));
			//이미지 이름 저장
			QByteArray& text = m_pWinManager->tempReportData.getImageList()[i].second.toLocal8Bit();
			char* textData = new char[repImgInfo[i].nameLength];
			strcpy(textData, text.data());
			file.write(textData, repImgInfo[i].nameLength);
			SAFE_DELETES(textData);
			//이미지 저장
			uchar* dt = image_list[i].first.bits();
			file.write((char*)dt, repImgInfo[i].size);
		}

		QByteArray& text = m_pWinManager->tempReportData.getHTML().toLocal8Bit();
		char* textData = new char[proj.detail.reportInfo.htmlSize];
		strcpy(textData, text.data());
		file.write(textData, proj.detail.reportInfo.htmlSize);

		SAFE_DELETES(textData);
		SAFE_DELETES(repImgInfo);
		m_pWinManager->tempReportData.clear();
	}

	if (proj.detail.chunkInfo[PROJ_CHUNK::PC_SURFACE_SUB])
		proj.surfaceSubInfoWrite(file);

	setProgressValue(100);

	bool res = file.commit();

	m_pWinManager->setSaveState(res);
	m_pVolumeData->threadResult = res;

#endif

	emit finished();
}