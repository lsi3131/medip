#pragma once

#ifndef MACRORUNNER_H
#define MACRORUNNER_H

#include "Macro/MacroDefines.h"

class OmniverseContext;
class WindowManager;

class MacroRunner
{
public:
	static bool RunWithArgments(QStringList args, QString* pOutErrorMessage);

private:
	static bool ParseMacroArgumentToParameter(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage);
	static bool ParseMacroArgumentToParameter_Export(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage);

public:
	MacroRunner(OmniverseContext* pOmniverse, WindowManager* pWindowManager);

public:
	bool Run(QStringList args, QString* pOutErrorMessage = nullptr);

private:
	OmniverseContext* m_pOmniverse;
	WindowManager* m_pWindowManager;
};
#endif
