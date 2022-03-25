//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureStringEnum
//* .File Name       : $Workfile:   SVGigeFeatureStringEnumList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeFeatureStringEnumList.h"
#pragma endregion Includes

SVGigeFeatureStringEnumList::SVGigeFeatureStringEnumList()
{
}

SVGigeFeatureStringEnumList::~SVGigeFeatureStringEnumList()
{
}


SVGigeFeatureStringEnumList::SVGigeFeatureStringEnumList(const SVGigeFeatureStringEnumList& rList)
{
	if (rList.HasTranslation())
	{
		m_stringEnums = rList.m_stringEnums;
	}
}

SVGigeFeatureStringEnumList::SVGigeFeatureStringEnumList(const StringPairVector& rRhs) :
m_stringEnums{rRhs}
{
}

SVGigeFeatureStringEnumList& SVGigeFeatureStringEnumList::operator=(const SVGigeFeatureStringEnumList& rList)
{
	if (this != &rList)
	{
		if (rList.HasTranslation())
		{
			m_stringEnums = rList.m_stringEnums;
		}
	}
	return *this;
}

bool SVGigeFeatureStringEnumList::HasTranslation() const
{
	return m_stringEnums.size() > 0;
}

HRESULT SVGigeFeatureStringEnumList::GetGigeFeatureString(const std::string& rDeviceParam, std::string& rGigeFeature) const
{
	for(const auto& rEntry : m_stringEnums)
	{
		if(rEntry.first == rDeviceParam)
		{
			rGigeFeature = rEntry.second;
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT SVGigeFeatureStringEnumList::GetDeviceParamString(const std::string& rGigeFeature, std::string& rDeviceParam) const
{
	for (const auto& rEntry : m_stringEnums)
	{
		if (rEntry.second == rGigeFeature)
		{
			rDeviceParam = rEntry.first;
			return S_OK;
		}
	}

	return E_FAIL;
}

