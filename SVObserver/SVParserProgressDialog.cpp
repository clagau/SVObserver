//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressDialog
//* .File Name       : $Workfile:   SVParserProgressDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   25 Jun 2014 11:57:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVParserProgressDialog.h"
#include "SVInspectionProcess.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressStruct.h"
#include "ObjectInterfaces/SVUserMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// For display purposes
#define MAX_VISIBLE_PROGRESS_ROWS		10
#define MAX_VISIBLE_PROGRESS_COLUMNS	2 // 3
#define MAX_VISIBLE_PROGRESS_BARS (MAX_VISIBLE_PROGRESS_ROWS * MAX_VISIBLE_PROGRESS_COLUMNS)

/////////////////////////////////////////////////////////////////////////////
// SVParserProgressDialog dialog


SVParserProgressDialog::SVParserProgressDialog(LPCTSTR title, CWnd* pParent /*=NULL*/)
: SVProgressDialog(title, pParent)
{
	//{{AFX_DATA_INIT(SVParserProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_scrollBarShown = FALSE;
}

SVParserProgressDialog::~SVParserProgressDialog()
{
}

void SVParserProgressDialog::addScrollBar( int visibleWidth, int maxWidth )
{
	SCROLLINFO scrollInfo;

	if( !m_scrollBarShown )
	{
		ShowScrollBar( SB_HORZ );
		m_scrollBarShown = TRUE;

		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		scrollInfo.nPage = visibleWidth;
		scrollInfo.nPos = 0;
		scrollInfo.nTrackPos = 0;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = maxWidth;
		SetScrollInfo( SB_HORZ, &scrollInfo, FALSE );
	}
	else
	{
		GetScrollInfo( SB_HORZ, &scrollInfo );
		if( scrollInfo.nMax != maxWidth )
		{
			scrollInfo.cbSize = sizeof(SCROLLINFO);
			scrollInfo.fMask = SIF_ALL;
			scrollInfo.nPage = visibleWidth;
			scrollInfo.nMax = maxWidth;
			SetScrollInfo( SB_HORZ, &scrollInfo, FALSE );
		}
	}
}

void SVParserProgressDialog::resizeAndMoveDialog( CRect& RRect, int cnt )
{
	// Get Dialog's Client rect
	CRect clientRect;
	GetClientRect( &clientRect );

	int inflateW = 0;
	int inflateH = 0;
	
	if( cnt < MAX_VISIBLE_PROGRESS_BARS )
	{
		if( RRect.right > clientRect.right )
		{
			inflateW = m_originalWidth / 2; //(RRect.right - clientRect.right) + 10;
		}
	}
	else
	{
		int columnNo = cnt / MAX_VISIBLE_PROGRESS_ROWS;

		int visibleWidth = MAX_VISIBLE_PROGRESS_COLUMNS * m_originalWidth;
		int maxWidth = ( columnNo + 1 ) * m_originalWidth;

		addScrollBar( visibleWidth, maxWidth );
	}
	
	if( RRect.bottom > clientRect.bottom )
	{
		inflateH = (RRect.bottom - clientRect.bottom) + 10;
	}

	if( inflateW || inflateH )
	{
		CRect outsideRect;
		GetWindowRect( &outsideRect );
		outsideRect.InflateRect( inflateW, inflateH );
		CSize dlgSize( outsideRect.Width(), outsideRect.Height() );
		CPoint dlgPos( outsideRect.left, outsideRect.top );
		SetWindowPos( &wndTop, dlgPos.x, dlgPos.y, dlgSize.cx, dlgSize.cy, SWP_NOZORDER);
	}
}

BOOL SVParserProgressDialog::AddParser( unsigned long parserHandle, SVObjectScriptParserClass* pParser )
{
	BOOL rc = false;

	if( pParser )
	{
		rc = true;
		SVParserProgressControlStruct parserControl;

		ProgressCtrlSharedPtr pProgressCtrl = nullptr;
		StaticSharedPtr pStaticTextCtrl = nullptr;

		// Get Progress Control name...
		CString text;
		if( pParser->GetOwnerObject() )
			text = pParser->GetOwnerObject()->GetName();

		if( SV_IS_KIND_OF( pParser->GetOwnerObject(), SVInspectionProcess ) )
		{
			// Get the Document Name...
			SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( pParser->GetOwnerObject() );

			if( nullptr != pInspection )
			{
				text = pInspection->GetName();
			}
		}

		// Add to Total Size
		parserControl.TotalSize = pParser->GetTotal();

		// check if parser progress control array is empty
		if( m_parserControlList.size() == 0 )
		{
			//Set the shared pointer to null deleter because it is an object on the heap
			pProgressCtrl = ProgressCtrlSharedPtr(&m_progressCtrl, SVNullDeleter());
			pStaticTextCtrl = StaticSharedPtr(&m_staticTextCtrl, SVNullDeleter());
		}
		
		// Popluate parser progress control
		parserControl.OwnerGuid			= pParser->GetOwnerGuid();
		parserControl.pParser			= pParser;
		parserControl.pProgressCtrl		= pProgressCtrl;
		parserControl.pStaticTextCtrl	= pStaticTextCtrl;
		parserControl.Text = text;	
		m_parserControlList.insert( std::make_pair(parserHandle, parserControl) );
	}
	return rc;
}

