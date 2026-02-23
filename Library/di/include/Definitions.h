#pragma once

#define AI_SEGMENTATION_VERSION 1

enum class ClientType : unsigned int
{
	MEDIP, // assigned 0
	COVID19, //assigned 1
	DEEPCATCH, //assigned 2
	MEDIP_CREDIT, // assigned 3
	USER_CREATED, // assigned 4
	TISEPX_CREDIT, // assigned 5
};

enum class SegmentationArch : unsigned int
{
	UNET, //assigned 0
	//FRRN,		//1
	UNET3D=2,	//2
	UNET3DRev1,	//3
	UNET3DRev12,	//4
	UNETATTENTION,  //5
	NNUNET3D, //6
	NNUNET2D, //7
	NNUNETATTENTION, //8
	//RESNET = 2000, //
	//RESNET3D,
};

enum class ClassificationArch : unsigned int
{
	RESNET, //assigned 0
	EFFICIENTNET, //assigned 1
};


enum class TranslationArch : unsigned int
{
	PIX2PIX, //assigned 0
	PIX2PIX_HD, // 1
	UNET_GENERATOR64, // 2
	UNET_GENERATOR32, // 3
	TRANCE_2DUNET, // 4
	ED2NET_ADT_N2C, // 5
	ED2NET_PED_N2C, // 6
	ED2NET_ADT_C2N, // 7
};


enum class RegressionArch : unsigned int
{
	UNET_REGRESSION32,
};

enum class FeatureClassificationArch : unsigned int
{
	LINEAR_FULLY_CONNECTED,
};

enum class PredictionArch : unsigned int
{
	XGBOOST_HCC,
};


enum class Task : unsigned int
{
	SEGMENTATION2D, //0
	SEGMENTATION3D, //1
	FEATURE_CLASSIFICATION, //2
	TRANSLATION2D, //3
	CLASSIFICATION2D, //4
	REGRESSION2D, //5
	SEGMENTATION23D, //6, nnU-Net
	TRANSLATION_CE, //7
	PREDICTION, //8
};

enum class NormMethod : unsigned int
{
	LINEAR_WW_WL = 0,
	// ww, wl, target_low, target_high
	
	LINEAR_MIN_MAX = 1,
	// min_source, max_source, target_low, target_high
	
	LINEAR_CUSTOM = 2,
	// value 1, value ..., value n, target 1, target ..., target n	

	SELF_LINEAR_MIN_MAX = 3,
	//target_low, target_high || data in image will be used to count min/max_source

	SELF_STANDARDIZATION = 4,
	//No parameter, will do standardization to mean of 0 and std of 1

	NONE = 5,
	//No parameter, no normalization will be done

	ADD_SUBTRACT = 6,
	//variable (positive -> add, negative -> subtract)

	PROFNORM = 7,
	//No parameter, std and percentile

	TB = 8,
	//No parameter

	NN_CT = 9,
	//mean, std, percentile_00_5, percentile_99_5

	NN_CT2 = 10,
	//percentile_00_5, percentile_99_5

	NN_ELSE = 11,
};

enum class DenormMethod : unsigned int
{
	NO_DENORM = 0, //blank array
	LINEAR_MIN_MAX = 1,
	//class1:min_source, max_source, target_low, target_high | c... | classN
	ADD_SUBTRACT = 2,
	//variable (positive -> add, negative -> subtract)
};

enum class ActivationFunction : unsigned int
{
	RELU,	//0
	LRELU,	//1
};

enum class LossFunction : unsigned int
{
	Sigmoid,	//0
	Softmax,	//1
	DiceSigmoid,	//2
	DiceSoftmax,	//3
};
