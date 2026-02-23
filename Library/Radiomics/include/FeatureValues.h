#pragma once

#include <string>

namespace FeatureValues 
{
	namespace FirstOrder {
		const std::string ENERGY = "FirstOrder_Energy";
		const std::string TOTAL_ENERGY = "FirstOrder_TotalEnergy";
		const std::string ENTROPY = "FirstOrder_Entropy";
		const std::string MINIMUM = "FirstOrder_Minimum";
		const std::string PERCENTILE_10 = "FirstOrder_10Percentile";
		const std::string PERCENTILE_90 = "FirstOrder_90Percentile";
		const std::string MAXIMUM = "FirstOrder_Maximum";
		const std::string MEAN = "FirstOrder_Mean";
		const std::string MEDIAN = "FirstOrder_Median";
		const std::string INTERQUARTILE_RANGE = "FirstOrder_InterquartileRange";
		const std::string RANGE = "FirstOrder_Range";
		const std::string MEAN_ABSOLUTE_DEVIATION = "FirstOrder_MeanAbsoluteDeviation";
		const std::string ROBUST_MEAN_ABSOLUTE_DEVIATION = "FirstOrder_RobustMeanAbsoluteDeviation";
		const std::string ROOT_MEAN_SQUARED = "FirstOrder_RootMeanSquared";
		const std::string SKEWNESS = "FirstOrder_Skewness";
		const std::string KURTOSIS = "FirstOrder_Kurtosis";
		const std::string VARIANCE = "FirstOrder_Variance";
		const std::string UNIFORMITY = "FirstOrder_Uniformity";
	}

	namespace Shape3D {
		const std::string MESH_VOLUME = "Shape3D_MeshVolume";
		const std::string VOXEL_VOLUME = "Shape3D_VoxelVolume";
		const std::string SURFACE_AREA = "Shape3D_SurfaceArea";
		const std::string SURFACE_VOLUME_RATIO = "Shape3D_SurfaceVolumeRatio";
		const std::string SPHERICITY = "Shape3D_Sphericity";
		const std::string MAXIMUM_3D_DIAMETER = "Shape3D_Maximum3DDiameter";
		const std::string MAXIMUM_2D_DIAMETER_SLICE = "Shape3D_Maximum2DDiameterSlice";
		const std::string MAXIMUM_2D_DIAMETER_COLUMN = "Shape3D_Maximum2DDiameterColumn";
		const std::string MAXIMUM_2D_DIAMETER_ROW = "Shape3D_Maximum2DDiameterRow";
		const std::string MAJOR_AXIS_LENGTH = "Shape3D_MajorAxisLength";
		const std::string MINOR_AXIS_LENGTH = "Shape3D_MinorAxisLength";
		const std::string LEAST_AXIS_LENGTH = "Shape3D_LeastAxisLength";
		const std::string ELONGATION = "Shape3D_Elongation";
		const std::string FLATNESS = "Shape3D_Flatness";
	}

	namespace Shape2D {
		const std::string MESH_SURFACE = "Shape2D_MeshSurface";
		const std::string PIXEL_SURFACE = "Shape2D_PixelSurface";
		const std::string PERIMETER = "Shape2D_Perimeter";
		const std::string PERIMETER_SURFACE_RATIO = "Shape2D_PerimeterSurfaceRatio";
		const std::string SPHERICITY = "Shape2D_Sphericity";
		const std::string MAXIMUM_DIAMETER = "Shape2D_MaximumDiameter";
		const std::string MAJOR_AXIS_LENGTH = "Shape2D_MajorAxisLength";
		const std::string MINOR_AXIS_LENGTH = "Shape2D_MinorAxisLength";
		const std::string ELONGATION = "Shape2D_Elongation";
	}

	namespace GLCM {
		const std::string AUTO_CORRELATION = "GLCM_Autocorrelation";
		const std::string JOINT_AVERAGE = "GLCM_JointAverage";
		const std::string CLUSTER_PROMINENCE = "GLCM_ClusterProminence";
		const std::string CLUSTER_SHADE = "GLCM_ClusterShade";
		const std::string CLUSTER_TENDENCY = "GLCM_ClusterTendency";
		const std::string CONTRAST = "GLCM_Contrast";
		const std::string CORRELATION = "GLCM_Correlation";
		const std::string DIFFERENCE_AVERAGE = "GLCM_DifferenceAverage";
		const std::string DIFFERENCE_ENTROPY = "GLCM_DifferenceEntropy";
		const std::string DIFFERENCE_VARIANCE = "GLCM_DifferenceVariance";
		const std::string JOINT_ENERGY = "GLCM_JointEnergy";
		const std::string JOINT_ENTROPY = "GLCM_JointEntropy";
		const std::string IMC1 = "GLCM_Imc1";
		const std::string IMC2 = "GLCM_Imc2";
		const std::string IDM = "GLCM_Idm";
		const std::string MCC = "GLCM_MCC";
		const std::string IDMN = "GLCM_Idmn";
		const std::string ID = "GLCM_Id";
		const std::string IDN = "GLCM_Idn";
		const std::string INVERSE_VARIANCE = "GLCM_InverseVariance";
		const std::string MAXIMUM_PROBABILITY = "GLCM_MaximumProbability";
		const std::string SUM_AVERAGE = "GLCM_SumAverage";
		const std::string SUM_ENTROPY = "GLCM_SumEntropy";
		const std::string SUM_SQUARES = "GLCM_SumSquares";
	}

