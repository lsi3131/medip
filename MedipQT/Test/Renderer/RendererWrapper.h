#pragma once

#include <memory>
#include "Renderer/Renderer.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshEdit/CMeshModelViewManager.h"


class RendererWrapper
{
public:
	static std::shared_ptr<RendererWrapper> DefaultForTest()
	{
		return std::make_shared<RendererWrapper>();
	}
public:
	RendererWrapper()
	{
		int width = 800;
		int height = 600;

		Data.init3D(width, height);
		Data.initShader();
		Data.doneCurrent();
	}

	~RendererWrapper()
	{
		Data.release();
	}

public:
	mip::Renderer Data;
};

