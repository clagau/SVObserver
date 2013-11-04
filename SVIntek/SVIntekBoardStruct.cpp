// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBoardStruct
// * .File Name       : $Workfile:   SVIntekBoardStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:28  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekBoardStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVIntekBoardStruct::SVIntekBoardStruct()
{
	m_csName.Empty();
	m_csFireStackName.Empty();

	m_ulCardId.AsUINT32 = 0;

	m_hThread = NULL;

	m_bFirstBusResetStarted = false;
	m_bFirstBusResetComplete = false;

	m_pFireBus = NULL;
}

SVIntekBoardStruct::~SVIntekBoardStruct()
{
	if ( m_hThread != NULL )
	{
		::CloseHandle( m_hThread );

		m_hThread = NULL;
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekBoardStruct.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:50:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Mar 2006 08:18:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  552
 * SCR Title:  Fix problem with Intek driver when attaching multiple cameras to one board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new variables to board structure to monitor initial bus reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 12:29:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/