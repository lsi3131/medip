#include "stdafx.h"
#include "DicomPresentationContext.h"

DicomPresentationContext::DicomPresentationContext(
	OFString abstractSyntaxName,
	T_ASC_SC_ROLE roleSelect)
{
	AbstractSyntaxName = abstractSyntaxName;
	RoleSelect = roleSelect;
	TransferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
	TransferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
	TransferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
	NumTransferSyntaxes = 3;
}
