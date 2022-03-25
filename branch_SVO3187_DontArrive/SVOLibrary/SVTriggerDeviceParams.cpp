//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerDeviceParams
//* .File Name       : $Workfile:   SVTriggerDeviceParams.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVTriggerDeviceParams.h"

SVTriggerDeviceParams::SVTriggerDeviceParams()
: m_Name( _T( "" ) ), m_Channel( -1 )
{
}

SVTriggerDeviceParams::SVTriggerDeviceParams( const SVTriggerDeviceParams& p_rObject )
: m_Name( p_rObject.m_Name ), m_Channel( p_rObject.m_Channel )
{
}

SVTriggerDeviceParams::SVTriggerDeviceParams( LPCTSTR p_szName, int p_Channel )
	: m_Name( (nullptr != p_szName) ? p_szName : std::string() ), m_Channel( p_Channel )
{
}
