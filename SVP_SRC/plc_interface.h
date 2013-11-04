// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CPlc_interfaceApp
// * .File Name       : $Workfile:   plc_interface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:33:42  $
// ******************************************************************************

#if !defined(AFX_PLC_INTERFACE_H__308AD5D5_C388_11D3_A870_00106F000B22__INCLUDED_)
#define AFX_PLC_INTERFACE_H__308AD5D5_C388_11D3_A870_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "PLCBaseClass.h"
#include "SLC505APIClass.h"
#include "SLC504APIClass.h"
#include "SVPLC5APIClass.h"
#include "SVMicroLogixAPIClass.h"
#include "SVS7PBFMS.h"
#include "SetupDialogSheet.h"
#include "SVASABtcpMicroLogixAPI.h"
#include "SVASABtcpSLC500API.h"

#if !defined (AFX_SVLANGUAGEMANAGER)

typedef enum { NoLanguage, USEnglish, French, UKEnglish, German, Italian, Portuguese } SVLanguage;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlc_interfaceApp
// See plc_interface.cpp for the implementation of this class
//

#define IDC_FORCE_NEXT              10000
#define IDC_PLACEBUTTON_FORCENEXT   10001
#define IDC_PLACEBUTTON_FORCEBACK   10002
#define IDC_FORCE_BACK              10003

class CPlc_interfaceApp : public CWinApp
{
public:

	CPlc_interfaceApp();
	~CPlc_interfaceApp();


   long           SetLanguage (SVLanguage aLanguage);

   BOOL           InitInstance ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlc_interfaceApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPlc_interfaceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



   SVLanguage     msvCurrentLanguage;

   HINSTANCE      msvhCurrentResourceDll;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLC_INTERFACE_H__308AD5D5_C388_11D3_A870_00106F000B22__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\plc_interface.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:33:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 May 2010 14:24:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:55:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Nov 2001 15:05:52   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added includes and definitions for supporting MicroLogix and PLC5 series PLC's.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Sep 2000 11:53:48   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Changes for Resource/Language management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:14   mike
 * Initial revision.
*/