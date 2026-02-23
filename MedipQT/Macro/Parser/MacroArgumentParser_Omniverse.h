#pragma once

#include "Macro/MacroDefines.h"

class MacroArgumentParser_Omniverse
{
public:
	MacroArgumentParser_Omniverse();

public:
	bool Parse(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage);

private:
	bool parseUploadUsd(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage);
	bool parseCreateUsdModel(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage);
	bool parseCreateMeshSTL(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage);
};
