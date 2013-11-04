//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManager
//* .File Name       : $Workfile:   SVGigeCameraManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   05 Jul 2013 09:11:02  $
//******************************************************************************

#ifndef SVGIGECAMERAMANAGER_H
#define SVGIGECAMERAMANAGER_H

#include <map>
#include "SVGigeCameraStruct.h"

class SVGigeCameraManager  
{
public:
	static SVGigeCameraManager& Instance();

	HRESULT Refresh();

	const SVGigeCameraStructSet& GetCameraOrder() const;

	const SVGigeCameraStructSet& GetOriginalCameraIniOrder() const;

	HRESULT UpdateCameraOrder( const SVGigeCameraStructSet& rSVCameraList, bool ignoreHandles=false );
	HRESULT UpdateConnectedCameras( const SVGigeCameraStructSet& rSVCameraList );

	void UpdateCameraIniList();

protected:
	typedef std::map< CString, SVGigeCameraStruct > SVConnectedCameraMap;

	SVGigeCameraManager();

	// For a singleton object, this constructor is here to warn of a invalid copy construction.  Do not implement this constructor.
	SVGigeCameraManager( const SVGigeCameraManager& p_rObject );

	// For a singleton object, this operator is here to warn of a invalid assignement operator.  Do not implement this operator.
	const SVGigeCameraManager& operator=( const SVGigeCameraManager& p_rObject );

	void GetCameraOrder( SVGigeCameraStructSet& rSVCameraList ) const;
	void GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const;

	HRESULT OrderCameras( SVConnectedCameraMap& rSVCameraMap );

	SVGigeCameraStructSet m_OrderedCamaras;

	//Hold the orginal camera order from INI
	SVGigeCameraStructSet m_iniCameras;

};

#define TheSVGigeCameraManager SVGigeCameraManager::Instance()

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraManager.h_v  $
 * 
 *    Rev 1.1   05 Jul 2013 09:11:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Jun 2013 12:17:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods GetOriginalCameraIniOrder & UpdateCameraIniList, new member variable m_iniCameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:42:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Jan 2013 14:45:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter to ignore handles for use in the cancel button of the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2010 11:55:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to fix issues with changing cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
