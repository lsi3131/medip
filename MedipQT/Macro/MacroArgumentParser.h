#pragma once

#include <QStringList>
#include "Macro/MacroDefines.h"

class MacroArgumentParser
{
public:
	static bool Parse(MacroParameter* pOut, QStringList args, QString* pOutErrorMessage = nullptr);

private:
	static bool parseMacroArgumentToParameter(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage);

};
