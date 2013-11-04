//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorageResult
//* .File Name       : $Workfile:   SVStorageResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVStorageResult.h"

SVStorageResult::SVStorageResult()
: m_Storage(), m_Status( S_OK ), m_TriggerCount( 0 )
{
}

SVStorageResult::SVStorageResult( const SVStorage& p_rStorage, HRESULT p_Status, unsigned long p_TriggerCount )
: m_Storage( p_rStorage ), m_Status( p_Status ), m_TriggerCount( p_TriggerCount )
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStorageResult.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:14:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
