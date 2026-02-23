#pragma once
#include "./DllSupport.h"

#ifdef RADIOMICS_EXPORTS 
#include "algorithm/Radiomics/Interface/FeatureClass.h"
#include "algorithm/Radiomics/Interface/SimpleLinkedList.h"
#else
#include "FeatureClass.h"
#include "SimpleLinkedList.h"
#endif

typedef void(*progUpdatefunc)(int value, void *data);

struct InputInformation
{
	short *cropImage;
	int *cropMask;
	int width;
	int height;
	int depth;
	double spacingX;
	double spacingY;
	double spacingZ;
};

#ifdef RADIOMICS_EXPORTS 
class Radiomics_API FeatureResponsor
#else
class FeatureResponsor
#endif
{
public:
	void setInformation(InputInformation *info);
	SimpleLinkedList* getFeatureValues(FeatureClass featureClassName);
	SimpleLinkedList* getFeatureValues(FeatureClass featureClassName, progUpdatefunc updater, void *data = nullptr);		
	SimpleLinkedList* getFeatureValues_Shape3DWithoutDiameter(FeatureClass featureClassName, progUpdatefunc updater, void* data);

private:
	InputInformation *infoInput;	
};


