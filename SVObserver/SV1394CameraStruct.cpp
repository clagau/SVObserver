//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraStruct
//* .File Name       : $Workfile:   SV1394CameraStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:22  $
//******************************************************************************

#include "Stdafx.h"
#include "SV1394CameraStruct.h"

SV1394CameraStruct::SV1394CameraStruct()
{
	Clear();

	iPosition = 0;
}

SV1394CameraStruct::SV1394CameraStruct(const SV1394CameraStruct &SV1394cs)
{
	*this = SV1394cs;
}

SV1394CameraStruct::~SV1394CameraStruct()
{
}

const SV1394CameraStruct& SV1394CameraStruct::operator = (const SV1394CameraStruct &SV1394cs)
{
	if ( this != &SV1394cs )
	{
		eChangeType = SV1394cs.eChangeType;
		iPosition = SV1394cs.iPosition;
		m_ui64SerialNumber = SV1394cs.m_ui64SerialNumber;
		m_csVendorId = SV1394cs.m_csVendorId;
		strSerialNum = SV1394cs.strSerialNum;
		strModelName = SV1394cs.strModelName;
		strVendorName = SV1394cs.strVendorName;

		m_ulHandle = SV1394cs.m_ulHandle;
	}

	return *this;
}

void SV1394CameraStruct::SetInfo(const SV1394CameraStruct& rhs)
{
	if ( this != &rhs )
	{
		m_ui64SerialNumber = rhs.m_ui64SerialNumber;
		m_csVendorId = rhs.m_csVendorId;
		strSerialNum = rhs.strSerialNum;
		strModelName = rhs.strModelName;
		strVendorName = rhs.strVendorName;

		m_ulHandle = rhs.m_ulHandle;

	}
}

void SV1394CameraStruct::Clear()
{
	m_ui64SerialNumber = 0;
	m_csVendorId.Empty();
	strSerialNum.Empty();
	strModelName.Empty();
	strVendorName.Empty();
	eChangeType = SVNoChange;
	
	m_ulHandle = NULL;
}

bool SV1394CameraStruct::operator == (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum == SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::operator < (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum < SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::operator > (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum > SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::HasSerialNumber() const
{
	return !strSerialNum.IsEmpty();
}

CString SV1394CameraStruct::GetSerialNumber() const
{
	return strSerialNum;
}

bool SV1394CameraStruct::HasModelName() const
{
	return !strModelName.IsEmpty();
}

CString SV1394CameraStruct::GetModelName() const
{
	return strModelName;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Feb 2010 10:07:58   jspila
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
