//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageViewScroll
//* .File Name       : $Workfile:   SVImageViewScroll.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:56:30  $
//******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVImageViewScroll.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVGlobal.h"
#include "SVImageView.h"
#include "SVSVIMStateClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVIPChildFrm.h"

IMPLEMENT_DYNCREATE( SVImageViewScroll, CScrollView )

BEGIN_MESSAGE_MAP(SVImageViewScroll, CScrollView)
	//{{AFX_MSG_MAP(SVImageViewScroll)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVImageViewScroll::SVImageViewScroll()
: CScrollView()
{
  CRuntimeClass *pImageRunTime = RUNTIME_CLASS (SVImageViewClass);
  psvImageView = (SVImageViewClass *) pImageRunTime->CreateObject ();
  ASSERT (psvImageView->IsKindOf (RUNTIME_CLASS (SVImageViewClass)));

	m_oOldScrollPoint = CPoint( 0, 0 );
	m_oOldMaxScrollPoint = CPoint( 0, 0 );
	m_hWindowBackgroundColor = ::CreateSolidBrush( SV_DEFAULT_IMAGE_VIEW_BACKGROUND_COLOR );
	m_SizeTotal = CSize( 0, 0 );
}

SVImageViewScroll::~SVImageViewScroll()
{
	if( m_hWindowBackgroundColor )
	{
		DeleteObject( m_hWindowBackgroundColor );
	}
}

BOOL SVImageViewScroll::OnEraseBkgnd( CDC* p_pDC )
{
	BOOL l_bOk = TRUE;

	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	CRect l_oRect( CPoint( 0, 0 ), l_oSizeTotal );

	ValidateRect( l_oRect ); // Removed painted area
	
	l_bOk = CScrollView::OnEraseBkgnd( p_pDC ); // Clears non-painted area

	InvalidateRect( l_oRect, false );  // invalidates painted area
	
	return l_bOk;
}

void SVImageViewScroll::OnDraw(CDC *pDC)
{
}

void SVImageViewScroll::OnInitialUpdate()
{
	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	SetImageSize( l_oSizeTotal );

	CScrollView::OnInitialUpdate();
}

BOOL SVImageViewScroll::Create(LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DWStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext)
{
 	LPCTSTR lpszClassName = AfxRegisterWndClass( CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW, 0, m_hWindowBackgroundColor , 0 );
	if (CWnd::Create( lpszClassName, _T( "Untitled Image Scroll" ), DWStyle, Rect, PParentWnd, NID, PContext ))
  {
    return psvImageView->Create (LPSZClassName, LPSZWindowName, DWStyle, Rect, (CWnd *) this, NID, PContext);
  }

  return FALSE;
}

void SVImageViewScroll::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		int l_iMapMode;
		SIZE l_oSizeTotal;
		SIZE l_oSizePage;
		SIZE l_oSizeLine;

		GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

		if( m_SizeTotal.cx != l_oSizeTotal.cx || m_SizeTotal.cy != l_oSizeTotal.cy )
		{
			SetImageSize( l_oSizeTotal );

			CScrollView::OnUpdate( pSender, lHint, pHint );   // This call will cause flicker
		}
	}
}

void SVImageViewScroll::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));

	CScrollView::OnMouseMove(nFlags, point);
}

BOOL SVImageViewScroll::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL l_bOk = TRUE;

	if ( psvImageView != NULL )
	{
		l_bOk = psvImageView->OnCommand (wParam, lParam);
	}

	l_bOk = CScrollView::OnCommand(wParam, lParam) && l_bOk;

	return l_bOk;
}

void SVImageViewScroll::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if ( psvImageView != NULL )
	{
		psvImageView->OnContextMenu( pWnd, point );
	}
}	

void SVImageViewScroll::SetViewSize( CSize &p_rcSize )
{
	CWnd *l_pcWnd = this;
	SVIPSplitterFrame *l_pSplitterFrame = NULL;

	do
	{
		l_pcWnd = l_pcWnd->GetParent();

		if ( l_pcWnd != NULL )
		{
			l_pSplitterFrame = dynamic_cast<SVIPSplitterFrame *>( l_pcWnd );
		}
	}
	while ( l_pSplitterFrame == NULL && l_pcWnd != NULL );

	if ( l_pSplitterFrame != NULL )
	{
		l_pSplitterFrame->SetViewSize( this, p_rcSize );
	}
}

