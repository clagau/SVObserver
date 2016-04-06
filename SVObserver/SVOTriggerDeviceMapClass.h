//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceMapClass
//* .File Name       : $Workfile:   SVOTriggerDeviceMapClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:20  $
//******************************************************************************

#pragma once

class SVOTriggerDeviceMapClass : public CMapStringToPtr  
{
public:
	SVOTriggerDeviceMapClass();
	virtual ~SVOTriggerDeviceMapClass();

	virtual BOOL GetDeviceList( CStringList &rDeviceList );
};

