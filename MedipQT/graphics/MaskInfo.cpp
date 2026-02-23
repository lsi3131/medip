#include "stdafx.h"
#include "MaskInfo.h"

void MaskInfo::Copy(MaskInfo* pOutMaskInfo) const
{
	pOutMaskInfo->uid = uid;
	pOutMaskInfo->color = color;
	pOutMaskInfo->mask_id = mask_id;
	pOutMaskInfo->show = show;
	wcscpy(pOutMaskInfo->maskName, maskName);
	pOutMaskInfo->meshConnected = meshConnected;
	pOutMaskInfo->mask_id2 = mask_id2;
	pOutMaskInfo->layerAlpha = layerAlpha;
	pOutMaskInfo->bIsUseCredit = bIsUseCredit;
}

std::shared_ptr<MaskInfo> MaskInfo::Clone() const
{
	std::shared_ptr<MaskInfo> pCloneData = std::make_shared<MaskInfo>();
	Copy(pCloneData.get());

	return pCloneData;
}

bool MaskInfo::operator==(const MaskInfo& rhs) const
{
	return
		uid == rhs.uid &&
		color == rhs.color &&
		mask_id == rhs.mask_id &&
		show == rhs.show &&
		(wcscmp(maskName, rhs.maskName) == 0) &&
		mask_id2 == rhs.mask_id2 &&
		layerAlpha == rhs.layerAlpha &&
		bIsUseCredit == rhs.bIsUseCredit;
}

bool MaskInfo::operator!=(const MaskInfo& rhs) const
{
	return !(*this == rhs);
}