void SVImageViewScroll::SetImageSize( SIZE p_oSize )
{
	int l_iMapMode;
	SIZE l_oSizeTotal;
	SIZE l_oSizePage;
	SIZE l_oSizeLine;

	CRect l_oWindowRect;
	CRect l_oClientRect;

	CPoint l_oPoint( 0, 0 );

	GetDeviceScrollSizes( l_iMapMode, l_oSizeTotal, l_oSizePage, l_oSizeLine );

	if( l_iMapMode == MM_TEXT )
	{
		l_oPoint = GetScrollPosition();
	}

	SetScrollSizes( MM_TEXT, p_oSize );

	m_SizeTotal = p_oSize;

	ScrollToPosition( CPoint( 0, 0 ) );

	if ( psvImageView != NULL )
	{
		CRect l_oRect( CPoint( 0, 0 ), p_oSize );

		psvImageView->MoveWindow( l_oRect );
	}

	GetClientRect( l_oClientRect );
	GetWindowRect( l_oWindowRect );

	double l_dX = l_oPoint.x;
	double l_dY = l_oPoint.y;

	if( l_oWindowRect.Size().cy == l_oClientRect.Size().cy )
	{
		l_dX = 0.0;

		if( l_oWindowRect.Size().cx <= l_oSizeTotal.cx )
		{
			m_oOldScrollPoint.x = l_oPoint.x;

			m_oOldMaxScrollPoint.x = l_oSizeTotal.cx - l_oWindowRect.Size().cx;
		}
	}
	else
	{
		if( l_oSizeTotal.cx < l_oWindowRect.Size().cx )
		{
			l_dX = (double)m_oOldScrollPoint.x;
		}

		if( m_oOldMaxScrollPoint.x != p_oSize.cx - l_oWindowRect.Size().cx )
		{
			if( 0 < m_oOldMaxScrollPoint.x )
			{
				l_dX = l_dX * (double)(p_oSize.cx - l_oWindowRect.Size().cx) / (double)m_oOldMaxScrollPoint.x;
			}

			m_oOldMaxScrollPoint.x = p_oSize.cx - l_oWindowRect.Size().cx;
		}

		m_oOldScrollPoint.x = (long)l_dX;
	}

	if( l_oWindowRect.Size().cx == l_oClientRect.Size().cx )
	{
		l_dY = 0;

		if( l_oWindowRect.Size().cy <= l_oSizeTotal.cy )
		{
			m_oOldScrollPoint.y = l_oPoint.y;

			m_oOldMaxScrollPoint.y = l_oSizeTotal.cy - l_oWindowRect.Size().cy;
		}
	}
	else
	{
		if( l_oSizeTotal.cy < l_oWindowRect.Size().cy )
		{
			l_dY = (double)m_oOldScrollPoint.y;
		}

		if( m_oOldMaxScrollPoint.y != p_oSize.cy - l_oWindowRect.Size().cy )
		{
			if( 0 < m_oOldMaxScrollPoint.y )
			{
				l_dY = l_dY * (double)(p_oSize.cy - l_oWindowRect.Size().cy) / (double)m_oOldMaxScrollPoint.y;
			}

			m_oOldMaxScrollPoint.y = p_oSize.cy - l_oWindowRect.Size().cy;
		}

		m_oOldScrollPoint.y = (long)l_dY;
	}

	l_oPoint.x = (long)l_dX;
	l_oPoint.y = (long)l_dY;

	ScrollToPosition( l_oPoint );
}

BOOL SVImageViewScroll::GetParameters(SVObjectWriter& rWriter)
{
	BOOL bOk = TRUE;

	CRect l_WindowRect;
	CSize l_ScrollSize = GetTotalSize();
	CPoint l_ScrollPoint = 	GetScrollPosition();

	GetWindowRect( l_WindowRect );

	_variant_t svVariant;

	rWriter.StartElement(CTAG_IMAGE_VIEW);
	bOk = psvImageView->GetParameters(rWriter) && bOk;
	rWriter.EndElement();
	
	rWriter.StartElement(CTAG_SCROLL_SIZE);
	svVariant = l_ScrollSize.cx;
	rWriter.WriteAttribute(CTAG_CX, svVariant);
	svVariant.Clear();

	svVariant = l_ScrollSize.cy;
	rWriter.WriteAttribute(CTAG_CY, svVariant);
	svVariant.Clear();
	
	rWriter.EndElement();

	rWriter.StartElement(CTAG_SCROLL_POINT);
	svVariant = l_ScrollPoint.x;
	rWriter.WriteAttribute(CTAG_X, svVariant);
	svVariant.Clear();

	svVariant = l_ScrollPoint.y;
	rWriter.WriteAttribute(CTAG_Y, svVariant);
	svVariant.Clear();

	rWriter.EndElement();

	svVariant = l_WindowRect.Height();
	rWriter.WriteAttribute(CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = l_WindowRect.Width();
	rWriter.WriteAttribute(CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();

	return bOk;
}

BOOL SVImageViewScroll::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiData = NULL;

	CSize l_ViewSize;
	CSize l_ScrollSize;
	CPoint l_ScrollPoint;

	bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_IMAGE_VIEW, htiParent, htiData );
	if ( bOk )
	{
		bOk = psvImageView->SetParameters( rTree, htiData );
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cy = svVariant;
		}
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cx = svVariant;
		}
	}

	if ( bOk )
	{
		SetViewSize( l_ViewSize );
	}

	CRect l_WindowRect;

	GetWindowRect( l_WindowRect );

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_SCROLL_SIZE, htiParent, htiData );
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CX, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollSize.cx = svVariant;
			}

			if ( bOk )
			{
				bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CY, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollSize.cy = svVariant;
				}
			}

			if( bOk )
			{
				SetImageSize( l_ScrollSize );
			}
		}
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_SCROLL_POINT, htiParent, htiData );
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_X, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollPoint.x = svVariant;
			}

			if ( bOk )
			{
				bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_Y, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollPoint.y = svVariant;
				}
			}
		}

		if ( bOk )
		{
		  ScrollToPosition( l_ScrollPoint );
		}
	}

	return bOk;
}

