// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDeviceManager
// * .File Name       : $Workfile:   SVMatroxDeviceManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:32:54  $
// ******************************************************************************

#pragma once
#ifndef __MATROXDEVICEMANAGER_H__
#define __MATROXDEVICEMANAGER_H__

#include "SVObserverEnums.h"
#include "SVMatroxAppManager.h"
#include "SVMatroxDeviceParamCollection.h"

#include <mil.h>
#include <map>

#include "mildetect.h"
#include <static_check.h>

// if you get an error on this line, you do not have the correct mil.h installed
FILE_SCOPE_STATIC_CHECK( SV_CURRENT_MIL_1394_PATCH >= 22, Need_to_compile_with_1394_patch_22_mil_dot_h_or_greater );


class SVMatroxDeviceManager
{
public:
	SVMatroxDeviceManager();
	~SVMatroxDeviceManager();

	HRESULT Create();
	HRESULT Destroy();
	HRESULT Reset();
	HRESULT ReallocDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT GetAllDevices( SVMatroxDeviceParamCollectionArray& aDevices );
	HRESULT GetDevice( const CString& sDeviceName, SVMatroxDeviceParamCollection& device );

	bool IsValidDigitizerSubsystem();
	int GetMaxNumDigitizers();

	static HRESULT WriteCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulValue );
	static HRESULT ReadCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long& rulValue );
	static HRESULT WriteCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue );
	static HRESULT ReadCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue );

	static long SwitchByteOrder( long& l );
	static unsigned long SwitchByteOrder( unsigned long& l );

protected:

	HRESULT AllocDigitizer( SVMatroxDeviceParamCollection& device );
	HRESULT FreeDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT AddDigitizer( SVMatroxDeviceParamCollection& device );

	HRESULT RemoveDigitizer( const CString& sDigName );


	MIL_ID 	m_MilSystem;
	long m_lNumDigitizers;
	long m_lMaxNumDigitizers;
	bool m_bHubSystem;

	typedef std::map<CString, SVMatroxDeviceParamCollection> DigitizerParamMapType;
	typedef std::map<MIL_ID, long> DigIdToDigNumMapType;	// used by read/write camera register fns
	DigitizerParamMapType m_mapDigitizerParams;
	static DigIdToDigNumMapType m_mapDigitizerIds;	// yuk! don't want to make it static, but it will only be used by read/write camera register fns

	SVMatroxAppManager m_AppManager;	// just declaring this will initialize the Mil App

	static long MFTYPE HandleError(long lHookType, MIL_ID EventId, void MPTYPE* pUserData);
};

#endif	//#ifndef __MATROXDEVICEMANAGER_H__

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxDeviceManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Nov 2005 15:59:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Feb 2005 09:50:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed patch 21 to patch 22 (same header)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Dec 2004 08:31:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added compile-time check for MIL 1394 patch 21 or greater
 * removed duplicate Loki static check code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Oct 2004 09:13:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added hub support (more than 3 digitizers, and change default allocation for hub systems)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Nov 2003 09:51:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem of creating dummy devices when no cams attached
 * connect to SV1394CameraManager to determine order of digitizers
 * fixed block transfer code
 * store VendorId param for each dig
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Nov 2003 16:24:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to allow unplugging and plugging cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Oct 2003 08:27:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   support for bit rate setting (M_SPEED)
 * save serial numbers to registry
 * added block transfer code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Sep 2003 14:59:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 08:58:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVCameraFormatsDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 11:21:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added files to support Matrox acquisition devices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/