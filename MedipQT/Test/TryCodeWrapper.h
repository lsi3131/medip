#pragma once

#ifndef TRYCODE_WRAPPER_H
#define TRYCODE_WRAPPER_H

#include "main.h"

class TryCodeWrapper
{
public:
	static int Do(int argc, char** argv, bool allowMultiProcess);
	static int Do(const QStringList& args, bool allowMultiProcess);
};
#endif