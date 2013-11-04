//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
: m_Name( p_szName ), m_Channel( p_Channel )
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerDeviceParams.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:28:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2010 14:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
