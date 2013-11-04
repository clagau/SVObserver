//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMeasureAnalyzerAdjustment
//* .File Name       : $Workfile:   SVMeasureAnalyzerAdjustment.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:34:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVMeasureAnalyzerAdjustment.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVIPDoc.h"
#include "SVLine.h"
#include "SVLineAnalyzer.h"
#include "SVSVIMStateClass.h"
#include "SVTaskObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVMeasureAnalyzerAdjustmentSheetClass

//
// Saved location of dialog.
//
static CPoint gPtPosition(0,0);

IMPLEMENT_DYNAMIC(SVMeasureAnalyzerAdjustmentSheetClass, CPropertySheet)

SVMeasureAnalyzerAdjustmentSheetClass::SVMeasureAnalyzerAdjustmentSheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheetClass::SVMeasureAnalyzerAdjustmentSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheetClass::~SVMeasureAnalyzerAdjustmentSheetClass()
{
}

BEGIN_MESSAGE_MAP(SVMeasureAnalyzerAdjustmentSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVMeasureAnalyzerAdjustmentSheetClass)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVMeasureAnalyzerAdjustmentSheetClass 


BOOL SVMeasureAnalyzerAdjustmentSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
    //
	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (hWnd != NULL)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}

	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

    // Position this dialog at saved (or initial) location.
    //
    BOOL bResult2 = SetWindowPos(
        NULL,          // Z order not used
        gPtPosition.x,
        gPtPosition.y,
        0,0,           // Width and Height Not Used
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW 
    );
	
	return bResult;
}

void SVMeasureAnalyzerAdjustmentSheetClass::OnDestroy() 
{
	CPropertySheet::OnDestroy();

    //
    // Save the current location of the property sheet.
    //
    CRect rectWindow;
    GetWindowRect(&rectWindow);
    gPtPosition.x = rectWindow.left;
    gPtPosition.y = rectWindow.top;
    	
	// TODO: Add your message handler code here
	
}

void SVMeasureAnalyzerAdjustmentSheetClass::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMeasureAnalyzerAdjustment.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:34:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   09 Jul 2012 14:08:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   19 Jun 2012 13:01:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   03 Sep 2009 10:29:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   09 Apr 2009 14:58:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  657
   SCR Title:  Remove Apply button from the Tool Adjust Dialogs
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the apply button
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   09 Aug 2005 07:26:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   18 Jul 2005 11:50:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added static_cast for setting Threshold Value Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   17 Feb 2005 14:51:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   09 Dec 2003 12:30:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated line analyzer dialogs to store edge control parameters differently in the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   04 Dec 2003 13:08:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   21 Nov 2003 11:55:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with value objects.  Was setting to last set and not to bucket 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Nov 2003 10:24:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed problems with using This Edge
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Nov 2003 08:14:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the edge/threshold parameters with the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   13 Nov 2003 07:37:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems where it was using the edge struct instead of the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Oct 2003 16:23:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   08 Oct 2003 11:06:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using the value objects for the edge parameters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 11:25:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   20 Nov 2002 08:34:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   Changed ValueObject semantics to work with buckets
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   10 May 2001 15:44:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in:
   updateDisplay
   updateDisplay
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   06 Sep 2000 16:31:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   14 Jun 2000 13:41:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  158
   SCR Title:  Profile Tool Lower Threshold Setup problem
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added initEdgeParameters method.
   Revised OnInitDialog to call initEdgeParameters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   29 Feb 2000 14:28:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  106
   SCR Title:  Math Tool - List processing
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Close Button on caption.
   Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   07 Feb 2000 15:06:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  94
   SCR Title:  New Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to correct problem with constructors in
   SVEdgeMarkerAdjustmentPageClass and
   SVThresholdAdjustmentPageClass which were changed for
   the new profile tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   04 Feb 2000 09:36:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  94
   SCR Title:  New Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to reduce redundant code.
   Revised constructor so this class can be used as a base class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:08:02   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Nov 11 1999 08:54:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Add code to 'save' the location of the measurement property sheet.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   09 Nov 1999 14:22:06   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added updateLineThresholds method called by 
   OnSetActive and OnVscroll 
   (SVImageToLineProject needs to be updated first before
   running)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Oct 05 1999 09:52:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Edit controls repaired.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Sep 10 1999 18:09:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     No Change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   30 Aug 1999 19:24:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVRunStatsuClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   27 Aug 1999 13:35:12   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Ported to 3.0.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   Aug 26 1999 18:31:06   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
