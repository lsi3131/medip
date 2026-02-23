#include "stdafx.h"
#include "MacroArgumentParser.h"
#include "Parser/MacroArgumentParser_Omniverse.h"

bool MacroArgumentParser::Parse(MacroParameter* pOut, QStringList args, QString* pOutErrorMessage)
{
	args.pop_front();

	QString macroName;
	QStringListIterator it(args);

	if (it.hasNext() == false)
	{
		qWarning() << "macro option('-macro') is empty";
		return false;
	}

	QString macroOption = it.next();
	if (macroOption != "-macro")
	{
		qWarning() << "argument string does not '-macro' option. but :"<<macroOption;
		return false;
	}

	if (it.hasNext() == false)
	{
		qWarning() << "macro name is empty.";
		return false;
	}
	pOut->MacroName = it.next();

	if (parseMacroArgumentToParameter(pOut, pOut->MacroName, it, pOutErrorMessage) == false)
	{
		qWarning() << "fail to parse macro argument";
		return false;
	}

	return true;
}

bool MacroArgumentParser::parseMacroArgumentToParameter(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage)
{
	if (
		macro == MACRO_OMNIVERSE_UPLOAD_USD ||
		macro == MACRO_OMNIVERSE_CREATE_USD_MODEL ||
		macro == MACRO_OMNIVERSE_CREATE_MESH_STL
		)
	{
		return MacroArgumentParser_Omniverse().Parse(pOutParameter, macro, it, pOutErrorMessage);
	}
	else if (macro == MACRO_DUMI_TEST)
	{
		return true;
	}
	else
	{
		qWarning() << "invalid omniverse macro name : " << macro;
		return false;
	}
}

