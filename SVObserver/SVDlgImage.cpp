//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImage
//* .File Name       : $Workfile:   SVDlgImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Aug 2013 10:17:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgImage.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVObserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int SVDlgImageClass::m_iScrollBarSize = 12;



BEGIN_MESSAGE_MAP(SVDlgImageScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(SVDlgImageScrollBar)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SVDlgImageScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd* pParent = GetParent();
	ClientToScreen(&point);
	pParent->ScreenToClient(&point);
	pParent->PostMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y) );
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVDlgImageClass
//* Note(s)    : CStatic
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


BEGIN_MESSAGE_MAP(SVDlgImageClass, CStatic)
	//{{AFX_MSG_MAP(SVDlgImageClass)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

SVDlgImageClass::SVDlgImageClass()
: m_bDisplayInputImage( false )
{
	mbInit = false;
	mpSourceImage = NULL;
	m_dZoomX = 1.0;
	m_dZoomY = 1.0;
	m_dFullSizeZoomFactor = 1.0;
	m_dFullSizeZoomFactorY = 1.0;
	m_dFullSizeZoomFactorScrollX = 1.0;
	m_dFullSizeZoomFactorScrollY = 1.0;
	m_bShowScrollBars = false;
}

SVDlgImageClass::~SVDlgImageClass()
{

}

void SVDlgImageClass::UpdateImageInfo( SVImageClass* pImage, bool bDisplayInputImage )
{
	mpSourceImage = pImage;
	m_bDisplayInputImage = bDisplayInputImage;
	
	// Allocate resources for current call to init()...
	if( mpSourceImage != NULL )
	{
		mSourceImageInfo = mpSourceImage->GetImageInfo();

		UpdateImageInfo( NULL, SVSmartHandlePointer() );
	}
}


void SVDlgImageClass::UpdateImageInfo( const SVImageInfoClass* pImageInfo, SVSmartHandlePointer pImageHandle )
{
	ASSERT( mpSourceImage != NULL || (pImageInfo != NULL && pImageHandle != NULL) );	// if no source image, must supply info & handle

	if ( pImageInfo != NULL )
	{
		mSourceImageInfo = *pImageInfo;
		mpSourceImage = NULL;	// EB 20050408
	}
	if ( !( pImageHandle.empty() ) )
	{
		mSourceImageHandle = pImageHandle;
	}

	if ( m_sbHorizontal.m_hWnd == NULL )
	{
		CRect rect;
		CRect rectClient;
		GetWindowRect(&rect);
		GetClientRect(&rectClient);
		CRect rectH(rectClient);
		CRect rectV(rectClient);
		rectV.left = rectV.right - m_iScrollBarSize;	// skinny scrollbar
		rectV.bottom -= m_iScrollBarSize;
		rectH.top = rectH.bottom - m_iScrollBarSize;	// skinny scrollbar
		m_sbHorizontal.Create(WS_CHILD | SBS_HORZ, rectH, this, 100);
		m_sbVertical.Create(WS_CHILD | SBS_VERT, rectV, this, 101);
	}

	GetClientRect( &m_ClientRect );
	m_OriginalClientRect = m_ClientRect;

	bool bSetZoom = false;
	CRect rectTemp;
	mSourceImageInfo.GetOutputRectangle( rectTemp );
	if ( m_SourceRect != rectTemp )
	{
		m_ViewportRect = m_SourceRect;
		bSetZoom = true;
	}
	m_SourceRect = rectTemp;
	m_dFullSizeZoomFactor = (double) m_ClientRect.Width() / (double) m_SourceRect.Width();
	m_dFullSizeZoomFactorY = (double) m_ClientRect.Height() / (double) m_SourceRect.Height();
	m_dFullSizeZoomFactorScrollX = (double) (m_ClientRect.Width() - m_iScrollBarSize) / (double) m_SourceRect.Width();
	m_dFullSizeZoomFactorScrollY = (double) (m_ClientRect.Height() - m_iScrollBarSize) / (double) m_SourceRect.Height();
	if ( bSetZoom )
		SetZoom(0);
	if ( m_bShowScrollBars )
	{
		m_ClientRect.bottom = m_OriginalClientRect.bottom - m_iScrollBarSize;
		m_ClientRect.right = m_OriginalClientRect.right - m_iScrollBarSize;
	}

	mbInit = true;

}


