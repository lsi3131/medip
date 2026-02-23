#include "stdafx.h"
#include "ActionCalculateFeature.h"
#include "algorithm/Radiomics.h"
#include "Windows/windowManager.h"
#include "System/stringManager.h"
#include "Windows/Main/MainTAWidget.h"
#include "algorithm/MagicCut.h"
#include <utility> 

//>> Radiomics
#include "Bound3D.h"
#include "MaskInfoHelper.h"
#include "FeatureValues.h"
#include "FeatureResponsor.h"
#include "SimpleLinkedList.h"
//<< Radiomics


ActionCalculateFeature::ActionCalculateFeature()
{
}


ActionCalculateFeature::~ActionCalculateFeature()
{
}

void ActionCalculateFeature::redo(void)
{

}

//>> WorkCalculateFeature

WorkCalculateFeature::WorkCalculateFeature(VOLUME_DATA* pVolumeData, const QString &strGroupName, muint32 uid, TASubClassResultCtrl* pResult, bool _bShape3DWithoutDiameter)
{
	m_pVolumeData = pVolumeData;

	m_strGroupName = strGroupName;
	m_pResult = pResult;
	m_uid = uid;

	this->setFeatureMapping();

	numProgress = 0;

	this->bShape3DWithoutDiameter = _bShape3DWithoutDiameter;
	m_nLoopCount = -1;
}

void WorkCalculateFeature::threadRun()
{
	//
	setProgressValue(0, true);
	//qDebug() << "m_strGroupName: " << m_strGroupName;
	if (!m_strGroupName.compare("All Calculate", Qt::CaseInsensitive))
	{
		int increaseValue = 10;
		setProgressValue(increaseValue, false);
		m_nLoopCount = 0;
		for (int i = STR_TA_MIDDLE_FIRST_ORDER_STATISTICS; i <= STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX; i++)
		{
			RSTRING str = static_cast<RSTRING>(i);

			ImageArray<short> *cropImage = new ImageArray<short>;
			ImageArray<int> *cropMask = new ImageArray<int>;

			makeCropItem(m_uid, cropImage, cropMask);
			map<RSTRING, double> listFeatureValue = processFeature(str, cropImage, cropMask);

			//
			updateFeatureValueString(m_pResult, str, listFeatureValue);
			//debugPrintFeatureValue("FeatureValues", listFeatureValue);

			// 
			destroyImageArray<short>(cropImage);
			destroyImageArray<int>(cropMask);
			m_nLoopCount++;
		}
		setProgressValue(100);
		emit finished();
	}
	else 
	{
		RSTRING currentFeatureCategory;
		currentFeatureCategory = findRSTRING(m_strGroupName);
		if (!isValidCategory(currentFeatureCategory))
		{
			setProgressValue(100);
			emit finished();
		}
		else {

			int increaseValue = 10;

			setProgressValue(increaseValue, false);

			// 
			ImageArray<short> *cropImage = new ImageArray<short>;
			ImageArray<int> *cropMask = new ImageArray<int>;

			makeCropItem(m_uid, cropImage, cropMask);
			map<RSTRING, double> listFeatureValue = processFeature(currentFeatureCategory, cropImage, cropMask);

			//
			updateFeatureValueString(m_pResult, currentFeatureCategory, listFeatureValue);
			//debugPrintFeatureValue("FeatureValues", listFeatureValue);

			// 
			destroyImageArray<short>(cropImage);
			destroyImageArray<int>(cropMask);

			setProgressValue(100);
			emit finished();
		}
	}
}

void WorkCalculateFeature::setProgressValue(int increaseValue, bool resetProgress)
{
	static int val = 0;

	if (resetProgress)
	{
		this->numProgress = 0;

		val = 0;
		emit progress(val);
	}
	else {

		this->numProgress += increaseValue;

		if (100 <= this->numProgress) {
			emit progress(100);
		}
		else {
			val = this->numProgress;
			emit progress(val);
		}
	}
}

void WorkCalculateFeature::_setProgressValue(int increaseValue, bool resetProgress)
{
	if (!m_strGroupName.compare("All Calculate", Qt::CaseInsensitive))
	{
		int totalCount = STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX - STR_TA_MIDDLE_FIRST_ORDER_STATISTICS + 1;

		static int val = 0;

		this->numProgress += increaseValue * ((float)(m_nLoopCount+1)/totalCount);

		if (100 <= this->numProgress) {
			emit progress(100);
		}
		else {
			val = this->numProgress;
			emit progress(val);
		}
	}
	else
	{
		static int val = 0;

		if (resetProgress)
		{
			this->numProgress = 0;

			val = 0;
			emit progress(val);
		}
		else {

			this->numProgress += increaseValue;

			if (100 <= this->numProgress) {
				emit progress(100);
			}
			else {
				val = this->numProgress;
				emit progress(val);
			}
		}
	}
}

