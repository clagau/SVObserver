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
#include "stdafx.h"
#include "SVGigeFeatureStringEnumList.h"

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

HRESULT SVGigeFeatureStringEnumList::GetGigeFeatureString(const SVDeviceParamString& in, SVGigeFeatureString& out) const
{
	HRESULT hr = S_OK;

	typedef GigeFeatureStringEnumList::index_const_iterator<from>::type const_iterator;
	const_iterator it = m_stringEnums.get<from>().find(in);
	if (it != m_stringEnums.get<from>().end())
	{
		out = it->second;
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVGigeFeatureStringEnumList::GetDeviceParamString(const SVGigeFeatureString& in, SVDeviceParamString& out) const
{
	HRESULT hr = S_OK;

	typedef GigeFeatureStringEnumList::index_const_iterator<to>::type const_iterator;
	const_iterator it = m_stringEnums.get<to>().find(in);
	if (it != m_stringEnums.get<to>().end())
	{
		out = it->first;
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