void SVDlgImageClass::refresh()
{
//	RedrawWindow();
	InvalidateRect( ( LPCRECT ) &m_ClientRect, TRUE );
	UpdateWindow();
}

double SVDlgImageClass::GetZoom()
{
	return m_dZoomX;
}

HRESULT SVDlgImageClass::SetZoom(double dPercent)
{
	if ( dPercent == 0 )
	{
		dPercent = m_dFullSizeZoomFactor;
		m_ViewportRect.left = 0;
		m_ViewportRect.top = 0;
	}


	if ( dPercent > m_dFullSizeZoomFactor )
	{
		m_dZoomX = dPercent / m_dFullSizeZoomFactorScrollX;
		m_dZoomY = dPercent / m_dFullSizeZoomFactorScrollY;
		m_bShowScrollBars = true;

		m_sbHorizontal.ShowScrollBar();
		m_sbVertical.ShowScrollBar();
		m_ClientRect.bottom = m_OriginalClientRect.bottom - m_iScrollBarSize;
		m_ClientRect.right = m_OriginalClientRect.right - m_iScrollBarSize;

		// calculate viewport
		m_ViewportRect.right = static_cast<long>(m_ViewportRect.left + m_SourceRect.Width() / m_dZoomX);
		m_ViewportRect.bottom = static_cast<long>(m_ViewportRect.top + m_SourceRect.Height() / m_dZoomY);

		//CRect rect = m_ClientRect;
		CRect rect = m_SourceRect;
		m_sbHorizontal.SetScrollRange(0, rect.Width());
		m_sbVertical.SetScrollRange(0, rect.Height());
		SCROLLINFO info;
		m_sbHorizontal.GetScrollInfo(&info);
		double dPagePercent = (double)m_ViewportRect.Width() / (double)m_SourceRect.Width();
		info.nPage = static_cast<unsigned int >(rect.Width() * dPagePercent);
		info.fMask = SIF_PAGE;
		m_sbHorizontal.SetScrollInfo(&info);
		dPagePercent = (double)m_ViewportRect.Height() / (double)m_SourceRect.Height();
		m_sbVertical.GetScrollInfo(&info);
		info.nPage = static_cast<unsigned int>(rect.Height() * dPagePercent);
		m_sbVertical.SetScrollInfo(&info);
	}
	else
	{
		m_bShowScrollBars = false;
		m_dZoomX = dPercent / m_dFullSizeZoomFactor;
		m_dZoomY = dPercent / m_dFullSizeZoomFactor;

		m_sbHorizontal.ShowScrollBar(FALSE);
		m_sbVertical.ShowScrollBar(FALSE);
		m_ClientRect.bottom = m_OriginalClientRect.bottom;
		m_ClientRect.right = m_OriginalClientRect.right;

		// calculate viewport
		m_ViewportRect.right = static_cast<long>(m_ViewportRect.left + m_SourceRect.Width() / m_dZoomX);
		m_ViewportRect.bottom = static_cast<long>(m_ViewportRect.top + m_SourceRect.Height() / m_dZoomY);
	}

	OnSetZoom();

	refresh();
	return S_OK;
}

HRESULT SVDlgImageClass::SetZoomFullSize()// scale image to size of display window
{
	m_dZoomX = m_dFullSizeZoomFactor;	//!! needs to be set to full scale, not 100%
	m_dZoomY = m_dFullSizeZoomFactor;	//!! needs to be set to full scale, not 100%
	return S_OK;
}

HRESULT SVDlgImageClass::SetViewportOrigin( CPoint ptOrigin, bool bRedraw )
{
	CSize size = m_ViewportRect.Size();
	m_ViewportRect.top = ptOrigin.y;
	m_ViewportRect.left = ptOrigin.x;
	m_ViewportRect.right = ptOrigin.x + size.cx;
	m_ViewportRect.bottom = ptOrigin.y + size.cy;

	if ( bRedraw )
	{
		m_sbHorizontal.SetScrollPos( ptOrigin.x );
		m_sbVertical.SetScrollPos( ptOrigin.y );
	}

	return S_OK;
}

CPoint SVDlgImageClass::GetViewportOrigin()
{
	return m_ViewportRect.TopLeft();
}

