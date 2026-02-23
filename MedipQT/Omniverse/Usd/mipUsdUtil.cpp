#include "stdafx.h"
#include "mipUsdUtil.h"

bool mipUsdUtil::CopyProperties(pxr::UsdPrim* pDst, const pxr::UsdPrim& src)
{
	std::vector<UsdAttribute> attrs = src.GetAttributes();
	for (const UsdAttribute& attrSrc : attrs)
	{
		pxr::TfToken name = attrSrc.GetName();
		pxr::SdfValueTypeName typeName = attrSrc.GetTypeName();
		pxr::SdfVariability var = attrSrc.GetVariability();

		pxr::UsdAttribute attrDst = pDst->CreateAttribute(name, typeName, var);
		//std::cout << "attr name : " << name.data() << ", type : " << typeName
		//	<< ", var : " << var << std::endl;

		pxr::VtValue v;
		if (attrSrc.Get(&v))
		{
			attrDst.Set(v);
		}
	}

	std::vector<pxr::UsdRelationship> relationShips = src.GetRelationships();
	for (const pxr::UsdRelationship& relSrc : relationShips)
	{
		pxr::TfToken name = relSrc.GetName();
		bool isCustom = relSrc.IsCustom();

		pxr::UsdRelationship relDest = pDst->CreateRelationship(name, isCustom);
		//std::cout << "relatation ship name : " << name.data() << ", custom : " << isCustom;

		pxr::SdfPathVector targets;
		if (relSrc.GetTargets(&targets))
		{
			relDest.SetTargets(targets);
		}
	}

	return true;
}

bool mipUsdUtil::IsCompatiblePrimName(const std::wstring& wname)
{
	bool isCompatibleName = true;
	if (wname.empty())
	{
		return false;
	}

	if (hasNoneAsciiChar(wname))
	{
		return false;
	}

	if (hasSpecialChar(wname))
	{
		return false;
	}

	if (isStartWithNumber(wname))
	{
		return false;
	}

	return true;
}

bool mipUsdUtil::hasNoneAsciiChar(const std::wstring& wstr)
{
	auto it = std::find_if(wstr.begin(), wstr.end(), [](wchar_t c) {
		return iswascii(c) == false;
		});
	return it != wstr.end();
}

bool mipUsdUtil::hasSpecialChar(const std::wstring& wstr)
{
	auto it = std::find_if(wstr.begin(), wstr.end(), [](wchar_t c) {
		return (iswalnum(c) == false) && isNotCompatibleSpecialChar(c);
		});
	return it != wstr.end();
}

bool mipUsdUtil::isNotCompatibleSpecialChar(wchar_t c)
{
	std::wstring wcompatibleCharList = L"_";
	auto it = std::find_if(wcompatibleCharList.begin(), wcompatibleCharList.end(), [c](wchar_t rhs) {
		return rhs != c;
		});
	return it != wcompatibleCharList.end();

	return false;
}

bool mipUsdUtil::isStartWithNumber(const std::wstring& wstr)
{
	return iswdigit(wstr[0]);
}