void WorkCalculateFeature::updateProgress(int value, void *inst)
{
	WorkCalculateFeature* worker = (WorkCalculateFeature*)inst;
	worker->_setProgressValue(value);
}

void WorkCalculateFeature::setFeatureMapping()
{
	//
	featureMapping.insert({ FeatureValues::FirstOrder::ENERGY, STR_TA_FOS_ENERGY });
	featureMapping.insert({ FeatureValues::FirstOrder::TOTAL_ENERGY, STR_TA_FOS_TOTAL_ENERGY });
	featureMapping.insert({ FeatureValues::FirstOrder::ENTROPY, STR_TA_FOS_ENTROPY });
	featureMapping.insert({ FeatureValues::FirstOrder::MINIMUM, STR_TA_FOS_MINIMUM });
	featureMapping.insert({ FeatureValues::FirstOrder::PERCENTILE_10, STR_TA_FOS_10TH_PERCENTILE });
	featureMapping.insert({ FeatureValues::FirstOrder::PERCENTILE_90, STR_TA_FOS_90TH_PERCENTILE });
	featureMapping.insert({ FeatureValues::FirstOrder::MAXIMUM, STR_TA_FOS_MAXIMUM });
	featureMapping.insert({ FeatureValues::FirstOrder::MEAN, STR_TA_FOS_MEAN });
	featureMapping.insert({ FeatureValues::FirstOrder::MEDIAN, STR_TA_FOS_MEDIAN });
	featureMapping.insert({ FeatureValues::FirstOrder::INTERQUARTILE_RANGE, STR_TA_FOS_INTERQUARTILE_RANGE });
	featureMapping.insert({ FeatureValues::FirstOrder::RANGE, STR_TA_FOS_RANGE });
	featureMapping.insert({ FeatureValues::FirstOrder::MEAN_ABSOLUTE_DEVIATION, STR_TA_FOS_MEAN_ABSOLUTE_DEVIATION_MAD });
	featureMapping.insert({ FeatureValues::FirstOrder::ROBUST_MEAN_ABSOLUTE_DEVIATION, STR_TA_FOS_ROBUST_MEAN_ABSOLUTE_DEVIATION_rMAD });
	featureMapping.insert({ FeatureValues::FirstOrder::ROOT_MEAN_SQUARED, STR_TA_FOS_ROOT_MEAN_SQUARED_RMS });
	featureMapping.insert({ FeatureValues::FirstOrder::SKEWNESS, STR_TA_FOS_SKEWNESS });
	featureMapping.insert({ FeatureValues::FirstOrder::KURTOSIS, STR_TA_FOS_KURTOSIS });
	featureMapping.insert({ FeatureValues::FirstOrder::VARIANCE, STR_TA_FOS_VARIANCE });
	featureMapping.insert({ FeatureValues::FirstOrder::UNIFORMITY, STR_TA_FOS_UNIFORMITY });

	//
	featureMapping.insert({ FeatureValues::Shape3D::MESH_VOLUME, STR_TA_SB3D_MESH_VOLUME });
	featureMapping.insert({ FeatureValues::Shape3D::VOXEL_VOLUME, STR_TA_SB3D_VOXEL_VOLUME });
	featureMapping.insert({ FeatureValues::Shape3D::SURFACE_AREA, STR_TA_SB3D_SURFACE_AREA });
	featureMapping.insert({ FeatureValues::Shape3D::SURFACE_VOLUME_RATIO, STR_TA_SB3D_SURFACE_AREA_TO_VOLUME_RATIO });
	featureMapping.insert({ FeatureValues::Shape3D::SPHERICITY, STR_TA_SB3D_SPHERICITY });
	featureMapping.insert({ FeatureValues::Shape3D::MAXIMUM_3D_DIAMETER, STR_TA_SB3D_MAXIMUM_3D_DIAMETER });
	featureMapping.insert({ FeatureValues::Shape3D::MAXIMUM_2D_DIAMETER_SLICE, STR_TA_SB3D_MAXIMUM_2D_DIAMETER_SLICE });
	featureMapping.insert({ FeatureValues::Shape3D::MAXIMUM_2D_DIAMETER_COLUMN, STR_TA_SB3D_MAXIMUM_2D_DIAMETER_COLUMN });
	featureMapping.insert({ FeatureValues::Shape3D::MAXIMUM_2D_DIAMETER_ROW, STR_TA_SB3D_MAXIMUM_2D_DIAMETER_ROW });
	featureMapping.insert({ FeatureValues::Shape3D::MAJOR_AXIS_LENGTH, STR_TA_SB3D_MAJOR_AXIS_LENGTH });
	featureMapping.insert({ FeatureValues::Shape3D::MINOR_AXIS_LENGTH, STR_TA_SB3D_MINOR_AXIS_LENGTH });
	featureMapping.insert({ FeatureValues::Shape3D::LEAST_AXIS_LENGTH, STR_TA_SB3D_LEAST_AXIS_LENGTH });
	featureMapping.insert({ FeatureValues::Shape3D::ELONGATION, STR_TA_SB3D_ELONGATION });
	featureMapping.insert({ FeatureValues::Shape3D::FLATNESS, STR_TA_SB3D_FLAATNESS });

	//
	featureMapping.insert({ FeatureValues::Shape2D::MESH_SURFACE, STR_TA_SB2D_MESH_SURFACE });
	featureMapping.insert({ FeatureValues::Shape2D::PIXEL_SURFACE, STR_TA_SB2D_PIXEL_SURFACE });
	featureMapping.insert({ FeatureValues::Shape2D::PERIMETER, STR_TA_SB2D_PERIMETER });
	featureMapping.insert({ FeatureValues::Shape2D::PERIMETER_SURFACE_RATIO, STR_TA_SB2D_PERIMETER_TO_SURFACE_RATIO });
	featureMapping.insert({ FeatureValues::Shape2D::SPHERICITY, STR_TA_SB2D_SPHERICITY });
	featureMapping.insert({ FeatureValues::Shape2D::MAXIMUM_DIAMETER, STR_TA_SB2D_MAXIMUM_2D_DIAMETER });
	featureMapping.insert({ FeatureValues::Shape2D::MAJOR_AXIS_LENGTH, STR_TA_SB2D_MAJOR_AXIS_LENGTH });
	featureMapping.insert({ FeatureValues::Shape2D::MINOR_AXIS_LENGTH, STR_TA_SB2D_MINOR_AXIS_LENGTH });
	featureMapping.insert({ FeatureValues::Shape2D::ELONGATION, STR_TA_SB2D_ELONGATION });

	//
	featureMapping.insert({ FeatureValues::GLCM::AUTO_CORRELATION, STR_TA_GLCM_AUTOCORRELATION });
	featureMapping.insert({ FeatureValues::GLCM::JOINT_AVERAGE, STR_TA_GLCM_JOINT_AVERAGE });
	featureMapping.insert({ FeatureValues::GLCM::CLUSTER_PROMINENCE, STR_TA_GLCM_CLUSTER_PROMINENCE });
	featureMapping.insert({ FeatureValues::GLCM::CLUSTER_SHADE, STR_TA_GLCM_CLUSTER_SHADE });
	featureMapping.insert({ FeatureValues::GLCM::CLUSTER_TENDENCY, STR_TA_GLCM_CLUSTER_TENDENCY });
	featureMapping.insert({ FeatureValues::GLCM::CONTRAST, STR_TA_GLCM_CONTRAST });
	featureMapping.insert({ FeatureValues::GLCM::CORRELATION, STR_TA_GLCM_CORRELATION });
	featureMapping.insert({ FeatureValues::GLCM::DIFFERENCE_AVERAGE, STR_TA_GLCM_DIFFERENCE_AVERAGE });
	featureMapping.insert({ FeatureValues::GLCM::DIFFERENCE_ENTROPY, STR_TA_GLCM_DIFFERENCE_ENTROPY });
	featureMapping.insert({ FeatureValues::GLCM::DIFFERENCE_VARIANCE, STR_TA_GLCM_DIFFERENCE_VARIANCE });
	featureMapping.insert({ FeatureValues::GLCM::JOINT_ENERGY, STR_TA_GLCM_JOINT_ENERGY });
	featureMapping.insert({ FeatureValues::GLCM::JOINT_ENTROPY, STR_TA_GLCM_JOINT_ENTROPY });
	featureMapping.insert({ FeatureValues::GLCM::IMC1, STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_1 });
	featureMapping.insert({ FeatureValues::GLCM::IMC2, STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_2 });
	featureMapping.insert({ FeatureValues::GLCM::IDM, STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_IDM });
	featureMapping.insert({ FeatureValues::GLCM::MCC, STR_TA_GLCM_MAXIMAL_CORRELATION_COEFFICIENT_MCC });
	featureMapping.insert({ FeatureValues::GLCM::IDMN, STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_NORMALIZED_IDMN });
	featureMapping.insert({ FeatureValues::GLCM::ID, STR_TA_GLCM_INVERSE_DIFFERENCE_ID });
	featureMapping.insert({ FeatureValues::GLCM::IDN, STR_TA_GLCM_INVERSE_DIFFERENCE_NORMALIZED_IDN });
	featureMapping.insert({ FeatureValues::GLCM::INVERSE_VARIANCE, STR_TA_GLCM_INVERSE_VARIANCE });
	featureMapping.insert({ FeatureValues::GLCM::MAXIMUM_PROBABILITY, STR_TA_GLCM_MAXIMUM_PROBABILITY });
	featureMapping.insert({ FeatureValues::GLCM::SUM_AVERAGE, STR_TA_GLCM_SUM_AVERAGE });
	featureMapping.insert({ FeatureValues::GLCM::SUM_ENTROPY, STR_TA_GLCM_SUM_ENTROPY });
	featureMapping.insert({ FeatureValues::GLCM::SUM_SQUARES, STR_TA_GLCM_SUM_OF_SQUARES });

	//
	featureMapping.insert({ FeatureValues::GLRLM::SHORT_RUN_EMPHASIS, STR_TA_GLRLM_SHORT_RUN_EMPHASIS_SRE });
	featureMapping.insert({ FeatureValues::GLRLM::LONG_RUN_EMPHASIS, STR_TA_GLRLM_LONG_RUN_EMPHASIS_LRE });
	featureMapping.insert({ FeatureValues::GLRLM::GRAY_LEVEL_NON_UNIFORMITY, STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_GLN });
	featureMapping.insert({ FeatureValues::GLRLM::GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED, STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN });
	featureMapping.insert({ FeatureValues::GLRLM::RUN_LENGTH_NON_UNIFORMITY, STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_RLN });
	featureMapping.insert({ FeatureValues::GLRLM::RUN_LENGTH_NON_UNIFORMITY_NORMALIZED, STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_NORMALIZED_RLNN });
	featureMapping.insert({ FeatureValues::GLRLM::RUN_PERCENTAGE, STR_TA_GLRLM_RUN_PERCENTAGE_RP });
	featureMapping.insert({ FeatureValues::GLRLM::GRAY_LEVEL_VARIANCE, STR_TA_GLRLM_GRAY_LEVEL_VARIANCE_GLV });
	featureMapping.insert({ FeatureValues::GLRLM::RUN_VARIANCE, STR_TA_GLRLM_RUN_VARIANCE_RV });
	featureMapping.insert({ FeatureValues::GLRLM::RUN_ENTROPY, STR_TA_GLRLM_RUN_ENTROPY_RE });
	featureMapping.insert({ FeatureValues::GLRLM::LOW_GRAY_LEVEL_RUN_EMPHASIS, STR_TA_GLRLM_LOW_GRAY_LEVEL_RUN_EMPHASIS_LGLRE });
	featureMapping.insert({ FeatureValues::GLRLM::HIGH_GRAY_LEVEL_RUN_EMPHASIS, STR_TA_GLRLM_HIGH_GRAY_LEVEL_RUN_EMPHASIS_HGLRE });
	featureMapping.insert({ FeatureValues::GLRLM::SHORT_RUN_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLRLM_SHORT_RUN_LOW_GRAY_LEVEL_EMPHASIS_SRLGLE });
	featureMapping.insert({ FeatureValues::GLRLM::SHORT_RUN_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLRLM_SHORT_RUN_HIGH_GRAY_LEVEL_EMPHASIS_SRHGLE });
	featureMapping.insert({ FeatureValues::GLRLM::LONG_RUN_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLRLM_LONG_RUN_LOW_GRAY_LEVEL_EMPHASIS_LRLGLE });
	featureMapping.insert({ FeatureValues::GLRLM::LONG_RUN_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLRLM_LONG_RUN_HIGH_GRAY_LEVEL_EMPHASIS_LRHGLE });

	//
	featureMapping.insert({ FeatureValues::GLSZM::SMALL_AREA_EMPHASIS, STR_TA_GLSZM_SMALL_AREA_EMPHASIS_SAE });
	featureMapping.insert({ FeatureValues::GLSZM::LARGE_AREA_EMPHASIS, STR_TA_GLSZM_LARGE_AREA_EMPHASIS_LAE });
	featureMapping.insert({ FeatureValues::GLSZM::GRAY_LEVEL_NON_UNIFORMITY, STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_GLN });
	featureMapping.insert({ FeatureValues::GLSZM::GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED, STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN });
	featureMapping.insert({ FeatureValues::GLSZM::SIZE_ZONE_NON_UNIFORMITY, STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY__SZN });
	featureMapping.insert({ FeatureValues::GLSZM::SIZE_ZONE_NON_UNIFORMITY_NORMALIZED, STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY_NORMALIZED_SZNN });
	featureMapping.insert({ FeatureValues::GLSZM::ZONE_PERCENTAGE, STR_TA_GLSZM_ZONE_PERCENTAGE_ZP });
	featureMapping.insert({ FeatureValues::GLSZM::GRAY_LEVEL_VARIANCE, STR_TA_GLSZM_GRAY_LEVEL_VARIANCE_GLV });
	featureMapping.insert({ FeatureValues::GLSZM::ZONE_VARIANCE, STR_TA_GLSZM_ZONE_VARIANCE_ZV });
	featureMapping.insert({ FeatureValues::GLSZM::ZONE_ENTROPY, STR_TA_GLSZM_ZONE_ENTROPY_ZE });
	featureMapping.insert({ FeatureValues::GLSZM::LOW_GRAY_LEVEL_ZONE_EMPHASIS, STR_TA_GLSZM_LOW_GRAY_LEVEL_ZONE_EMPHASIS_LGLZE });
	featureMapping.insert({ FeatureValues::GLSZM::HIGH_GRAY_LEVEL_ZONE_EMPHASIS, STR_TA_GLSZM_HIGH_GRAY_LEVEL_ZONE_EMPHASIS_HGLZE });
	featureMapping.insert({ FeatureValues::GLSZM::SMALL_AREA_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLSZM_SMALL_AREA_LOW_GRAY_LEVEL_EMPHASIS_SALGLE });
	featureMapping.insert({ FeatureValues::GLSZM::SMALL_AREA_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLSZM_SMALL_AREA_HIGH_GRAY_LEVEL_EMPHASIS_SAHGLE });
	featureMapping.insert({ FeatureValues::GLSZM::LARGE_AREA_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLSZM_LARGE_AREA_LOW_GRAY_LEVEL_EMPHASIS_LALGLE });
	featureMapping.insert({ FeatureValues::GLSZM::LARGE_AREA_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLSZM_LARGE_AREA_HIGH_GRAY_LEVEL_EMPHASIS_LAHGLE });

	// 
	featureMapping.insert({ FeatureValues::NGTDM::COARSENESS, STR_TA_NGTDM_COARSENESS });
	featureMapping.insert({ FeatureValues::NGTDM::CONTRAST, STR_TA_NGTDM_CONTRAST });
	featureMapping.insert({ FeatureValues::NGTDM::BUSYNESS, STR_TA_NGTDM_BUSYNESS });
	featureMapping.insert({ FeatureValues::NGTDM::COMPLEXITY, STR_TA_NGTDM_COMPLEXITY });
	featureMapping.insert({ FeatureValues::NGTDM::STRENGTH, STR_TA_NGTDM_STRENGTH });

	// 
	featureMapping.insert({ FeatureValues::GLDM::SMALL_DEPENDENCE_EMPHASIS, STR_TA_GLDM_SMALL_DEPENDENCE_EMPHASIS_SDE });
	featureMapping.insert({ FeatureValues::GLDM::LARGE_DEPENDENCE_EMPHASIS, STR_TA_GLDM_LARGE_DEPENDENCE_EMPHASIS_LDE });
	featureMapping.insert({ FeatureValues::GLDM::GRAY_LEVEL_NON_UNIFORMITY, STR_TA_GLDM_GRAY_LEVEL_NON_UNIFORMITY_GLN });
	featureMapping.insert({ FeatureValues::GLDM::DEPENDENCE_NON_UNIFORMITY, STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_DN });
	featureMapping.insert({ FeatureValues::GLDM::DEPENDENCE_NON_UNIFORMITY_NORMALIZED, STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_NORMALIZED_DNN });
	featureMapping.insert({ FeatureValues::GLDM::GRAY_LEVEL_VARIANCE, STR_TA_GLDM_GRAY_LEVEL_VARIANCE_GLV });
	featureMapping.insert({ FeatureValues::GLDM::DEPENDENCE_VARIANCE, STR_TA_GLDM_DEPENDENCE_VARIANCE_DV });
	featureMapping.insert({ FeatureValues::GLDM::DEPENDENCE_ENTROPY, STR_TA_GLDM_DEPENDENCE_ENTROPY_DE });
	featureMapping.insert({ FeatureValues::GLDM::LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_LOW_GRAY_LEVEL_EMPHASIS_LGLE });
	featureMapping.insert({ FeatureValues::GLDM::HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_HIGH_GRAY_LEVEL_EMPHASIS_HGLE });
	featureMapping.insert({ FeatureValues::GLDM::SMALL_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_SMALL_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_SDLGLE });
	featureMapping.insert({ FeatureValues::GLDM::SMALL_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_SMALL_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE });
	featureMapping.insert({ FeatureValues::GLDM::LARGE_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_LARGE_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_LDLGLE });
	featureMapping.insert({ FeatureValues::GLDM::LARGE_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS, STR_TA_GLDM_LARGE_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE });

}

