// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParamCollection
// * .File Name       : $Workfile:   SVDeviceParamCollection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   28 Feb 2014 09:31:46  $
// ******************************************************************************

// SVDeviceParamCollection.cpp: definition for the SVDeviceParamCollection struct.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVDeviceParamCollection.h"
#include "SVDeviceParams.h"
#include "SVCustomDeviceParam.h"
//#include "SVObserverEnums.h"
#include "SVOMFCLibraryGlobals.h"
#include <algorithm>

SVDeviceParamIndexer::SVDeviceParamIndexer(const SVDeviceParamMap& map) : m_mapIndexed(map)
{
	SVDeviceParamMap::const_iterator iter;
	// fill vector with unsorted enums
	for (iter = map.begin(); iter != map.end(); ++iter)
	{
		push_back(iter->first);
	}
	// now sort
	std::sort(begin(), end(), keycomp(m_mapIndexed));
}

bool SVDeviceParamIndexer::keycomp::operator () (const SVDeviceParamEnum& a, const SVDeviceParamEnum& b) const
{
	SVDeviceParamMap::const_iterator iterA = m_mapIndexed.find(a);
	SVDeviceParamMap::const_iterator iterB = m_mapIndexed.find(b);
	const SVDeviceParam* pA = iterA->second;
	const SVDeviceParam* pB = iterB->second;
	if (pB==NULL)
		return true;
	else if (pA==NULL)
		return false;
	else
		return iterA->second->GetOrder() < iterB->second->GetOrder();
}

SVDeviceParamCollection::SVDeviceParamCollection( const SVDeviceParamCollection& rhs )
{
		mapParameters = rhs.mapParameters;
}

SVDeviceParamCollection& SVDeviceParamCollection::operator = ( const SVDeviceParamCollection& rhs )
{
	if ( this != &rhs )
	{
		mapParameters = rhs.mapParameters;
	}
	return *this;
}

HRESULT SVDeviceParamCollection::Clear()
{
	for( SVDeviceParamMap::iterator it = mapParameters.begin(); it != mapParameters.end() ; ++it )
	{
		it->second.Clear();
	}
	mapParameters.clear();
	return S_OK;
}

HRESULT SVDeviceParamCollection::CreateParameter( SVDeviceParamEnum e, const VARIANT& rv )
{
	if (e >= DeviceParamGigeCustom1 && e <= DeviceParamGigeCustom30)
	{
		SVDeviceParam* pParam1 = SVCustomDeviceParam::Create(e, rv);
		if (pParam1)
		{
			SetParameter(e, pParam1);
			delete pParam1;
			SVDeviceParam* pParam2 = GetParameter(e);
			if (pParam2)
			{
				return pParam2->SetValue(rv);
			}
		}
	}
	else
	{
		SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVDeviceParam* pParam = GetParameter( e );
		if ( pParam )
		{
			return pParam->SetValue( rv );
		}
	}
	return SV_FALSE;
}

