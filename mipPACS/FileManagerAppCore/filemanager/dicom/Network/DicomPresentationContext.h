#pragma once

#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmnet/assoc.h"

class DicomPresentationContext
{
public:
	DicomPresentationContext(
		OFString abstractSyntaxName,
		T_ASC_SC_ROLE roleSelect);
public:
	OFString AbstractSyntaxName;
	T_ASC_SC_ROLE RoleSelect;
	const char* TransferSyntaxes[3];
	int NumTransferSyntaxes;


};

