#pragma once

namespace fm
{
	enum class ELicense
	{
		LITE,
		RESEARCH,
		PROFESSINAL,
		MDBOX,
		DEEPCATCH
	};

	enum class EProductType
	{
		MEDIP,
		DEEPCATCH,
	};


	enum class ELanguageType
	{
		KOR = 0,
		ENG,
	};

	struct ProductFunctionType
	{
		EProductType ProductType;
		bool CanPACSDownload;
		bool CanPACSUpload;

		ProductFunctionType()
		{
			ProductType = EProductType::MEDIP;
			CanPACSDownload = false;
			CanPACSUpload = false;
		}

		ProductFunctionType(
			EProductType productType, 
			bool canPACSDownload,
			bool canPACSUpload)
		{
			ProductType = productType;
			CanPACSDownload = canPACSDownload;
			CanPACSUpload = canPACSUpload;
		}
	};
}