RSTRING WorkCalculateFeature::findRSTRING(QString valueStr)
{

	RSTRING retValue = STR_DEFAULT;
	LocMap *locMap = &(STRING_MANAGER->locMap);
	QMap<RSTRING, QString> *strList = &(STRING_MANAGER->strList);

	for (int i = STR_TA_MIDDLE_FIRST_ORDER_STATISTICS; i <= STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX; i++)
	{
		RSTRING str = static_cast<RSTRING>(i);

		//
		QMap<RSTRING, QString>::iterator result;

		locMap->loc = ENGLISH;
		locMap->rstr = str;
		result = strList->find(locMap->rstr);

		if (result != strList->end() && !valueStr.compare(result.value())) {
			retValue = str;
			break;
		}
	}

	return retValue;
}

bool WorkCalculateFeature::isValidCategory(RSTRING category) {
	RSTRING FIRST_FEATURE = STR_TA_MIDDLE_FIRST_ORDER_STATISTICS;
	RSTRING LAST_FEATURE = STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX;
	if (category < FIRST_FEATURE || category > LAST_FEATURE)
		return false;
	else
		return true;

}

void WorkCalculateFeature::makeCropItem(muint32 m_uid, ImageArray<short> *cropImage, ImageArray<int> *cropMask)
{

	MaskInfo *selectedMaskInfo = m_pVolumeData->getMaskInfo(m_uid, true);
	MaskInfoHelper maskInfo(m_pVolumeData, selectedMaskInfo);

	int sx = m_pVolumeData->getCX();
	int sy = m_pVolumeData->getCY();
	int sz = m_pVolumeData->getCZ();

	//
	Bound3D imageBound;
	Bound3D maskBound;

	int countMaskVoxel = 0;
	for (int z = 0; z < sz; z++) {
		for (int y = 0; y < sy; y++) {
			for (int x = 0; x < sx; x++) {
				imageBound.setMinMax(x, y, z);

				int index = z*sy*sx + y*sx + x;
				if (maskInfo.getMaskVolume()[index] & maskInfo.getMaskValue())
				{
					countMaskVoxel++;
					maskBound.setMinMax(x, y, z);
				}
			}
		}
	}

	cropImage->allocArray(maskBound.getSizeX(), maskBound.getSizeY(), maskBound.getSizeZ());
	cropMask->allocArray(maskBound.getSizeX(), maskBound.getSizeY(), maskBound.getSizeZ());
	for (int z = 0; z < maskBound.getSizeZ(); z++) {
		for (int y = 0; y < maskBound.getSizeY(); y++) {
			for (int x = 0; x < maskBound.getSizeX(); x++) {

				int index = (z + maskBound.minZ)*sy*sx + (y + maskBound.minY)*sx + x + maskBound.minX;
				int voxelValue = m_pVolumeData->getHUDataPoint()[index];

				cropImage->setElement(x, y, maskBound.getSizeZ() - 1 - z, voxelValue);

				if (maskInfo.getMaskVolume()[index] & maskInfo.getMaskValue()) {
					cropMask->setElement(x, y, maskBound.getSizeZ() - 1 - z, 1);
				}
			}
		}
	}
}

