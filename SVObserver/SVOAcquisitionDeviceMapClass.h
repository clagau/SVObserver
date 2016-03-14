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

#if !defined(AFX_SVOACQUISITIONDEVICEMAPCLASS_H__D5EC93E1_5706_474C_A442_50F04866C708__INCLUDED_)
#define AFX_SVOACQUISITIONDEVICEMAPCLASS_H__D5EC93E1_5706_474C_A442_50F04866C708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVOAcquisitionDeviceMapClass : public CMapStringToPtr  
{
public:
	SVOAcquisitionDeviceMapClass();
	virtual ~SVOAcquisitionDeviceMapClass();

	virtual BOOL GetDeviceList( CStringList &rDeviceList );
};

#endif // !defined(AFX_SVOACQUISITIONDEVICEMAPCLASS_H__D5EC93E1_5706_474C_A442_50F04866C708__INCLUDED_)

