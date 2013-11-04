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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeFeatureStringEnumList.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:06:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
