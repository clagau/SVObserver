//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
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

HRESULT SVGigeFeatureStringEnumList::GetGigeFeatureString(const SVString& rDeviceParam, SVString& rGigeFeature) const
{
	HRESULT hr = S_OK;

	typedef SVBidirectionalMap<SVString, SVString>::type::index_const_iterator<from>::type const_iterator;
	const_iterator it = m_stringEnums.get<from>().find(rDeviceParam);
	if (it != m_stringEnums.get<from>().end())
	{
		rGigeFeature = it->second;
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVGigeFeatureStringEnumList::GetDeviceParamString(const SVString& rGigeFeature, SVString& rDeviceParam) const
{
	HRESULT hr = S_OK;

	typedef SVBidirectionalMap<SVString, SVString>::type::index_const_iterator<to>::type const_iterator;
	const_iterator it = m_stringEnums.get<to>().find(rGigeFeature);
	if (it != m_stringEnums.get<to>().end())
	{
		rDeviceParam = it->first;
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