SVParserProgressControlStruct& SVParserProgressDialog::GetParserControl(unsigned long parserHandle)
{
	SVParserControlList::iterator it = m_parserControlList.find(parserHandle);
	if (it != m_parserControlList.end())
	{
		return it->second;
	}
	throw std::exception("Parser control not found");
}

CProgressCtrl* SVParserProgressDialog::GetProgressControl( unsigned long parserHandle )
{
	try
	{
		SVParserProgressControlStruct& parserControl = GetParserControl(parserHandle);
		
		return parserControl.pProgressCtrl.get();
	}
	catch(std::exception& e)
	{
		TRACE(e.what());
	}
	return NULL;
}

CStatic* SVParserProgressDialog::GetTextControl( unsigned long parserHandle )
{
	try
	{
		SVParserProgressControlStruct& parserControl = GetParserControl(parserHandle);
		return parserControl.pStaticTextCtrl.get();
	}
	catch(std::exception& e)
	{
		TRACE(e.what());
	}
	return NULL;
}

SVParserProgressControlStruct& SVParserProgressDialog::GetFirstAvailableParserControl()
{
	for (SVParserControlList::iterator it = m_parserControlList.begin();it != m_parserControlList.end(); ++it)
	{
		if (it->second.pParser)
		{
			return it->second;
		}
	}
	throw (std::exception("No Parsers found"));
}

bool SVParserProgressDialog::CheckComplete()
{
	bool rc = true;
	for (SVParserControlList::iterator it = m_parserControlList.begin();it != m_parserControlList.end(); ++it)
	{
		if (it->second.pParser)
		{
			rc = false;
			break;
		}
	}
	return rc;
}

