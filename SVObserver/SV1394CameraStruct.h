//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraStruct
//* .File Name       : $Workfile:   SV1394CameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:30  $
//******************************************************************************

#ifndef SV1394CAMERASTRUCT_H
#define SV1394CAMERASTRUCT_H

#include "SVCameraStructInterface.h"
#include "SVContainerLibrary/SVVector.h"

/**
@SVObjectName 1394 Camera Description

@SVObjectOverview This object is used to hold the 1394 Camera data.

@SVObjectOperations This operator presents operations to store, clear, and compair 1394 camera data.

*/
struct SV1394CameraStruct : public SVCameraStructInterface
{
	SV1394CameraStruct();
	virtual ~SV1394CameraStruct();
	SV1394CameraStruct(const SV1394CameraStruct &SV1394cs);
	const SV1394CameraStruct& operator= (const SV1394CameraStruct &SV1394cs);
	void SetInfo(const SV1394CameraStruct& rhs);
	void Clear();
	bool operator == (const SV1394CameraStruct &SV1394cs);
	bool operator < (const SV1394CameraStruct &SV1394cs);
	bool operator > (const SV1394CameraStruct &SV1394cs);

	virtual bool HasSerialNumber() const;
	virtual CString GetSerialNumber() const;
	virtual bool HasModelName() const;
	virtual CString GetModelName() const;

	unsigned __int64 m_ui64SerialNumber;
	CString m_csVendorId;
	CString strSerialNum;
	CString strModelName;
	CString strVendorName;
	
	int iPosition;
	unsigned long m_ulHandle;
};

typedef SVVector< SV1394CameraStruct > SV1394CameraStructSet;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:40:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Feb 2010 10:07:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated container used to hold the 1394 camera data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 10:56:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (moved from SV1394CameraManager)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