	namespace GLRLM {
		const std::string SHORT_RUN_EMPHASIS = "GLCM_ShortRunEmphasis";
		const std::string LONG_RUN_EMPHASIS = "GLCM_LongRunEmphasis";
		const std::string GRAY_LEVEL_NON_UNIFORMITY = "GLCM_GrayLevelNonUniformity";
		const std::string GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED = "GLCM_GrayLevelNonUniformityNormalized";
		const std::string RUN_LENGTH_NON_UNIFORMITY = "GLCM_RunLengthNonUniformity";
		const std::string RUN_LENGTH_NON_UNIFORMITY_NORMALIZED = "GLCM_RunLengthNonUniformityNormalized";
		const std::string RUN_PERCENTAGE = "GLCM_RunPercentage";
		const std::string GRAY_LEVEL_VARIANCE = "GLCM_GrayLevelVariance";
		const std::string RUN_VARIANCE = "GLCM_RunVariance";
		const std::string RUN_ENTROPY = "GLCM_RunEntropy";
		const std::string LOW_GRAY_LEVEL_RUN_EMPHASIS = "GLCM_LowGrayLevelRunEmphasis";
		const std::string HIGH_GRAY_LEVEL_RUN_EMPHASIS = "GLCM_HighGrayLevelRunEmphasis";
		const std::string SHORT_RUN_LOW_GRAY_LEVEL_EMPHASIS = "GLCM_ShortRunLowGrayLevelEmphasis";
		const std::string SHORT_RUN_HIGH_GRAY_LEVEL_EMPHASIS = "GLCM_ShortRunHighGrayLevelEmphasis";
		const std::string LONG_RUN_LOW_GRAY_LEVEL_EMPHASIS = "GLCM_LongRunLowGrayLevelEmphasis";
		const std::string LONG_RUN_HIGH_GRAY_LEVEL_EMPHASIS = "GLCM_LongRunHighGrayLevelEmphasis";
	}

	namespace GLSZM {
		const std::string SMALL_AREA_EMPHASIS = "GLSZM_SmallAreaEmphasis";
		const std::string LARGE_AREA_EMPHASIS = "GLSZM_LargeAreaEmphasis";
		const std::string GRAY_LEVEL_NON_UNIFORMITY = "GLSZM_GrayLevelNonUniformity";
		const std::string GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED = "GLSZM_GrayLevelNonUniformityNormalized";
		const std::string SIZE_ZONE_NON_UNIFORMITY = "GLSZM_SizeZoneNonUniformity";
		const std::string SIZE_ZONE_NON_UNIFORMITY_NORMALIZED = "GLSZM_SizeZoneNonUniformityNormalized";
		const std::string ZONE_PERCENTAGE = "GLSZM_ZonePercentage";
		const std::string GRAY_LEVEL_VARIANCE = "GLSZM_GrayLevelVariance";
		const std::string ZONE_VARIANCE = "GLSZM_ZoneVariance";
		const std::string ZONE_ENTROPY = "GLSZM_ZoneEntropy";
		const std::string LOW_GRAY_LEVEL_ZONE_EMPHASIS = "GLSZM_LowGrayLevelZoneEmphasis";
		const std::string HIGH_GRAY_LEVEL_ZONE_EMPHASIS = "GLSZM_HighGrayLevelZoneEmphasis";
		const std::string SMALL_AREA_LOW_GRAY_LEVEL_EMPHASIS = "GLSZM_SmallAreaLowGrayLevelEmphasis";
		const std::string SMALL_AREA_HIGH_GRAY_LEVEL_EMPHASIS = "GLSZM_SmallAreaHighGrayLevelEmphasis";
		const std::string LARGE_AREA_LOW_GRAY_LEVEL_EMPHASIS = "GLSZM_LargeAreaLowGrayLevelEmphasis";
		const std::string LARGE_AREA_HIGH_GRAY_LEVEL_EMPHASIS = "GLSZM_LargeAreaHighGrayLevelEmphasis";
	}

	namespace GLDM {
		const std::string SMALL_DEPENDENCE_EMPHASIS = "GLDM_SmallDependenceEmphasis";
		const std::string LARGE_DEPENDENCE_EMPHASIS = "GLDM_LargeDependenceEmphasis";
		const std::string GRAY_LEVEL_NON_UNIFORMITY = "GLDM_GrayLevelNonUniformity";
		const std::string DEPENDENCE_NON_UNIFORMITY = "GLDM_DependenceNonUniformity";
		const std::string DEPENDENCE_NON_UNIFORMITY_NORMALIZED = "DependenceNonUniformityNormalized";
		const std::string GRAY_LEVEL_VARIANCE = "GLDM_GrayLevelVariance";
		const std::string DEPENDENCE_VARIANCE = "GLDM_DependenceVariance";
		const std::string DEPENDENCE_ENTROPY = "GLDM_DependenceEntropy";
		const std::string LOW_GRAY_LEVEL_EMPHASIS = "GLDM_LowGrayLevelEmphasis";
		const std::string HIGH_GRAY_LEVEL_EMPHASIS = "GLDM_HighGrayLevelEmphasis";
		const std::string SMALL_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS = "GLDM_SmallDependenceLowGrayLevelEmphasis";
		const std::string SMALL_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS = "GLDM_SmallDependenceHighGrayLevelEmphasis";
		const std::string LARGE_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS = "GLDM_LargeDependenceLowGrayLevelEmphasis";
		const std::string LARGE_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS = "GLDM_LargeDependenceHighGrayLevelEmphasis";
	}

	namespace NGTDM {
		const std::string COARSENESS = "Coarseness";
		const std::string CONTRAST = "Contrast";
		const std::string BUSYNESS = "Busyness";
		const std::string COMPLEXITY = "Complexity";
		const std::string STRENGTH = "Strength";
	}
}

