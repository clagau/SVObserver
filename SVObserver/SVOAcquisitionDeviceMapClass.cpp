//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOAcquisitionDeviceMapClass
//* .File Name       : $Workfile:   SVOAcquisitionDeviceMapClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:37:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVOAcquisitionDeviceMapClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOAcquisitionDeviceMapClass::SVOAcquisitionDeviceMapClass()
{
}

SVOAcquisitionDeviceMapClass::~SVOAcquisitionDeviceMapClass()
{
}

BOOL SVOAcquisitionDeviceMapClass::GetDeviceList(CStringList &rDeviceList)
{
	BOOL bOk = TRUE;

	POSITION pos;
	CString csName;
	void *pItem = nullptr;

	rDeviceList.RemoveAll();

  for( pos = GetStartPosition(); bOk && nullptr != pos; GetNextAssoc( pos, csName, pItem ) )
	{
		bOk = ! csName.IsEmpty();

		if ( bOk )
		{
			rDeviceList.AddTail( csName );
		}

		csName.Empty();
	}

	if ( ! bOk )
	{
		rDeviceList.RemoveAll();
	}
	
	return bOk;
}

