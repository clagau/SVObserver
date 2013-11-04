//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStruct
//* .File Name       : $Workfile:   SVGigeCameraStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:19:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVGigeCameraStruct::SVGigeCameraStruct()
{
	Clear();
	iPosition = 0;
}

SVGigeCameraStruct::SVGigeCameraStruct(const SVGigeCameraStruct &SVGigecs)
{
	*this = SVGigecs;
}

SVGigeCameraStruct::~SVGigeCameraStruct()
{
}

const SVGigeCameraStruct& SVGigeCameraStruct::operator = (const SVGigeCameraStruct &SVGigecs)
{
	if ( this != &SVGigecs )
	{
		eChangeType = SVGigecs.eChangeType;
		iPosition = SVGigecs.iPosition;

		strSerialNum = SVGigecs.strSerialNum;
		strModelName = SVGigecs.strModelName;
		strVendorName = SVGigecs.strVendorName;
		strIPAddress = SVGigecs.strIPAddress;

		m_ulHandle = SVGigecs.m_ulHandle;
	}

	return *this;
}

void SVGigeCameraStruct::SetInfo(const SVGigeCameraStruct& rhs)
{
	if ( this != &rhs )
	{
		strSerialNum = rhs.strSerialNum;
		strModelName = rhs.strModelName;
		strVendorName = rhs.strVendorName;
		strIPAddress = rhs.strIPAddress;
		m_ulHandle = rhs.m_ulHandle;
	}
}

void SVGigeCameraStruct::Clear()
{
	strSerialNum.Empty();
	strModelName.Empty();
	strVendorName.Empty();
	strIPAddress.Empty();
	eChangeType = SVNoChange;
	
	m_ulHandle = NULL;
}

bool SVGigeCameraStruct::operator == (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress == SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::operator < (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress < SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::operator > (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress > SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::HasSerialNumber() const
{
	return !strSerialNum.IsEmpty();
}

CString SVGigeCameraStruct::GetSerialNumber() const
{
	return strSerialNum;
}

bool SVGigeCameraStruct::HasModelName() const
{
	return !strModelName.IsEmpty();
}

CString SVGigeCameraStruct::GetModelName() const
{
	return strModelName;
}

bool SVGigeCameraStruct::HasVendorName() const
{
	return !strVendorName.IsEmpty();
}

CString SVGigeCameraStruct::GetVendorName() const
{
	return strVendorName;
}

bool SVGigeCameraStruct::HasIPAddress() const
{
	return !strIPAddress.IsEmpty();
}

CString SVGigeCameraStruct::GetIPAddress() const
{
	return strIPAddress;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraStruct.cpp_v  $
 * 
 *    Rev 1.1   12 Jun 2013 15:19:54   bWalter
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
 *    Rev 1.0   23 Apr 2013 10:43:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2010 11:55:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to fix issues with changing cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:47:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