BOOL SVImageViewScroll::CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	_variant_t svVariant;

	SVTreeType::SVBranchHandle htiData = NULL;

	CSize l_ViewSize;
	CSize l_ScrollSize;
	CPoint l_ScrollPoint;

	CRect l_CheckWindowRect;
	CSize l_CheckScrollSize = GetTotalSize();
	CPoint l_CheckScrollPoint =	GetScrollPosition();

	GetWindowRect( l_CheckWindowRect );

	bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_IMAGE_VIEW, htiParent, htiData );
	if ( bOk )
	{
		bOk = psvImageView->SetParameters( rTree, htiData );
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cy = svVariant;
		}
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_ViewSize.cx = svVariant;
		}
	}

	if ( bOk )
	{
		if( l_ViewSize.cx != l_CheckWindowRect.Width() ||
		    l_ViewSize.cy != l_CheckWindowRect.Height() )
		{
			SetViewSize( l_ViewSize );

			GetWindowRect( l_CheckWindowRect );
		}
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_SCROLL_SIZE, htiParent, htiData );
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CX, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollSize.cx = svVariant;
			}

			if ( bOk )
			{
				bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CY, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollSize.cy = svVariant;
				}
			}

			if( bOk )
			{
				if( l_ScrollSize != l_CheckScrollSize )
				{
					SetImageSize( l_ScrollSize );
				}
			}
		}
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItemBranch( rTree, CTAG_SCROLL_POINT, htiParent, htiData );
		if ( bOk )
		{
			bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_X, htiData, svVariant );
			if ( bOk )
			{
				l_ScrollPoint.x = svVariant;
			}

			if ( bOk )
			{
				bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_Y, htiData, svVariant );
				if ( bOk )
				{
					l_ScrollPoint.y = svVariant;
				}
			}
		}

		if ( bOk )
		{
			if( l_ScrollPoint != l_CheckScrollPoint )
			{
				ScrollToPosition( l_ScrollPoint );
			}
		}
	}

	return bOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageViewScroll.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:56:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   11 Oct 2012 10:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   28 Sep 2012 14:49:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   27 Sep 2012 18:34:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated type definitions from HTREEITEM to generic branch handle type and fixed initialization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   25 Sep 2012 15:19:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   18 Sep 2012 18:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   04 Sep 2012 15:27:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   07 Aug 2012 16:11:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to reduce flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   09 Jul 2012 14:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Jul 2012 16:58:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Jun 2012 12:50:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Sep 2011 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   27 Jan 2011 11:20:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Dec 2010 11:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   05 Nov 2010 11:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Jun 2010 10:43:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Dec 2009 11:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   29 Oct 2007 16:40:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Manage GDI Resource - add DeleteObject in destructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Feb 2005 14:28:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Apr 2003 10:24:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Feb 2003 17:29:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Jan 2003 14:26:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Nov 2002 14:09:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added GetParameters and SetParameters functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 May 2001 15:42:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * OnContextMenu
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Jun 2000 10:39:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  155
 * SCR Title:  Context Menu available in Run Mode
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnContextMenu to only allow menu if not Run or test mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Apr 2000 08:31:22   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  131
 * SCR Title:  Bug fixes for blob analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Modified 'onContextMenu' to delete the Menu Item ID_ANALYZER_RESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Mar 2000 14:32:44   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  114
 * SCR Title:  Integrate Single Pixel Move Context Menu.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   "Adjust Position" was appearing on context window when no  image was being displayed in window.  This would cause a crash when selected.  
 * 
 * Changed
 *   SVImageViewScroll::OnContextMenu() to remove "adjust position"option when no image is in window.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Feb 2000 16:49:26   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
