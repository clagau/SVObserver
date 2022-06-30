// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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

bool SVDeviceParamIndexer::keycomp::operator () (const SVDeviceParamEnum& rLhs, const SVDeviceParamEnum& rRhs) const
{
	SVDeviceParamMap::const_iterator iterLhs = m_mapIndexed.find(rLhs);
	SVDeviceParamMap::const_iterator iterRhs = m_mapIndexed.find(rRhs);
	const SVDeviceParam* pLhs = iterLhs->second;
	const SVDeviceParam* pRhs = iterRhs->second;
	if (nullptr == pRhs)
	{
		return true;
	}
	else if (nullptr == pLhs)
	{
		return false;
	}
	return iterLhs->second->GetOrder() < iterRhs->second->GetOrder();
}

SVDeviceParamCollection::SVDeviceParamCollection( const SVDeviceParamCollection& rhs ) :
	mapParameters {rhs.mapParameters}
{
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
		SetParameter( e, std::unique_ptr<SVDeviceParam>(SVDeviceParam::Create(e)).get());
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
			l_bRising = pParam->strValue.compare(_T("rising")) == 0;
		}

		if ( ParameterExists( DeviceParamIOStrobeInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = Parameter( DeviceParamIOStrobeInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionStrobeEdge
		SetParameter( DeviceParamAcquisitionStrobeEdge, std::unique_ptr<SVDeviceParam>(SVDeviceParam::Create(DeviceParamAcquisitionStrobeEdge)).get());
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
			l_bRising = pParam->strValue.compare( _T( "rising" )) == 0;
		}

		if ( ParameterExists( DeviceParamIOTriggerInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = Parameter( DeviceParamIOTriggerInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionTriggerEdge
		SetParameter( DeviceParamAcquisitionTriggerEdge, std::unique_ptr<SVDeviceParam>(SVDeviceParam::Create(DeviceParamAcquisitionTriggerEdge)).get());
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

