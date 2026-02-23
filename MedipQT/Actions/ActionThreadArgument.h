#pragma once

#include "defineMEDIP.h"
#include "Actions/ActionDefinitions.h"
#include <QThread>
#include <memory>

class ActionThreadArgument
{
public:
	ActionThreadArgument(ACTION_PROCESSING eCurrent, eAfterTHREAD eNext, std::shared_ptr<void> pUserData, QThread* pThread = nullptr)
	{
		eCurrentThread = eCurrent;
		eNextThread = eNext;
		pTempData = pUserData;
		pWorkThread = pThread;
	}

public:
	ACTION_PROCESSING eCurrentThread;				// Current Execute thread
	eAfterTHREAD eNextThread;				// Next Execute thread
	std::shared_ptr<void> pTempData;	// input:current thread, use:next thread end
	QThread* pWorkThread = nullptr;
};

