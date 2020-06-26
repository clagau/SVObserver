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

#pragma region Includes
#include "stdafx.h"
#include "SVParserProgressDialog.h"
#include "SVInspectionProcess.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressStruct.h"
#include "Definitions/SVUserMessage.h"
#include "SVUtilityLibrary/SVNullDeleter.h"
#pragma endregion

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

BEGIN_MESSAGE_MAP(SVParserProgressDialog, SVProgressDialog)
	//{{AFX_MSG_MAP(SVParserProgressDialog)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE(SV_UPDATE_PROGRESS, OnUpdateProgress)
	ON_MESSAGE(SV_END_PROGRESS_DIALOG, OnEndProgressDialog)
END_MESSAGE_MAP()

SVParserProgressDialog::SVParserProgressDialog(LPCTSTR title, CWnd* pParent /*=nullptr*/)
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
		std::string Temp;
		if( pParser->GetOwnerObject() )
		{
			Temp = pParser->GetOwnerObject()->GetName();
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
		parserControl.m_OwnerId			= pParser->GetOwnerId();
		parserControl.pParser			= pParser;
		parserControl.pProgressCtrl		= pProgressCtrl;
		parserControl.pStaticTextCtrl	= pStaticTextCtrl;
		parserControl.Text = Temp;	
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	catch(std::exception& e)
	{
		TRACE(e.what());
	}
#else
	catch(std::exception& )
	{
	}
#endif
	return nullptr;
}

CStatic* SVParserProgressDialog::GetTextControl( unsigned long parserHandle )
{
	try
	{
		SVParserProgressControlStruct& parserControl = GetParserControl(parserHandle);
		return parserControl.pStaticTextCtrl.get();
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	catch(std::exception& e)
	{
		TRACE(e.what());
	}
#else
	catch(std::exception& )
	{
	}
#endif
	return nullptr;
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

LRESULT SVParserProgressDialog::OnUpdateProgress( WPARAM , LPARAM lParam )
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

LRESULT SVParserProgressDialog::OnEndProgressDialog( WPARAM, LPARAM lParam )
{
   msvEndDialogLock.Lock ();

	bool allDone = false;

	// Get Parser
	unsigned long parserHandle = static_cast<unsigned long> (lParam);

	try
	{
		SVParserProgressControlStruct& rParserControl = GetParserControl(parserHandle);
		ProgressCtrlSharedPtr pProgressCtrl = rParserControl.pProgressCtrl;
		
		// Update Progress Control
		pProgressCtrl->SetPos( 100 );

		// Delete the Parser
		delete rParserControl.pParser;
		rParserControl.pParser = nullptr;

		// if all complete
		allDone = CheckComplete();
		if( allDone )
		{
			EndDialog( IDOK );
		}
		else
		{
			try
			{
				SVParserProgressControlStruct& rParserStart = GetFirstAvailableParserControl();
				// Start next worker thread
				AfxBeginThread( SVObjectScriptParserClass::ParserThread, rParserStart.pParser );
			}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			catch(std::exception& e)
			{
				TRACE(e.what());
			}
#else
			catch(std::exception& )
			{
			}
#endif
		}
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
	catch(std::exception& e)
	{
		TRACE(e.what());
	}
#else
	catch(std::exception& )
	{
	}
#endif
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
		std::string Temp( l_rParserControl.Text );
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
		pProgressCtrl = ProgressCtrlSharedPtr{ new CProgressCtrl() };
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
		pStaticTextCtrl = StaticSharedPtr{ new CStatic() };
		pStaticTextCtrl->Create( Temp.c_str(), WS_VISIBLE | WS_CHILD, textRect, this );

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
		m_staticTextCtrl.SetWindowText( parserControl.Text.c_str() );

		SetProgressLocations();

		// Start a worker thread
		AfxBeginThread( SVObjectScriptParserClass::ParserThread, parserControl.pParser );
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

