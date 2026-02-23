#pragma once

#ifndef MASKINFO_H
#define MASKINFO_H

#include "color.h"
#include <memory>

#define	MASKINFO_TEXT_LENGTH_MAX	20

class vtkDataArray;
struct MaskInfo
{
	MaskInfo()
	{
		ZeroMemory(&maskName[0], sizeof(WCHAR) * MASKINFO_TEXT_LENGTH_MAX);
	};
	muint32	uid; // 0 ~ 29 ( MASK_MAX - 1 ) // ROI mask UID, del 시 mask 메모리 줄어들때 변경됌
	COLOR	color;
	mask	mask_id; // MASK2 ~ MASK7	// first mask byte used at mask array 
	bool	show;
	WCHAR	maskName[MASKINFO_TEXT_LENGTH_MAX];
	bool	meshConnected;
	mask	mask_id2;					// remain mask byte used at mask array 
	muint8	layerAlpha;
	bool bIsUseCredit = false;	// editable 변수로 사용함

	void Copy(MaskInfo* pOutMaskInfo) const;
	std::shared_ptr<MaskInfo> Clone() const;

	bool operator==(const MaskInfo& rhs) const;
	bool operator!=(const MaskInfo& rhs) const;

	// f:foreseed, b:backseed mask로 사용
	// mask_id |mask_id2-------------------
	//-------------------------------------
	//|fb234567|01234567|01234567|01234567|
	//-------------------------------------
	// uid
	//|fb012345|6-----------------------29|
};
#endif