map<RSTRING, double> WorkCalculateFeature::processFeature(RSTRING currentFeatureClass, ImageArray<short> *cropImage, ImageArray<int> *cropMask)
{
	//
	std::map<RSTRING, FeatureClass> featureSetMapping;
	featureSetMapping.insert({ STR_TA_MIDDLE_FIRST_ORDER_STATISTICS, FeatureClass::FIRSTORDER });
	featureSetMapping.insert({ STR_TA_MIDDLE_SHAPE_BASED_3D, FeatureClass::SHAPE3D });
	featureSetMapping.insert({ STR_TA_MIDDLE_SHAPE_BASED_2D, FeatureClass::SHAPE2D });
	featureSetMapping.insert({ STR_TA_MIDDLE_GRAY_LEVEL_COOCCURRENCE_MATRIX, FeatureClass::GLCM });
	featureSetMapping.insert({ STR_TA_MIDDLE_GRAY_LEVEL_RUN_LENGTH_MATRIX, FeatureClass::GLRLM });
	featureSetMapping.insert({ STR_TA_MIDDLE_GRAY_LEVEL_SIZE_ZONE_MATRIX, FeatureClass::GLSZM });
	featureSetMapping.insert({ STR_TA_MIDDLE_NEIGHBOURING_GRAY_TONE_DIFFERENCE_MATRIX, FeatureClass::NGTDM });
	featureSetMapping.insert({ STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX, FeatureClass::GLDM });

	RSTRING featureCategory = currentFeatureClass;
	FeatureClass featureName = featureSetMapping.find(featureCategory)->second;

	map<RSTRING, double> listFeatureValue;
	map<std::string, double> featureValuesPair;
		
	FeatureResponsor responsor;
	InputInformation info;
	info.cropImage = new short[cropImage->getSize() * sizeof(short)];
	memcpy(info.cropImage, cropImage->ndarray, cropImage->getSize() * sizeof(short));
	info.cropMask = new int[cropMask->getSize() * sizeof(int)];
	memcpy(info.cropMask, cropMask->ndarray, cropMask->getSize() * sizeof(int));
	info.width = cropImage->sx;
	info.height = cropImage->sy;
	info.depth = cropImage->sz;
	info.spacingX = m_pVolumeData->getSpaceX(true);
	info.spacingY = m_pVolumeData->getSpaceY(true);
	info.spacingZ = m_pVolumeData->getSpaceZ(true);

	SimpleLinkedList *listValues;
	responsor.setInformation(&info);

	if (bShape3DWithoutDiameter)
		listValues = responsor.getFeatureValues_Shape3DWithoutDiameter(featureName, updateProgress, this);
	else 
		listValues = responsor.getFeatureValues(featureName, updateProgress, this);	

	/*Node *tempHead = listValues->getHead();
	while (tempHead != nullptr) {
		qDebug() << tempHead->featureValueName << " : " << tempHead->featureValue;
		tempHead = tempHead->next;
	}*/

	delete[] info.cropImage;
	delete[] info.cropMask;

	Node *head = listValues->getHead();
	while (head != nullptr) {
		std::string featureValueName = head->featureValueName;
		map<std::string, RSTRING>::iterator iter = featureMapping.find(featureValueName);
		if (iter != featureMapping.end()) {
			listFeatureValue.insert({ iter->second, head->featureValue });
		}
		head = head->next;
	}
	listValues->destroyList();
	delete listValues;

	return listFeatureValue;
}

