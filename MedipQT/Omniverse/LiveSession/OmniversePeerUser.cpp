#include "stdafx.h"
#include "OmniversePeerUser.h"

OmniversePeerUser::OmniversePeerUser(const std::string& id, const std::string& name, const std::string& app) :
	m_userID(id),
	m_userName(name),
	m_app(app)
{
}

bool OmniversePeerUser::operator==(const OmniversePeerUser& rhs) const
{
	return
		m_userID == rhs.m_userID &&
		m_userName == rhs.m_userName &&
		m_app == rhs.m_app;
}

bool OmniversePeerUser::operator!=(const OmniversePeerUser& rhs) const
{
	return !(*this == rhs);
}

bool OmniversePeerUser::operator<(const OmniversePeerUser& rhs) const
{
	return (m_userName + m_app + m_userID) < (rhs.m_userName + rhs.m_app + rhs.m_userID);
}

bool OmniversePeerUser::operator>(const OmniversePeerUser& rhs) const
{
	return (m_userName + m_app + m_userID) > (rhs.m_userName + rhs.m_app + rhs.m_userID);
}

std::string OmniversePeerUser::GetUserID() const
{
	return m_userID;
}

std::string OmniversePeerUser::GetUserName() const
{
	return m_userName;
}

std::string OmniversePeerUser::GetApp() const
{
	return m_app;
}

