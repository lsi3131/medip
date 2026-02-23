#include "stdafx.h"
#include "DataContextUtil.h"

std::shared_ptr<DataContext> DataContextUtil::CreateWithRenderer(mip::Renderer* pRenderer)
{
	std::shared_ptr<DataContext> pDataContext = std::make_shared<DataContext>();

	pDataContext->m_MeshData.SetRenderer(pRenderer);

	return pDataContext;
}

