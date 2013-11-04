//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestAcquisitionSubsystem.cpp
//* .File Name       : $Workfile:   SVTestGigeAcquisitionSubsystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:13:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionClass.h"

SVTestGigeAcquisitionSubsystem::SVTestGigeAcquisitionSubsystem()
: SVTestAcquisitionSubsystem()
{
}

SVTestGigeAcquisitionSubsystem::~SVTestGigeAcquisitionSubsystem()
{
}

SVTestAcquisitionClass* SVTestGigeAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long l_ulHandle = 0;

	m_svDigitizers.GetHandle( &l_ulHandle, selectedCamera );
		
	return new SVTestGigeAcquisitionClass( *this, l_ulHandle );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestGigeAcquisitionSubsystem.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:13:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 18:17:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/