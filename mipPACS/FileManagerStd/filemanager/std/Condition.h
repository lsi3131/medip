#pragma once

#include "filemanager/std/export.h"

namespace fm
{
	enum Status
	{
		FM_OK,
		FM_ERROR,
		FM_FAILURE
	};

	struct ConditionConst
	{
		unsigned short theModule;
		unsigned short theCode;
		Status theStatus;
		const char* Text;
	};

	extern FM_STD_EXPORT const ConditionConst Normal;
	extern FM_STD_EXPORT const ConditionConst MemoryExhausted;
	extern FM_STD_EXPORT const ConditionConst DcmNetInfoNotInitialized;
	extern FM_STD_EXPORT const ConditionConst DcmNetAssociationNegociation;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCUInitialize;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCPListenerInitialize;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCPDownloading;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCPReceiveAssociation;
	extern FM_STD_EXPORT const ConditionConst DcmNetInvalidDownloadProtocol;
	extern FM_STD_EXPORT const ConditionConst DcmNetDIMSEEcho;
	extern FM_STD_EXPORT const ConditionConst DcmNetDIMSEFind;
	extern FM_STD_EXPORT const ConditionConst DcmNetDIMSEMove;
	extern FM_STD_EXPORT const ConditionConst DcmNetDIMSEGet;
	extern FM_STD_EXPORT const ConditionConst DcmNetDIMSEStore;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCURelease;
	extern FM_STD_EXPORT const ConditionConst DcmNetSCPRelease;

	class FM_STD_EXPORT Condition
	{
	public:
		Condition(const ConditionConst& aConst = Normal)
			: m_condition(aConst)
		{
		}

		inline const char *text() const
		{
			return m_condition.Text;
		}

		inline bool good() const
		{
			Status s = m_condition.theStatus;
			return (s == FM_OK);
		}

		inline bool bad() const
		{
			Status s = m_condition.theStatus;
			return (s != FM_OK);
		}
	private:
		ConditionConst m_condition;
	};
};

#define makeConditionConst(name, module, code, status, text) \
  const fm::ConditionConst name = { (module), (code), (status), (text) }