HRESULT SVDlgImageClass::SetViewportRect( CRect rect )	// sets origin and zoom
{
	m_ViewportRect = rect;
	// calculate zoom
	m_dZoomX = (double) m_SourceRect.Width() / (double) m_ViewportRect.Width();
	m_dZoomY = (double) m_SourceRect.Height() / (double) m_ViewportRect.Height();
	return S_OK;
}


void SVDlgImageClass::OnPaint() 
{
	CPaintDC dc(this); // device context for painting


	if ( mbInit )
	{
		SVSmartHandlePointer l_ImageHandle;
		SVMatroxBuffer l_NewBuffer;

		if ( mpSourceImage != NULL )
		{
			if ( m_bDisplayInputImage )
			{
				if( mpSourceImage->GetImageType() == SVImageTypeEnum::SVImageTypePhysical )
				{
					mpSourceImage->GetParentImage()->GetImageHandle( l_ImageHandle );
				}
				else
				{
				mpSourceImage->GetParentImageHandle( l_ImageHandle );
				}
			}
			else
			{
				mpSourceImage->GetImageHandle( l_ImageHandle );
			}
		}
		else
		{
			l_ImageHandle = mSourceImageHandle;
		}
		
		SVImageBufferHandleImage l_MilHandle;
		SVBitmapInfo DibInfo;
		HBITMAP hbm = NULL;
		HBITMAP hOld = NULL;
		BYTE* pMilBuffer = NULL;
		CDC memDC;

		memDC.CreateCompatibleDC( &dc );

		if( !( l_ImageHandle.empty() ) )
		{
			l_ImageHandle->GetData( l_MilHandle );

			DibInfo = l_ImageHandle->GetBitmapInfo();
			pMilBuffer = ( BYTE* )l_ImageHandle->GetBufferAddress();
		
			SVMatroxBufferInterface::SVStatusCode l_Code = 
				SVMatroxBufferInterface::Create(l_NewBuffer, l_MilHandle.GetBuffer() );
			if( l_Code == S_OK)
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer( l_NewBuffer, l_MilHandle.GetBuffer());
				l_Code = SVMatroxBufferInterface::GetBitmapInfo( DibInfo, l_NewBuffer );
				l_Code = SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, l_NewBuffer );
			}
		}

		if( pMilBuffer != NULL && !( DibInfo.empty() ) )
		{
			BOOL b = m_DIB.DeleteObject();
			HRESULT hr = m_DIB.LoadDIBitmap( DibInfo.GetBitmapInfo(), pMilBuffer );
			
			if ( (HBITMAP) m_DIB != NULL )
			{
				hbm = (HBITMAP) m_DIB;
			}// end if
		}// end if
		
		//
		// Render the device independent bitmap (DIB) (stretching as required).
		//
		if ( hbm != NULL )
		{
			hOld = ( HBITMAP ) ::SelectObject( memDC.m_hDC, hbm );
			if ( m_ViewportRect.Width() * m_dFullSizeZoomFactor > m_ClientRect.Width() )
			{
				int iRegionX = static_cast<int>(m_SourceRect.Width() * m_dFullSizeZoomFactor * m_dZoomX);
				int iRegionY = static_cast<int>(m_SourceRect.Height() * m_dFullSizeZoomFactorY * m_dZoomY);
				CRect rectRight( iRegionX, 0, m_ClientRect.Width(), m_ClientRect.Height() );
				CRect rectBottom( 0, min(iRegionY, m_ClientRect.Height()), m_ClientRect.Width(), m_ClientRect.Height() );
				dc.StretchBlt(  0, 0, m_ClientRect.Width(),  m_ClientRect.Height(), 
				                &memDC, m_ViewportRect.left, m_ViewportRect.top,
				                m_ViewportRect.Width(), m_ViewportRect.Height(), SRCCOPY );
				dc.FillSolidRect(&rectRight, RGB(64,64,128));
				dc.FillSolidRect(&rectBottom, RGB(64,64,128));
			}
			else
			{
				dc.StretchBlt(  0, 0, m_ClientRect.Width(),  m_ClientRect.Height(), 
				                &memDC, m_ViewportRect.left, m_ViewportRect.top,
				                m_ViewportRect.Width(), m_ViewportRect.Height(), SRCCOPY );			
			}
			::SelectObject( memDC.m_hDC, hOld );
		}
		memDC.DeleteDC();

		l_NewBuffer.clear();

	}
	
	OnPaintOverlay(dc);	// give derived classes a chance to draw
	// Do not call CStatic::OnPaint() for painting messages
}

