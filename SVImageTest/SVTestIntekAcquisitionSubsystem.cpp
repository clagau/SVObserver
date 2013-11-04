// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekAcquisitionSubsystem.cpp
// * .File Name       : $Workfile:   SVTestIntekAcquisitionSubsystem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTestIntekAcquisitionSubsystem.h"
#include "SVTestIntekAcquisitionClass.h"

SVTestIntekAcquisitionSubsystem::SVTestIntekAcquisitionSubsystem()
: SVTestAcquisitionSubsystem()
{
}

SVTestIntekAcquisitionSubsystem::~SVTestIntekAcquisitionSubsystem()
{
}

SVTestAcquisitionClass* SVTestIntekAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long l_ulHandle = 0;

	m_svDigitizers.GetHandle( &l_ulHandle, selectedCamera );
		
	return new SVTestIntekAcquisitionClass( *this, l_ulHandle );
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestIntekAcquisitionSubsystem.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2008 08:47:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Adding new files to the project to implement multi-camera acquisition functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/