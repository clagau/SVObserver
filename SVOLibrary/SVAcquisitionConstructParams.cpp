//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionConstructParams
//* .File Name       : $Workfile:   SVAcquisitionConstructParams.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVAcquisitionConstructParams.h"

SVAcquisitionConstructParams::SVAcquisitionConstructParams()
: m_DigitizerName( _T( "" ) )
, m_Channel( -1 )
, m_DigNumber( -1 )
, m_BandSize( -1 )
, m_BandMaxSize( -1 )
, m_LUTAndLRSet( false )
, m_LUTInfo()
, m_LRBandSize( -1 )
{
}

SVAcquisitionConstructParams::SVAcquisitionConstructParams( const SVAcquisitionConstructParams& p_rObject )
: m_DigitizerName( p_rObject.m_DigitizerName )
, m_Channel( p_rObject.m_Channel )
, m_DigNumber( p_rObject.m_DigNumber )
, m_BandSize( p_rObject.m_BandSize )
, m_BandMaxSize( p_rObject.m_BandMaxSize )
, m_LUTAndLRSet( p_rObject.m_LUTAndLRSet )
, m_LUTInfo( p_rObject.m_LUTInfo )
, m_LRBandSize( p_rObject.m_LRBandSize )
{
}

SVAcquisitionConstructParams::SVAcquisitionConstructParams( LPCTSTR p_szDigitizerName,
	int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize,
	const SVLutInfo& p_rLUTInfo, int p_LRBandSize )
: m_DigitizerName( p_szDigitizerName )
, m_Channel( p_Channel )
, m_DigNumber( p_DigNumber )
, m_BandSize( p_BandSize )
, m_BandMaxSize( p_BandMaxSize )
, m_LUTAndLRSet( true )
, m_LUTInfo( p_rLUTInfo )
, m_LRBandSize( p_LRBandSize )
{
}

SVAcquisitionConstructParams::SVAcquisitionConstructParams( LPCTSTR p_szDigitizerName,
	int p_Channel, int p_DigNumber, int p_BandSize, int p_BandMaxSize )
: m_DigitizerName( p_szDigitizerName )
, m_Channel( p_Channel )
, m_DigNumber( p_DigNumber )
, m_BandSize( p_BandSize )
, m_BandMaxSize( p_BandMaxSize )
, m_LUTAndLRSet( false )
, m_LUTInfo()
, m_LRBandSize( -1 )
{
}

SVAcquisitionConstructParams::~SVAcquisitionConstructParams()
{
	m_DigitizerName.clear();
	m_LUTInfo.Cleanup();
}

SVAcquisitionConstructParams& SVAcquisitionConstructParams::operator=( const SVAcquisitionConstructParams& p_rObject )
{
	if( this != &p_rObject )
	{
		m_DigitizerName = p_rObject.m_DigitizerName;
		m_Channel = p_rObject.m_Channel;
		m_DigNumber = p_rObject.m_DigNumber;
		m_BandSize = p_rObject.m_BandSize;
		m_BandMaxSize = p_rObject.m_BandMaxSize;
		m_LUTAndLRSet = p_rObject.m_LUTAndLRSet;
		m_LUTInfo = p_rObject.m_LUTInfo;
		m_LRBandSize = p_rObject.m_LRBandSize;
	}

	return *this;
}
