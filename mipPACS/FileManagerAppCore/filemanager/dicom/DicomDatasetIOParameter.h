#pragma once

#include <dcmtk/dcmdata/dcfilefo.h>
namespace fm
{
	class DicomDatasetSaveParameter
	{
	public:
		DicomDatasetSaveParameter()
		{
			WriteXfer = EXS_Unknown;
			EncodingType = EET_UndefinedLength;
			GroupLength = EGL_recalcGL;
			PadEncoding = EPD_noChange;
			PadLength = 0;
			SubPadLength = 0;
			WriteMode = EWM_createNewMeta;
		}
	public:
		E_TransferSyntax WriteXfer = EXS_Unknown;
		E_EncodingType EncodingType = EET_UndefinedLength;
		E_GrpLenEncoding GroupLength = EGL_recalcGL;
		E_PaddingEncoding PadEncoding = EPD_noChange;
		Uint32 PadLength;
		Uint32 SubPadLength;
		E_FileWriteMode WriteMode;
	};

	class DicomDatasetLoadParameter
	{
	public:
		DicomDatasetLoadParameter()
		{
			ReadXfer = EXS_Unknown;
			GroupLength = EGL_noChange;
			MaxReadLength = DCM_MaxReadLength;
			ReadMode = ERM_autoDetect;
		}
	public:
		E_TransferSyntax ReadXfer;
		E_GrpLenEncoding GroupLength;
		Uint32 MaxReadLength;
		E_FileReadMode ReadMode;
	};
}