void WorkCalculateFeature::updateFeatureValueString(TASubClassResultCtrl* m_pResult, RSTRING featureClass, map<RSTRING, double> listFeatureValue)
{
	for (map<RSTRING, double>::iterator iter = listFeatureValue.begin(); iter != listFeatureValue.end(); iter++) {
		m_pResult->setFeatureValue("",
			STRING_MANAGER->getString(featureClass),
			STRING_MANAGER->getString(iter->first),
			iter->second);
	}
}

void WorkCalculateFeature::debugPrintFeatureValue(QString featureClassName, map<RSTRING, double> listFeatureValue)
{
	qDebug("--->> %s -----------", featureClassName.toUtf8().constData());
	for (map<RSTRING, double>::iterator iter = listFeatureValue.begin(); iter != listFeatureValue.end(); iter++) {
		qDebug() << iter->second;
	}
	qDebug("---<< %s -----------", featureClassName.toUtf8().constData());
}

//<< WorkCalculateFeature

ActionCalculate3DMap::ActionCalculate3DMap()
{

}

ActionCalculate3DMap::~ActionCalculate3DMap()
{

}

WorkCalculate3DMap::WorkCalculate3DMap(VOLUME_DATA* pVolumeData, const QString &strGroupName, float* p3DVolumeMap, mint32 nROIIdx, std::vector<std::pair<float, float>> *pColorCategory)
{
	m_pVolumeData = pVolumeData;

	m_strGroupName = strGroupName;
	m_p3DVolumeMap = p3DVolumeMap;
	m_nROIIdx = nROIIdx;
	m_pColorCategory = pColorCategory;
}

