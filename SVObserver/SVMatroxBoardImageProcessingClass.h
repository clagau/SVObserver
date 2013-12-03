//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBoardImageProcessingClass
//* .File Name       : $Workfile:   SVMatroxBoardImageProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:31:02  $
//******************************************************************************

#if !defined(AFX_SVMatroxBoardImageProcessingClass_H)
#define AFX_SVMatroxBoardImageProcessingClass_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVBoardImageProcessingClass.h"
#include "SVMatroxDeviceManager.h"

class SVMatroxBoardImageProcessingClass : public SVBoardImageProcessingClass 
{
public:
	SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                   SVDigitizerProcessingClass *p_pDigitizerSubsystem );
	SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                   SVDigitizerProcessingClass *p_pDigitizerSubsystem,
															       LPCSTR ObjectName );
	virtual ~SVMatroxBoardImageProcessingClass();

	virtual HRESULT Create();
	virtual HRESULT Destroy();
	virtual HRESULT DisconnectDevices();
	virtual HRESULT ConnectDevices();

private:
	HRESULT LocalCreate();
	HRESULT LocalDestroy();

	/*static*/SVMatroxDeviceManager m_DeviceManager;
	// does this need to be static?? if so, we need to work out the problem of SVMatroxAppManager initialization / App init.
};

#endif // !defined(AFX_SVMatroxDisplayImageProcessingClass_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxBoardImageProcessingClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:31:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Oct 2004 09:11:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   get max digitizers from MatroxDeviceManager, not hard coded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Oct 2003 08:25:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ConnectDevices / DisconnectDevices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 10:00:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Matrox 1394 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:17:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Feb 2003 10:04:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/