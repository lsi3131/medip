#pragma once

#include <string>

class OmniversePeerUser
{
public:
	OmniversePeerUser(const std::string& id, const std::string& name, const std::string& app);

public:
	bool operator==(const OmniversePeerUser& rhs) const;
	bool operator!=(const OmniversePeerUser& rhs) const;

	bool operator<(const OmniversePeerUser& rhs) const;
	bool operator>(const OmniversePeerUser& rhs) const;

	//struct Compare
	//{
	//	bool operator()(const OmniversePeerUser& lhs, const OmniversePeerUser& rhs) const
	//	{
	//		return (lhs.m_userName + lhs.m_app + lhs.m_userID) < (rhs.m_userName + rhs.m_app + rhs.m_userID);
	//	}
	//};

public:
	std::string GetUserID() const;
	std::string GetUserName() const;
	std::string GetApp() const;

public:
	std::string m_userID;
	std::string m_userName;
	std::string m_app;
};
