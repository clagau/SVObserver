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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOAcquisitionDeviceMapClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:38:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:24:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:33:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/