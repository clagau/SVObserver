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
//Moved to precompiled header: #include <algorithm>
#include "SVDlgImage.h"
#include "InspectionCommands/GetImage.h"
#include "InspectionCommands/GetOutputRectangle.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

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
		pParent->PostMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y));
	}

	BEGIN_MESSAGE_MAP(SVDlgImageClass, CStatic)
		//{{AFX_MSG_MAP(SVDlgImageClass)
		ON_WM_PAINT()
		ON_WM_DESTROY()
		//}}AFX_MSG_MAP
		ON_WM_HSCROLL()
		ON_WM_VSCROLL()
	END_MESSAGE_MAP()

	SVDlgImageClass::SVDlgImageClass()
		: m_inspectionId(GUID_NULL)
		, m_imageId(GUID_NULL)
	{
		mbInit = false;
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

	void SVDlgImageClass::UpdateImageInfo(const SVGUID& inspectionId, const SVGUID& imageId)
	{
		ASSERT(GUID_NULL != inspectionId || GUID_NULL != imageId);	// if no source image, must supply info & handle

		m_inspectionId = inspectionId;
		m_imageId = imageId;

		if (nullptr == m_sbHorizontal.m_hWnd)
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

		GetClientRect(&m_ClientRect);
		m_OriginalClientRect = m_ClientRect;

		bool bSetZoom = false;
		CRect rectTemp = GetOutputRectFromImage();

		if (m_SourceRect != rectTemp)
		{
			m_ViewportRect = m_SourceRect;
			bSetZoom = true;
		}
		m_SourceRect = rectTemp;
		m_dFullSizeZoomFactor = (double)m_ClientRect.Width() / (double)m_SourceRect.Width();
		m_dFullSizeZoomFactorY = (double)m_ClientRect.Height() / (double)m_SourceRect.Height();
		m_dFullSizeZoomFactorScrollX = (double)(m_ClientRect.Width() - m_iScrollBarSize) / (double)m_SourceRect.Width();
		m_dFullSizeZoomFactorScrollY = (double)(m_ClientRect.Height() - m_iScrollBarSize) / (double)m_SourceRect.Height();
		if (bSetZoom)
			SetZoom(0);
		if (m_bShowScrollBars)
		{
			m_ClientRect.bottom = m_OriginalClientRect.bottom - m_iScrollBarSize;
			m_ClientRect.right = m_OriginalClientRect.right - m_iScrollBarSize;
		}

		mbInit = true;

	}


	void SVDlgImageClass::refresh()
	{
		InvalidateRect(nullptr, true);
		UpdateWindow();
	}

	double SVDlgImageClass::GetZoom()
	{
		return m_dZoomX;
	}

	HRESULT SVDlgImageClass::SetZoom(double dPercent)
	{
		if (dPercent == 0)
		{
			dPercent = m_dFullSizeZoomFactor;
			m_ViewportRect.left = 0;
			m_ViewportRect.top = 0;
		}


		if (dPercent > m_dFullSizeZoomFactor)
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
			info.nPage = static_cast<unsigned int>(rect.Width() * dPagePercent);
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

	HRESULT SVDlgImageClass::SetViewportOrigin(CPoint ptOrigin, bool bRedraw)
	{
		CSize size = m_ViewportRect.Size();
		m_ViewportRect.top = ptOrigin.y;
		m_ViewportRect.left = ptOrigin.x;
		m_ViewportRect.right = ptOrigin.x + size.cx;
		m_ViewportRect.bottom = ptOrigin.y + size.cy;

		if (bRedraw)
		{
			m_sbHorizontal.SetScrollPos(ptOrigin.x);
			m_sbVertical.SetScrollPos(ptOrigin.y);
		}

		return S_OK;
	}

	CPoint SVDlgImageClass::GetViewportOrigin()
	{
		return m_ViewportRect.TopLeft();
	}

	HRESULT SVDlgImageClass::SetViewportRect(CRect rect)	// sets origin and zoom
	{
		m_ViewportRect = rect;
		// calculate zoom
		m_dZoomX = (double)m_SourceRect.Width() / (double)m_ViewportRect.Width();
		m_dZoomY = (double)m_SourceRect.Height() / (double)m_ViewportRect.Height();
		return S_OK;
	}


	void SVDlgImageClass::OnPaint()
	{
		CPaintDC dc(this); // device context for painting

		if (mbInit)
		{
			HBITMAP hbm = nullptr;
			CComPtr<IPicture> picture;

			typedef SvCmd::GetImage Command;
			typedef std::shared_ptr<Command> CommandPtr;
			CommandPtr commandPtr{ new Command(m_imageId) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_inspectionId, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				CComPtr<IPictureDisp> picDisp = commandPtr->Image();
				if (picDisp)
				{
					picDisp.QueryInterface<IPicture>(&picture);
					if (picture)
					{
						hr = picture->get_Handle((OLE_HANDLE*)&hbm);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
						if (S_OK != hr)
						{
							TRACE(_T("SVDlgImageClass::OnPaint() - pPicture->get_Handle failed - %08lx\n"), hr);
						}
#endif
					}
				}
			}

			CDC memDC;
			memDC.CreateCompatibleDC(&dc);

			//
			// Render the device independent bitmap (DIB) (stretching as required).
			//
			if (nullptr != hbm)
			{
				HBITMAP hOld = (HBITMAP) ::SelectObject(memDC.m_hDC, hbm);
				if (m_ViewportRect.Width() * m_dFullSizeZoomFactor > m_ClientRect.Width())
				{
					int iRegionX = static_cast<int>(m_SourceRect.Width() * m_dFullSizeZoomFactor * m_dZoomX);
					int iRegionY = static_cast<int>(m_SourceRect.Height() * m_dFullSizeZoomFactorY * m_dZoomY);
					CRect rectRight(iRegionX, 0, m_ClientRect.Width(), m_ClientRect.Height());
					CRect rectBottom(0, std::min(iRegionY, m_ClientRect.Height()), m_ClientRect.Width(), m_ClientRect.Height());
					dc.StretchBlt(0, 0, m_ClientRect.Width(), m_ClientRect.Height(),
						&memDC, m_ViewportRect.left, m_ViewportRect.top,
						m_ViewportRect.Width(), m_ViewportRect.Height(), SRCCOPY);
					dc.FillSolidRect(&rectRight, RGB(64, 64, 128));
					dc.FillSolidRect(&rectBottom, RGB(64, 64, 128));
				}
				else
				{
					dc.StretchBlt(0, 0, m_ClientRect.Width(), m_ClientRect.Height(),
						&memDC, m_ViewportRect.left, m_ViewportRect.top,
						m_ViewportRect.Width(), m_ViewportRect.Height(), SRCCOPY);
				}
				::SelectObject(memDC.m_hDC, hOld);
			}
			memDC.DeleteDC();
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

	void SVDlgImageClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pBar)
	{
		int iPos = pBar->GetScrollPos();
		SCROLLINFO info;
		switch (nSBCode)
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

	void SVDlgImageClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pBar)
	{
		int iPos = pBar->GetScrollPos();
		SCROLLINFO info;
		switch (nSBCode)
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

	CRect SVDlgImageClass::GetOutputRectFromImage()
	{
		CRect rect;
		typedef SvCmd::GetOutputRectangle Command;
		typedef std::shared_ptr<Command> CommandPtr;
		CommandPtr commandPtr{ new Command(m_imageId) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_inspectionId, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			rect = commandPtr->getRectangle();
		}
		return rect;
	}
} //namespace SvOg
