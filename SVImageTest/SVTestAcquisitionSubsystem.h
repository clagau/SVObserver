// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestAcquisitionSubsystem.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:42  $
// ******************************************************************************

#ifndef INCL_SVTESTACQUISITIONSUBSYSTEM_H
#define INCL_SVTESTACQUISITIONSUBSYSTEM_H

#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"

class SVTestAcquisitionClass;

class SVTestAcquisitionSubsystem
{

public:
	SVDigitizerLoadLibraryClass m_svDigitizers;

public:
	SVTestAcquisitionSubsystem();
	virtual ~SVTestAcquisitionSubsystem();

	virtual bool Create(const CString& sDLLname);
	virtual void Destroy();

	virtual SVTestAcquisitionClass* GetAcquisitionDevice( long selectedCamera );

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestAcquisitionSubsystem.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:10:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 11:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:53:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
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
