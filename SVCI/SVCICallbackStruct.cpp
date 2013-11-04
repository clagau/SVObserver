// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCICallbackStruct
// * .File Name       : $Workfile:   SVCICallbackStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:04  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCICallbackStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVCICallbackStruct::SVCICallbackStruct()
: m_ulDigitizerIndex( -1 )
, m_ulBufferIndex( -1 )
, m_IsTrash( false )
{
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCICallbackStruct::~SVCICallbackStruct()
{
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCICallbackStruct.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 May 2011 09:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 08:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/