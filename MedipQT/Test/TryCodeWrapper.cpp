#include "stdafx.h"
#include "TryCodeWrapper.h"

int TryCodeWrapper::Do(int argc, char** argv, bool allowMultiProcess)
{
	return 0;
}

int TryCodeWrapper::Do(const QStringList& args, bool allowMultiProcess)
{
	int argc = args.size();
	char* argv[1024];
	for (int i = 0; i < args.size(); ++i)
	{
		std::string arg_str = args[i].toStdString();
		int size = arg_str.size() + 1;
		argv[i] = new char[size];
		memcpy(argv[i], arg_str.c_str(), size);
	}

	return Do(argc, argv, allowMultiProcess);
}

