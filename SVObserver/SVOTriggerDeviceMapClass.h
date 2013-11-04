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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerDeviceMapClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:14:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/