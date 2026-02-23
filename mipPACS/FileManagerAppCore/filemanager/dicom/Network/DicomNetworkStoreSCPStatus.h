#pragma once

#include "filemanager/dicom/DicomDataset.h"
#include <string>

namespace fm
{
	struct DicomNetworkStoreSCPStatus
	{
		fm::DicomDataset DcmDataset;

		DicomNetworkStoreSCPStatus()
		{
		}
	};
}
