#pragma once

#include "updaterlib_global.h"
#include "updaterlib_Defines.h"

class updateProcess;

class UPDATERLIB_EXPORT updaterLib
{
public:
	static updaterLib *getInstance() {
		static updaterLib instance;
		return &instance;
	}

	void setEvent(void *pUIEvent, void(*pEventProcess)(void *pEvent));
	quint16 isAvailableUpdate(void *pUIEvent);

	// get url
	quint16 getUrlForLicenseServer(void *pUIEvent);

private:
	updaterLib();

	updateProcess *m_pUpdateProcess = nullptr;

};
