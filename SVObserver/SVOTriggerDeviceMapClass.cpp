//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceMapClass
//* .File Name       : $Workfile:   SVOTriggerDeviceMapClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVOTriggerDeviceMapClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOTriggerDeviceMapClass::SVOTriggerDeviceMapClass()
{
}

SVOTriggerDeviceMapClass::~SVOTriggerDeviceMapClass()
{
}

BOOL SVOTriggerDeviceMapClass::GetDeviceList(CStringList &rDeviceList)
{
	BOOL bOk = TRUE;

	POSITION pos;
	CString csName;
	void *pItem = NULL;

	rDeviceList.RemoveAll();

  for( pos = GetStartPosition(); bOk && pos != NULL; GetNextAssoc( pos, csName, pItem ) )
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

