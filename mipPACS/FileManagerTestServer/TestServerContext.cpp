#include "stdafx.h"
#include "TestServerContext.h"

using namespace fm;

TestServerContext::TestServerContext() :
	m_Server(nullptr)
{
}

TestServerContext::~TestServerContext()
{
}

bool TestServerContext::Initialize()
{
	Deinitialize();


	if (m_Server == nullptr)
	{
		m_Server = new fm::Server();
	}
	return true;
}

bool TestServerContext::Deinitialize()
{
	if (m_Server != nullptr)
	{
		delete m_Server;
		m_Server = nullptr;
	}
	return true;
}

fm::Server * TestServerContext::GetServer()
{
	return m_Server;
}

TestServerContext g_testServerContext;
