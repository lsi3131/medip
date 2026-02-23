#pragma once
#include "./DllSupport.h"

#ifdef RADIOMICS_EXPORTS
enum class Radiomics_API FeatureClass {
#else
enum class FeatureClass {
#endif

	FIRSTORDER,
	SHAPE3D,
	SHAPE2D,	
	
	GLCM,
	GLRLM,
	GLSZM,
	NGTDM,
	GLDM,
};

