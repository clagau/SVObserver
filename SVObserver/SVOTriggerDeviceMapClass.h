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

#if !defined(AFX_SVOTRIGGERDEVICEMAPCLASS_H__23EF5A0E_3E9C_4089_985E_B7DA2C99B9B0__INCLUDED_)
#define AFX_SVOTRIGGERDEVICEMAPCLASS_H__23EF5A0E_3E9C_4089_985E_B7DA2C99B9B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVOTriggerDeviceMapClass : public CMapStringToPtr  
{
public:
	SVOTriggerDeviceMapClass();
	virtual ~SVOTriggerDeviceMapClass();

	virtual BOOL GetDeviceList( CStringList &rDeviceList );
};

#endif // !defined(AFX_SVOTRIGGERDEVICEMAPCLASS_H__23EF5A0E_3E9C_4089_985E_B7DA2C99B9B0__INCLUDED_)

