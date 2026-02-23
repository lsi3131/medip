#include "stdafx.h"
#include "TASubClassResultCtrl.h"
#include "System/stringManager.h"

using namespace std;

TASubClassResultCtrl::TASubClassResultCtrl()
{
	// set classification name
	//m_vecClassfyName.push_back(MAJOR_CLASSIFICATION, "Size and shape based-features");
	int nMajorNum = 0, nMiddleNum = 0, nMinorNum = 0;
	std::string strTmp;
	// 0 ���� 1���� ����
 	vector<pair<QString, double>> vecSub;
	vector<pair<QString, vector<pair<QString, double>>>> vecMiddle;

	// 1-1
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MESH_VOLUME), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_VOXEL_VOLUME), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_SURFACE_AREA), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_SURFACE_AREA_TO_VOLUME_RATIO), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_SPHERICITY), 0.f));
	/*vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_COMPACTNESS_1), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_COMPACTNESS_2), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_SPHERICAL_DISPROPORTION), 0.f));*/
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_3D_DIAMETER), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_SLICE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_COLUMN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MAXIMUM_2D_DIAMETER_ROW), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MAJOR_AXIS_LENGTH), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_MINOR_AXIS_LENGTH), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_LEAST_AXIS_LENGTH), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_ELONGATION), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB3D_FLAATNESS), 0.f));

	vecMiddle.emplace_back(pair< QString, vector<pair<QString, double>> >(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_3D), std::move(vecSub)));	
	
	// Shape2D
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_MESH_SURFACE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_PIXEL_SURFACE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_PERIMETER), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_PERIMETER_TO_SURFACE_RATIO), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_SPHERICITY), 0.f));
	//vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_SPHERICAL_DISPROPORTION), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_MAXIMUM_2D_DIAMETER), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_MAJOR_AXIS_LENGTH), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_MINOR_AXIS_LENGTH), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_SB2D_ELONGATION), 0.f));
	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_SHAPE_BASED_2D), std::move(vecSub)));	

	m_vecClassify.emplace_back(pair<QString, vector<pair<QString, vector<pair<QString, double>>>>>(STRING_MANAGER->getString(STR_TA_MAJOR_SIZE_AND_SHAPE_BASED_FEATURES), std::move(vecMiddle)));

	//2
	vecMiddle.clear();
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_ENERGY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_TOTAL_ENERGY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_ENTROPY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_MINIMUM), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_10TH_PERCENTILE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_90TH_PERCENTILE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_MAXIMUM), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_MEAN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_MEDIAN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_INTERQUARTILE_RANGE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_RANGE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_MEAN_ABSOLUTE_DEVIATION_MAD), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_ROBUST_MEAN_ABSOLUTE_DEVIATION_rMAD), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_ROOT_MEAN_SQUARED_RMS), 0.f));
	//vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_STANDARD_DEVIATION), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_SKEWNESS), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_KURTOSIS), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_VARIANCE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_FOS_UNIFORMITY), 0.f));
	
	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_FIRST_ORDER_STATISTICS), std::move(vecSub)));

	m_vecClassify.emplace_back(pair<QString, vector<pair<QString, vector<pair<QString, double>>>>>(STRING_MANAGER->getString(STR_TA_MAJOR_HISTOGRAM_BASED_FEATURES), std::move(vecMiddle)));

	// 	3 -1
	vecMiddle.clear();
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_AUTOCORRELATION), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_JOINT_AVERAGE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_CLUSTER_PROMINENCE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_CLUSTER_SHADE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_CLUSTER_TENDENCY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_CONTRAST), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_CORRELATION), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_DIFFERENCE_AVERAGE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_DIFFERENCE_ENTROPY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_DIFFERENCE_VARIANCE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_JOINT_ENERGY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_JOINT_ENTROPY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_1), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_2), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_IDM), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_MAXIMAL_CORRELATION_COEFFICIENT_MCC), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_NORMALIZED_IDMN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INVERSE_DIFFERENCE_ID), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INVERSE_DIFFERENCE_NORMALIZED_IDN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_INVERSE_VARIANCE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_MAXIMUM_PROBABILITY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_SUM_AVERAGE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_SUM_ENTROPY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLCM_SUM_OF_SQUARES), 0.f));

	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_GRAY_LEVEL_COOCCURRENCE_MATRIX), std::move(vecSub)));

	// 	3-2
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_SHORT_RUN_EMPHASIS_SRE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_LONG_RUN_EMPHASIS_LRE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_GLN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_RLN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_NORMALIZED_RLNN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_RUN_PERCENTAGE_RP), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_GRAY_LEVEL_VARIANCE_GLV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_RUN_VARIANCE_RV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_RUN_ENTROPY_RE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_LOW_GRAY_LEVEL_RUN_EMPHASIS_LGLRE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_HIGH_GRAY_LEVEL_RUN_EMPHASIS_HGLRE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_SHORT_RUN_LOW_GRAY_LEVEL_EMPHASIS_SRLGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_SHORT_RUN_HIGH_GRAY_LEVEL_EMPHASIS_SRHGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_LONG_RUN_LOW_GRAY_LEVEL_EMPHASIS_LRLGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLRLM_LONG_RUN_HIGH_GRAY_LEVEL_EMPHASIS_LRHGLE), 0.f));
		
	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_GRAY_LEVEL_RUN_LENGTH_MATRIX), std::move(vecSub)));

	// 	3-3
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_SMALL_AREA_EMPHASIS_SAE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_LARGE_AREA_EMPHASIS_LAE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_GLN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY__SZN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY_NORMALIZED_SZNN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_ZONE_PERCENTAGE_ZP), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_GRAY_LEVEL_VARIANCE_GLV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_ZONE_VARIANCE_ZV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_ZONE_ENTROPY_ZE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_LOW_GRAY_LEVEL_ZONE_EMPHASIS_LGLZE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_HIGH_GRAY_LEVEL_ZONE_EMPHASIS_HGLZE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_SMALL_AREA_LOW_GRAY_LEVEL_EMPHASIS_SALGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_SMALL_AREA_HIGH_GRAY_LEVEL_EMPHASIS_SAHGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_LARGE_AREA_LOW_GRAY_LEVEL_EMPHASIS_LALGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLSZM_LARGE_AREA_HIGH_GRAY_LEVEL_EMPHASIS_LAHGLE), 0.f));
	
	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_GRAY_LEVEL_SIZE_ZONE_MATRIX), std::move(vecSub)));

	// 	3-4
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_NGTDM_COARSENESS), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_NGTDM_CONTRAST), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_NGTDM_BUSYNESS), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_NGTDM_COMPLEXITY), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_NGTDM_STRENGTH), 0.f));

	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_NEIGHBOURING_GRAY_TONE_DIFFERENCE_MATRIX), std::move(vecSub)));

	// 3-5
	vecSub.clear();
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_SMALL_DEPENDENCE_EMPHASIS_SDE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_LARGE_DEPENDENCE_EMPHASIS_LDE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_GRAY_LEVEL_NON_UNIFORMITY_GLN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_DN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_NORMALIZED_DNN), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_GRAY_LEVEL_VARIANCE_GLV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_DEPENDENCE_VARIANCE_DV), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_DEPENDENCE_ENTROPY_DE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_LOW_GRAY_LEVEL_EMPHASIS_LGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_HIGH_GRAY_LEVEL_EMPHASIS_HGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_SMALL_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_SDLGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_SMALL_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_LARGE_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_LDLGLE), 0.f));
	vecSub.emplace_back(pair<QString, double>(STRING_MANAGER->getString(STR_TA_GLDM_LARGE_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE), 0.f));

	vecMiddle.emplace_back(pair<QString, vector<pair<QString, double>>>(STRING_MANAGER->getString(STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX), std::move(vecSub)));
	
	m_vecClassify.emplace_back(pair<QString, vector<pair<QString, vector<pair<QString, double>>>>>(STRING_MANAGER->getString(STR_TA_MAJOR_RELATIONSHIPS_BETWEEN_IMAGE_VOXELS_BASED_FEATURES), std::move(vecMiddle)));

}

