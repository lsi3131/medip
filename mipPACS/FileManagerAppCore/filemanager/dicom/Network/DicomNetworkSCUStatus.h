#pragma once

#include "filemanager/dicom/DicomDataset.h"
#include <string>

namespace fm
{
	struct DicomNetworkSCUStatus
	{
		EDcmNetworkDIMSEType DIMSE_Type;
		int InProgressCount;
		int MaxProgressCount;

		DicomNetworkSCUStatus() :
			DIMSE_Type(EDcmNetworkDIMSEType::ECHO_SCU),
			InProgressCount(0),
			MaxProgressCount(0)
		{
		}
		DicomNetworkSCUStatus(EDcmNetworkDIMSEType type) :
			DIMSE_Type(type),
			InProgressCount(0),
			MaxProgressCount(0)
		{}

		float ProgressRate()
		{
			if (MaxProgressCount <= 0)
			{
				return 0.0f;
			}
			else
			{
				return (float)InProgressCount / (float)MaxProgressCount;
			}
		}
	};

}