void SVDlgImageClass::OnPaintOverlay(CPaintDC& dc)	// derived classes implement this to draw after picture is displayed
{
}

void SVDlgImageClass::OnDestroy() 
{
	CStatic::OnDestroy();
}

void SVDlgImageClass::DestroyBuffers()
{
}

void SVDlgImageClass::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pBar )
{
	int iPos = pBar->GetScrollPos();
	SCROLLINFO info;
	switch ( nSBCode )
	{
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			iPos = nPos;
			break;
		case SB_PAGEDOWN:
			iPos = pBar->GetScrollPos();
			pBar->GetScrollInfo(&info);
			iPos += info.nPage;
			break;
		case SB_PAGEUP:
			iPos = pBar->GetScrollPos();
			pBar->GetScrollInfo(&info);
			iPos -= info.nPage;
			break;
		case SB_LINEDOWN:
			iPos = pBar->GetScrollPos();
			iPos += 10;
			break;
		case SB_LINEUP:
			iPos = pBar->GetScrollPos();
			iPos -= 10;
			break;
	}

	pBar->SetScrollPos(iPos);
	m_ViewportRect.OffsetRect(CPoint(iPos - m_ViewportRect.left, 0));
	refresh();
}

void SVDlgImageClass::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pBar )
{
	int iPos = pBar->GetScrollPos();
	SCROLLINFO info;
	switch ( nSBCode )
	{
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			iPos = nPos;
			break;
		case SB_PAGEDOWN:
			iPos = pBar->GetScrollPos();
			pBar->GetScrollInfo(&info);
			iPos += info.nPage;
			break;
		case SB_PAGEUP:
			iPos = pBar->GetScrollPos();
			pBar->GetScrollInfo(&info);
			iPos -= info.nPage;
			break;
		case SB_LINEDOWN:
			iPos = pBar->GetScrollPos();
			iPos += 10;
			break;
		case SB_LINEUP:
			iPos = pBar->GetScrollPos();
			iPos -= 10;
			break;
	}

	pBar->SetScrollPos(iPos);
	m_ViewportRect.OffsetRect(CPoint(0, iPos - m_ViewportRect.top));
	refresh();
}

void SVDlgImageClass::OnSetZoom()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDlgImage.cpp_v  $
 * 
 *    Rev 1.3   12 Aug 2013 10:17:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  842
 * SCR Title:  Fix Image Tool - source image tab on mask editor tab.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed merge error in onPaint.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Aug 2013 08:00:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  842
 * SCR Title:  Fix Image Tool - source image tab on mask editor tab.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Image display for mask image uses type physicial, get parent image only for this type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jul 2013 13:28:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Jun 2013 10:15:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  842
 * SCR Title:  Fix Image Tool - source image tab on mask editor tab.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnPaint to display image correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:15:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   07 Apr 2011 16:32:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   25 Feb 2011 12:07:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   10 Feb 2011 14:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   27 Jan 2011 11:06:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 May 2008 11:40:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Temperary Buffer for OnPaint message handler so GetHostAddress and GetBitmapinfo will succeed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   24 Jul 2007 11:47:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   21 Jun 2007 12:27:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Aug 2006 15:48:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Aug 2006 16:03:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Aug 2005 15:39:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed draw bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   19 May 2005 14:56:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Zoom capability
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Feb 2005 13:49:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jan 2004 10:58:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed MIL Display Resources to dislplay information to dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2003 12:57:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added new init method, removed dead code / parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 09:15:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jun 2003 08:54:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method DestroyBuffers to cleanup display buffers that were never being deleted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 09:09:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Mar 2003 13:17:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jan 2003 15:19:08   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated init, OnDestroy and OnPaint methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Nov 2002 11:53:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Cosmetic changes
 *             Changed M_DISP-related MIL calls to wrapper calls
 *             Changed MIL image id�s to wrapper structs
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Sep 2000 16:28:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:49:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin. Separated out from SVView module
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
