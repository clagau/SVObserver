//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStruct
//* .File Name       : $Workfile:   SVGigeCameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:19:56  $
//******************************************************************************

#ifndef SVGIGECAMERASTRUCT_H
#define SVGIGECAMERASTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVGigeCameraStructInterface.h"

/**
@SVObjectName Gige Camera Description

@SVObjectOverview This object is used to hold the Gige Camera data.

@SVObjectOperations This operator presents operations to store, clear, and compare Gige camera data.

*/
struct SVGigeCameraStruct : public SVGigeCameraStructInterface
{
	SVGigeCameraStruct();
	virtual ~SVGigeCameraStruct();

	SVGigeCameraStruct(const SVGigeCameraStruct &SVGigecs);
	const SVGigeCameraStruct& operator= (const SVGigeCameraStruct &SVGigecs);
	
	void SetInfo(const SVGigeCameraStruct& rhs);
	void Clear();
	
	bool operator == (const SVGigeCameraStruct &SVGigecs) const;
	bool operator < (const SVGigeCameraStruct &SVGigecs) const;
	bool operator > (const SVGigeCameraStruct &SVGigecs) const;

	virtual bool HasSerialNumber() const;
	virtual CString GetSerialNumber() const;
	virtual bool HasModelName() const;
	virtual CString GetModelName() const;
	virtual bool HasVendorName() const;
	virtual CString GetVendorName() const;
	virtual bool HasIPAddress() const;
	virtual CString GetIPAddress() const;

	CString strSerialNum;
	CString strModelName;
	CString strVendorName;
	CString strIPAddress;
	
	int iPosition;
	unsigned long m_ulHandle;
};

typedef SVVector< SVGigeCameraStruct > SVGigeCameraStructSet;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraStruct.h_v  $
 * 
 *    Rev 1.1   12 Jun 2013 15:19:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 May 2013 10:22:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the string strIPAddress. Added methods HasIPAddress and GetIPAddress  Changed the "operator methods" to be const and to go off of the IPAddress instead of serial number.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:43:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2010 11:55:12   jspila
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