BEGIN_MESSAGE_MAP(SVParserProgressDialog, SVProgressDialog)
	//{{AFX_MSG_MAP(SVParserProgressDialog)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_UPDATE_PROGRESS, OnUpdateProgress )
	ON_MESSAGE( SV_END_PROGRESS_DIALOG, OnEndProgressDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVParserProgressDialog message handlers

LRESULT SVParserProgressDialog::OnUpdateProgress( WPARAM wParam, LPARAM lParam )
{
	SVParserProgressStruct* pParserProgress = reinterpret_cast<SVParserProgressStruct *>(lParam);
	int cnt = static_cast<int>(pParserProgress->amtProcessed); //(int)wParam;
	int total = static_cast<int>(pParserProgress->amtTotal); //(int)lParam;

	CProgressCtrl* pProgressCtrl = GetProgressControl( pParserProgress->parserHandle );

	// for setting of the initial range
	if (total > m_TotalSize)
	{
		m_TotalSize = total;
	}
	// To handle recursive routines
	if( total && total != m_TotalSize )
	{
		m_AmountProcessed = ( m_TotalSize - total ) + cnt;
	}
	else
	{
		m_AmountProcessed = cnt;
	}

	if( m_AmountProcessed && m_TotalSize )
	{
		double percent = ( static_cast<double>(m_AmountProcessed) / static_cast<double>(m_TotalSize) ) * 100.0;

		pProgressCtrl->SetPos( (int)percent );
	}
	return TRUE;
}

LRESULT SVParserProgressDialog::OnEndProgressDialog( WPARAM wParam, LPARAM lParam )
{
   msvEndDialogLock.Lock ();

	bool allDone = false;

	// Get Parser
	unsigned long parserHandle = static_cast<unsigned long>(lParam);

	try
	{
		SVParserProgressControlStruct& parserControl = GetParserControl(parserHandle);
		ProgressCtrlSharedPtr pProgressCtrl = parserControl.pProgressCtrl;
		
		// Update Progress Control
		pProgressCtrl->SetPos( 100 );

		// Update Text

		// Get Owner Object GUID
		const GUID& OwnerGuid = parserControl.pParser->GetOwnerGuid();

		// Delete the Parser
		delete parserControl.pParser;
		parserControl.pParser = NULL;

		// Post Message to MainFrame
		CWnd* pWnd = GetParent();

		// if all complete
		allDone = CheckComplete();
		if( allDone )
		{
			if (pWnd)
			{
				// Setup and create all objects
				for( SVParserControlList::iterator it = m_parserControlList.begin();it != m_parserControlList.end();++it )
				{
					SVParserProgressControlStruct& parserControl = it->second;
					parserControl.bValidate = static_cast<BOOL>(pWnd->SendMessage( SV_PARSE_OBJECT_SCRIPT_END, 0, ( LPARAM )&parserControl.OwnerGuid ));
				}

				// Validate and init all objects
				for( SVParserControlList::iterator it = m_parserControlList.begin();it != m_parserControlList.end();++it )
				{
					SVParserProgressControlStruct& parserControl = it->second;
					parserControl.bValidate = static_cast<int>(pWnd->SendMessage( SV_PARSE_OBJECT_CREATE_DONE, 0, ( LPARAM )&parserControl.OwnerGuid ));
				}
			}
			EndDialog( IDOK );
		}
		else
		{
			try
			{
				SVParserProgressControlStruct& parserControl = GetFirstAvailableParserControl();
				// Start next worker thread
				CWinThread* pParseThread = AfxBeginThread( SVObjectScriptParserClass::ParserThread, parserControl.pParser );
			}
			catch (std::exception& e)
			{
				TRACE(e.what());
			}
		}
	}
	catch (std::exception& e)
	{
		TRACE(e.what());
	}
	msvEndDialogLock.Unlock ();

	return TRUE;
}

void SVParserProgressDialog::SetProgressLocations()
{
	for( size_t i = 1; i < m_parserControlList.size(); ++i )
	{
		SVParserControlList::iterator it = m_parserControlList.begin();
		SVParserControlList::iterator prevIt = it;
		std::advance(it, i);
		std::advance(prevIt, i - 1);

		SVParserProgressControlStruct& l_rParserControl = it->second;

		// Get Progress Control name...
		CString text( l_rParserControl.Text );
		ProgressCtrlSharedPtr pProgressCtrl = nullptr;
		StaticSharedPtr pStaticTextCtrl = nullptr;

		int columnNo = static_cast<int>(i / MAX_VISIBLE_PROGRESS_ROWS);
		int rowNo = static_cast<int>(i % MAX_VISIBLE_PROGRESS_ROWS);

		CRect rect,textRect;
		CPoint textPos;
		CSize textSize;
		CPoint progressPos;
		CSize progressSize;

		// Get Location of previous Control
		if( rowNo )
		{
			SVParserProgressControlStruct& prevParserControl = prevIt->second;
			pProgressCtrl = prevParserControl.pProgressCtrl;
			pProgressCtrl->GetWindowRect( &rect );
		
			pStaticTextCtrl = prevParserControl.pStaticTextCtrl;
			pStaticTextCtrl->GetWindowRect( &textRect );
		}
		else if( columnNo )
		{
			SVParserProgressControlStruct& prevParserControl = m_parserControlList.begin()->second;
			pProgressCtrl = prevParserControl.pProgressCtrl;
			pProgressCtrl->GetWindowRect( &rect );
		
			pStaticTextCtrl = prevParserControl.pStaticTextCtrl;
			pStaticTextCtrl->GetWindowRect( &textRect );
		}
		// Get the Position and Sizes
		textPos.x = textRect.left;
		textPos.y = textRect.top;
		textSize.cx = textRect.Width();
		textSize.cy = textRect.Height();

		progressPos.x = rect.left;
		progressPos.y = rect.top;
		progressSize.cx = rect.Width();
		progressSize.cy = rect.Height();

		// Map to Client Coordinates
		ScreenToClient( &rect );
		ScreenToClient( &textRect );

		if( rowNo )
		{
			// Adjust y position, Height
			rect.top = rect.bottom + 24;
			rect.bottom = rect.top + progressSize.cy;
		}
		else if( columnNo )
		{
			// Adjust x position, Width
			rect.left += ( m_originalWidth * columnNo ); //rect.right + 20;
			rect.right = rect.left + progressSize.cx;
		}

		// Check if Dialog Needs resizing/moving
		resizeAndMoveDialog( rect, static_cast<int>(i) );

		// Create a new progress bar control
		pProgressCtrl = new CProgressCtrl();
		pProgressCtrl->Create( WS_VISIBLE | WS_CHILD, rect, this, static_cast<int>(i) );

		// Init the progress bar control
		pProgressCtrl->SetStep(1);
		pProgressCtrl->SetPos(0);

		// Set Location of the Static Text Control
		if( rowNo )
		{
			// Adjust y position, Height
			textRect.top = rect.top - ( progressPos.y - textPos.y );
			textRect.bottom = textRect.top + textSize.cy;
		}
		else if( columnNo )
		{
			// Adjust x position, Width
			textRect.left += ( m_originalWidth * columnNo ); // textRect.right + 20;
			textRect.right = textRect.left + textSize.cx;
		}

		// Create a new static text control
		pStaticTextCtrl = new CStatic();
		pStaticTextCtrl->Create( text, WS_VISIBLE | WS_CHILD, textRect, this );

		// Set the Font
		CFont* pFont = GetFont();
		pStaticTextCtrl->SendMessage( WM_SETFONT, ( WPARAM )( ( HFONT )*pFont ), 1 );

		l_rParserControl.pProgressCtrl		= pProgressCtrl;
		l_rParserControl.pStaticTextCtrl	= pStaticTextCtrl;
	}
}

BOOL SVParserProgressDialog::OnInitDialog() 
{
	SVProgressDialog::OnInitDialog();
	
	if( !m_Title.IsEmpty() )
		SetWindowText( m_Title );

	CRect rect;
	GetWindowRect( &rect );
	m_originalWidth = rect.Width();

	m_progressCtrl.SetStep(1);
	m_progressCtrl.SetPos(0);
	
	// Get Parser Control Element 0
	if( m_parserControlList.size() )
	{
		SVParserProgressControlStruct& parserControl = m_parserControlList.begin()->second;
		m_staticTextCtrl.SetWindowText( parserControl.Text );

		SetProgressLocations();

		// Start a worker thread
		CWinThread* pParseThread = AfxBeginThread( SVObjectScriptParserClass::ParserThread, parserControl.pParser );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVParserProgressDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int minPos,
		maxPos,
		curPos = GetScrollPos( SB_HORZ );

	int lineStep = m_originalWidth;
	CRect clientRect;
	GetClientRect( &clientRect );
	
	GetScrollRange( SB_HORZ, &minPos, &maxPos );

	switch( nSBCode )
	{
		case SB_PAGERIGHT:
			if( curPos <= ( maxPos - clientRect.Width() ) )
				curPos += clientRect.Width();
			else
				curPos = maxPos;
			break;

		case SB_PAGELEFT:
			if( curPos >= ( minPos + clientRect.Width() ) )
				curPos -= clientRect.Width();
			else
				curPos = minPos;
			break;

		case SB_LINERIGHT:	
			if( curPos <= ( maxPos - lineStep ) )
				curPos += lineStep;
			else
				curPos = maxPos;
			break;

		case SB_LINELEFT:		
			if( curPos >= ( minPos + lineStep ) )
				curPos -= lineStep;
			else
				curPos = minPos;
			break;

		case SB_THUMBPOSITION:
			curPos = nPos;
			break;

		case SB_THUMBTRACK:
			curPos = nPos;
			break;

		case SB_RIGHT:
			curPos = maxPos;
			break;

		case SB_LEFT:
			curPos = minPos;
			break;
	};

	// Set New position and get previous position
	int prevPos = SetScrollPos( SB_HORZ, curPos );

	// get the updated position
	curPos = GetScrollPos( SB_HORZ );

	// Scoll the Window accordingly
	ScrollWindow( prevPos - curPos, 0 );

	SVProgressDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVParserProgressDialog.cpp_v  $
 * 
 *    Rev 1.2   25 Jun 2014 11:57:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  910
 * SCR Title:  Fix memory leaks for Software Trigger Dialog and Parser Progress Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the CProgressCtrl and the CStatic to be a shared pointer so they get cleaned up correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:01:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:14:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   04 Sep 2012 15:50:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   08 Dec 2010 13:25:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   12 Jul 2004 12:38:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed dialog to work with new parser architecture
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   22 Apr 2003 13:37:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Aug 2001 07:52:32   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised "OnEndProgressDialog" so that all IPD objects are created first.  Next all IPD objects are validated and initialized.   Previous method was to loop through creating, validating, & initializing objects per IPD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   07 Aug 2001 15:11:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  208
 * SCR Title:  Improve configuration load time
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed memory leak of CWnd objects.
 * Changed IPDs to load in serial instead of in parallel
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   12 Apr 2000 14:25:06   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  125
 * SCR Title:  Make OCR tool load and go online without match string files present
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVLockableClass      msvEndDialogLock; 
 * was added to the SVParserProgressDialog class and implimented in SVParserProgressDialog::OnEndProgressDialog (), to control execution so that only one thread could execute OnEndProgressDialog () at a time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 21 2000 10:59:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *    Bug fix: Get name of document type safe.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   02 Dec 1999 11:02:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  64
 * SCR Title:  Progress Dialog
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed dead code.
 * Revised to eliminate warning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:24   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Nov 1999 09:20:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ScrollBars and Dynamic Sizing.
 * Added Support for Multiple Columns.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Nov 1999 11:36:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * Dialog Class to show the progress of multiple
 * SVObjectScriptParser objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
