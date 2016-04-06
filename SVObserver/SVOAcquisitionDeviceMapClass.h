//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOAcquisitionDeviceMapClass
//* .File Name       : $Workfile:   SVOAcquisitionDeviceMapClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:38:00  $
//******************************************************************************

#pragma once

class SVOAcquisitionDeviceMapClass : public CMapStringToPtr  
{
public:
	SVOAcquisitionDeviceMapClass();
	virtual ~SVOAcquisitionDeviceMapClass();

	virtual BOOL GetDeviceList( CStringList &rDeviceList );
};
