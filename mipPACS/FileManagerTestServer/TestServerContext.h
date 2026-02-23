#pragma once

#include "filemanager/net/Server.h"

class TestServerContext
{
public:
	TestServerContext();
	~TestServerContext();

	bool Initialize();
	bool Deinitialize();

public:
	fm::Server* GetServer();

private:
	fm::Server* m_Server;
};

extern TestServerContext g_testServerContext;
