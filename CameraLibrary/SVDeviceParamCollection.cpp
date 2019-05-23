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
//Moved to precompiled header: #include <algorithm>
#include "SVDeviceParamCollection.h"
#include "SVDeviceParams.h"
#include "SVBoolValueDeviceParam.h"
#include "SVStringValueDeviceParam.h"
#include "SVCustomDeviceParam.h"

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
	if (nullptr == pB)
		return true;
	else if (nullptr == pA)
		return false;
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
	return E_FAIL;
}

HRESULT SVDeviceParamCollection::SetParameterDefaults()
{
	// set default values for newly added parameters that wouldn't be in old configs
	// and for parameters that must always exist

	// IO Strobe Edge
	// The normal camera implementation has the strobe signal active high.
	// We want the normal signal after the gate inversion to be low.
	// So by default we need to provide an inversion. On cameras where the polarity is selectable,
	// no inversion is necessary - just default to active low.

	Clear();

	if ( !ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
	{
		bool l_bRising = true;

		if ( ParameterExists( DeviceParamGigeStrobeEdge ) )
		{
			const SVStringValueDeviceParam* pParam = Parameter( DeviceParamGigeStrobeEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( ParameterExists( DeviceParamIOStrobeInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = Parameter( DeviceParamIOStrobeInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionStrobeEdge
		SetParameter( DeviceParamAcquisitionStrobeEdge, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamAcquisitionStrobeEdge )) );
		SVBoolValueDeviceParam* pEdge = GetParameter( DeviceParamAcquisitionStrobeEdge ).DerivedValue(pEdge);

		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.m_Description = _T("Rising Edge");
		l_Option.m_Value = true;
		pEdge->info.options.push_back( l_Option );

		l_Option.m_Description = _T("Falling Edge");
		l_Option.m_Value = false;
		pEdge->info.options.push_back( l_Option );

		pEdge->bValue = l_bRising; // default falling edge
		pEdge->SetDetailLevel( 99 );
	}

	// IO Trigger Edge
	if ( !ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		bool l_bRising = true;

		if ( ParameterExists( DeviceParamGigeTriggerEdge ) )
		{
			const SVStringValueDeviceParam* pParam = Parameter( DeviceParamGigeTriggerEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( ParameterExists( DeviceParamIOTriggerInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = Parameter( DeviceParamIOTriggerInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionTriggerEdge
		SetParameter( DeviceParamAcquisitionTriggerEdge, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamAcquisitionTriggerEdge )) );
		SVBoolValueDeviceParam* pEdge = GetParameter( DeviceParamAcquisitionTriggerEdge ).DerivedValue(pEdge);

		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.m_Description = _T("Rising Edge");
		l_Option.m_Value = true;
		pEdge->info.options.push_back( l_Option );

		l_Option.m_Description = _T("Falling Edge");
		l_Option.m_Value = false;
		pEdge->info.options.push_back( l_Option );

		pEdge->bValue = l_bRising; // default falling edge
		pEdge->SetDetailLevel( 99 );
	}

	return S_OK;
}

