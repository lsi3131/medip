#pragma once

#ifndef ANNOTATION_FACTORY_H
#define ANNOTATION_FACTORY_H

#include "anotation.h"
#include <memory>

class AnalMPRPlaneView;
class WindowBase;
class AnalVolumeView;

class AnnotationFactory
{
public:
	static std::unique_ptr<Annotation> createInstance(
		ANNOTATION_TYPE type,
		AnalMPRPlaneView * win, int x, int y,
		COLOR color,
		QString text, muint16 fontSize);

	static std::unique_ptr<Annotation> createInstance(
		ANNOTATION_TYPE type,
		WindowBase * win, int x, int y,
		COLOR color,
		QString text, muint16 fontSize);

	static std::unique_ptr<Annotation> createInstance(
		ANNOTATION_TYPE type,
		AnalVolumeView * win, std::vector<mip::VECTOR3> & vList,
		COLOR color,
		QString text, muint16 fontSize);
};
#endif