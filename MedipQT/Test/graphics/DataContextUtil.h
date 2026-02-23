#pragma once

#include <memory>
#include "graphics/DataContext.h"

namespace mip
{
	class Renderer;
}

class DataContextUtil
{
public:
	static std::shared_ptr<DataContext> CreateWithRenderer(mip::Renderer* pRenderer);

};