TASubClassResultCtrl::~TASubClassResultCtrl()
{
	qDebug() << "destructor TA result " << endl;
}

// TASubClassResultCtrl * TASubClassResultCtrl::getSingleton()
// {
// 	static TASubClassResultCtrl instance;
// 	return &instance;
// }

int TASubClassResultCtrl::getMajorClassSize(void)
{
	return m_vecClassify.size();
}

QString TASubClassResultCtrl::getMajorClassName(int nNum)
{	
	return m_vecClassify[nNum].first;
}

int TASubClassResultCtrl::getMiddleClassSize(int nMajorNum)
{
	return m_vecClassify[nMajorNum].second.size();
}

QString TASubClassResultCtrl::getMiddleClassName(int nMajorNum, int nNum)
{
	return m_vecClassify[nMajorNum].second[nNum].first;
}

int TASubClassResultCtrl::getMinorClassSize(int nMajorNum, int nMiddleNum)
{
	return m_vecClassify [nMajorNum].second[nMiddleNum].second.size();
}

QString TASubClassResultCtrl::getMinorClassName(int nMajorNum, int nMiddleNum, int nNum)
{
	return m_vecClassify[nMajorNum].second[nMiddleNum].second[nNum].first;
}

bool TASubClassResultCtrl::setFeatureValue(const QString &strMajor, const QString &strMiddle, const QString &strMinor, double dVal)
{
	bool bRet = false;

	for (auto i = m_vecClassify.begin(); i != m_vecClassify.end(); ++i)
	{
		if (!strMajor.compare(i->first) || !strMajor.compare(""))
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				if (!strMiddle.compare(j->first))
				{
					for (auto k = j->second.begin(); k != j->second.end(); ++k)
					{
						if (!strMinor.compare(k->first))
						{
							bRet = true;
							k->second = dVal;
						}
					}
				}
			}
		}
	}
	
	return bRet;
}

double TASubClassResultCtrl::getFeatureValue(const QString &strMajor, const QString &strMiddle, const QString &strMinor)
{
	for (auto i = m_vecClassify.begin(); i != m_vecClassify.end(); ++i)
	{
		if (!strMajor.compare(i->first) || !strMajor.compare(""))
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				if (!strMiddle.compare(j->first))
				{
					for (auto k = j->second.begin(); k != j->second.end(); ++k)
					{
						if (!strMinor.compare(k->first))
						{							
							return k->second;
						}
					}
				}
			}
		}
	}
	return 0.f;
}

double TASubClassResultCtrl::getFeatureValue(int nMajorNum, int nMiddleNum, int nNum)
{
	return m_vecClassify[nMajorNum].second[nMiddleNum].second[nNum].second;
}

