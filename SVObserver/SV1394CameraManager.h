//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManager
//* .File Name       : $Workfile:   SV1394CameraManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:00  $
//******************************************************************************

#ifndef SV1394CAMERAMANAGER_H
#define SV1394CAMERAMANAGER_H

#include <map>
#include "SV1394CameraStruct.h"

class SV1394CameraManager  
{
public:
	static SV1394CameraManager& Instance();

	HRESULT Refresh();

	const SV1394CameraStructSet& GetCameraOrder() const;

	HRESULT UpdateCameraOrder( const SV1394CameraStructSet& rSVCameraList, bool ignoreHandles=false );
	HRESULT UpdateConnectedCameras( const SV1394CameraStructSet& rSVCameraList );

protected:
	typedef std::map< CString, SV1394CameraStruct > SVConnectedCameraMap;

	SV1394CameraManager();

	// For a singleton object, this constructor is here to warn of a invalid copy construction.  Do not implement this constructor.
	SV1394CameraManager( const SV1394CameraManager& p_rObject );

	// For a singleton object, this operator is here to warn of a invalid assignement operator.  Do not implement this operator.
	const SV1394CameraManager& operator=( const SV1394CameraManager& p_rObject );

	void GetCameraOrder( SV1394CameraStructSet& rSVCameraList ) const;
	void GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const;

	HRESULT OrderCameras( SVConnectedCameraMap& rSVCameraMap );

	SV1394CameraStructSet m_OrderedCamaras;

};

#define TheSV1394CameraManager SV1394CameraManager::Instance()

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Jan 2013 14:45:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter to ignore handles for use in the cancel button of the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Mar 2010 11:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Jun 2009 11:00:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to move SV1394CameraStruct to it's own module(s)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 11:48:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Nov 2005 15:34:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Oct 2004 09:09:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added functionality to work correctly with different number of max cameras (for hub systems)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jan 2004 09:57:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make the model and vendor name available in the camera manager dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Nov 2003 09:36:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