WorkCalculate3DMap::~WorkCalculate3DMap()
{

}

void WorkCalculate3DMap::setProgressValue(int value, bool init /*= false*/)
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

void WorkCalculate3DMap::threadRun()
{	
	int nProgress = 10;
	setProgressValue(nProgress += 10, true);

	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int depth = m_pVolumeData->getCZ();
	MaskInfo * pMaskInfo = m_pVolumeData->getMaskInfo(m_nROIIdx, true);
	mask maskBit = pMaskInfo->uid >= MASK_SECOND_MAX ? pMaskInfo->mask_id2 : pMaskInfo->mask_id;
	
	int maskUid = m_pVolumeData->getCurrentMaskIndex();

	setProgressValue(nProgress += 10, true);
	if (!m_strGroupName.compare(STRING_MANAGER->getString(STR_TA_FOS_MEAN)))
	{
		int mMinimum = getMinimumFeatureValue(m_pVolumeData->getMaskDataPoint(maskUid), m_pVolumeData->getHUDataPoint()
			, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ(), maskBit);
		int mMaximum = getMaximumFeatureValue(m_pVolumeData->getMaskDataPoint(maskUid), m_pVolumeData->getHUDataPoint()
			, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ(), maskBit);

		mip::TA::generateAverageMap(m_pVolumeData->getMaskDataPoint(maskUid), m_pVolumeData->getHUDataPoint(), m_p3DVolumeMap, width, height, depth, maskBit);

		static int cnt = 0;
		std::vector<pair<float, float>> testMap;
		if (cnt <= 0)
		{
			testMap.push_back(pair<float, float>(0.0, mMaximum));
			testMap.push_back(pair<float, float>(0.25, (mMaximum + (mMaximum + mMinimum) / 2) / 2));
			testMap.push_back(pair<float, float>(0.5, (mMaximum + mMinimum)/2));
			testMap.push_back(pair<float, float>(0.75, (mMinimum + (mMaximum + mMinimum) / 2) / 2));
			testMap.push_back(pair<float, float>(1.0, mMinimum));
			cnt++;
		}
		else
		{
			testMap.push_back(pair<float, float>(0.0, mMaximum));
			testMap.push_back(pair<float, float>(0.25, (mMaximum + (mMaximum + mMinimum) / 2) / 2));
			testMap.push_back(pair<float, float>(0.5, (mMaximum + mMinimum) / 2));
			testMap.push_back(pair<float, float>(0.75, (mMinimum + (mMaximum + mMinimum) / 2) / 2));
			testMap.push_back(pair<float, float>(1.0, mMinimum));
		}
		(*m_pColorCategory) = std::move(testMap);
	}
	else
	{
		WIN_MANAGER->pRadiomics3DVolume = nullptr;
		setProgressValue(100);
		emit finished();
		return;
	}

	setProgressValue(70);

	WIN_MANAGER->pRadiomics3DVolume = m_p3DVolumeMap;
	WIN_MANAGER->vecRadiomicsColorCategory = *m_pColorCategory;
	WIN_MANAGER->bRadiomicsDataChangeFlag[WT_AXIAL - WT_AXIAL] = true;
	WIN_MANAGER->bRadiomicsDataChangeFlag[WT_CORONAL - WT_AXIAL] = true;
	WIN_MANAGER->bRadiomicsDataChangeFlag[WT_SAGITTAL - WT_AXIAL] = true;
	setProgressValue(100);
	emit finished();
	return;
}
