//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOChildFrm
//* .File Name       : $Workfile:   SVIOChildFrm.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:08:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVDiscreteInputsView.h"
#include "SVDiscreteOutputsView.h"

#include "SVIOChildFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//
// Precreate
//	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
//		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;
//
//
BOOL SVIOSplitterFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~WS_VISIBLE;         // Create window but don't show it (yet).
	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// SVIOSplitterFrame

// Create a splitter window which splits an output text view and an input view
//                           |
//    TEXT VIEW (CTextView)  | INPUT VIEW (CInputView)
//                           |

IMPLEMENT_DYNCREATE( SVIOSplitterFrame, CMDIChildWnd )

SVIOSplitterFrame::SVIOSplitterFrame()
{
}

SVIOSplitterFrame::~SVIOSplitterFrame()
{
}


BOOL SVIOSplitterFrame::OnCreateClient( LPCREATESTRUCT, CCreateContext* pContext )
{
	// create a splitter with 2 rows, 1 column
	if( ! m_wndSplitter.CreateStatic( this, 2, 1 ) )
	{
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if( ! m_wndSplitter.CreateView( 0, 
									0,
									RUNTIME_CLASS( SVDiscreteInputsView ), 
									CSize( 640, 230 ), 
									pContext ) )
	{
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if( ! m_wndSplitter.CreateView( 1, 
									0,
									RUNTIME_CLASS( SVDiscreteOutputsView ), 
									CSize( 640, 230 ), 
									pContext ) )
	{
		return FALSE;
	}

	// activate the input view
	SetActiveView( ( CView* ) m_wndSplitter.GetPane( 0, 0 ) );

	return TRUE;
}

BEGIN_MESSAGE_MAP( SVIOSplitterFrame, CMDIChildWnd )
	//{{AFX_MSG_MAP(SVIOSplitterFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOChildFrm.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:08:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   14 Nov 2008 14:14:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  615
   SCR Title:  Integrate PLC Classes into SVObserver Outputs
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed IOViews to Tabbed windows
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 10:31:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   19 Nov 2002 14:36:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     OnCreateClient
               Changed sizes of splitter wnds
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:51:20   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   Oct 26 1999 15:17:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Add new version headers.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/