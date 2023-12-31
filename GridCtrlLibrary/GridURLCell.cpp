//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridURLCell
//* .File Name       : $Workfile:   GridURLCell.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:35:18  $
//******************************************************************************

// GridURLCell.cpp: implementation of the GridURLCell class.
//
//////////////////////////////////////////////////////////////////////

#pragma region Includes
#include "stdafx.h"
#include "GridCell.h"
#include "GridCtrl.h"
#include "GridURLCell.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvGcl
{
	IMPLEMENT_DYNCREATE(GridURLCell, GridCell)

	#ifndef _WIN32_WCE
	HCURSOR GridURLCell::g_hLinkCursor = nullptr;
	#endif

	// Possible prefixes that indicate a hyperlink
	URLStruct GridURLCell::g_szURIprefixes[] = { 
		{ _T("www."),    _tcslen(_T("www."))    },
		{ _T("http:"),   _tcslen(_T("http:"))   },
		{ _T("mailto:"), _tcslen(_T("mailto:")) },
		{ _T("ftp:"),    _tcslen(_T("ftp:"))    },
		{ _T("https:"),  _tcslen(_T("https:"))  },
		{ _T("news:"),   _tcslen(_T("news:"))   },
		{ _T("gopher:"), _tcslen(_T("gopher:")) },
		{ _T("telnet:"), _tcslen(_T("telnet:")) },
		{ _T("url:"),    _tcslen(_T("url:"))    },
		{ _T("file:"),   _tcslen(_T("file:"))   },
		{ _T("ftp."),    _tcslen(_T("ftp."))    }
	};

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	GridURLCell::GridURLCell()
	{
	#ifndef _WIN32_WCE
		g_hLinkCursor = GetHandCursor();
	#endif
		m_bLaunchUrl = TRUE;
		m_clrUrl = GetSysColor(COLOR_HIGHLIGHT);
	}

	GridURLCell::~GridURLCell()
	{
	}

	BOOL GridURLCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
	{
		// If url is present then change text color
		if (HasUrl(GetText()))
			SetTextClr(m_clrUrl);

		// Good a place as any to store the bounds of the rect
		m_Rect = rect;

		return GridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);
	}

	#pragma warning(disable:4100)
	BOOL GridURLCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
	{
		return FALSE;
	}
	#pragma warning(default:4100)

	void GridURLCell::OnClick(CPoint PointCellRelative)
	{
	#ifndef _WIN32_WCE
		CString strURL;
		if (GetAutoLaunchUrl() && OverURL(PointCellRelative, strURL))
		{
			ShellExecute(nullptr, _T("open"), strURL, nullptr, nullptr, SW_SHOW);
		}
	#endif
	}

	// Return TRUE if you set the cursor
	BOOL GridURLCell::OnSetCursor()
	{
	#ifndef _WIN32_WCE
		CString strURL;
		CPoint pt(GetMessagePos());
		GetGrid()->ScreenToClient(&pt);
		pt = pt - m_Rect.TopLeft();

		if (OverURL(pt, strURL))
		{
			SetCursor(g_hLinkCursor);
			return TRUE;
		}
		else
	#endif
			return GridCell::OnSetCursor();
	}

	#ifndef _WIN32_WCE
	HCURSOR GridURLCell::GetHandCursor()
	{
		if (nullptr == g_hLinkCursor)		// No cursor handle - load our own
		{
			// Get the windows directory
			CString strWndDir;
			GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
			strWndDir.ReleaseBuffer();

			strWndDir += _T("\\winhlp32.exe");
			// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
			HMODULE hModule = LoadLibrary(strWndDir);
			if( hModule )
			{
				HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
				if( hHandCursor )
				{
					g_hLinkCursor = CopyCursor(hHandCursor);
				}
			}
			FreeLibrary(hModule);
		}

		return g_hLinkCursor;
	}
	#endif

	////////////////////////////////////////////////////////////////////////////////////////////
	// Helper functions

	BOOL GridURLCell::HasUrl(CString str)
	{
		int nNumPrefixes = sizeof(g_szURIprefixes) / sizeof(g_szURIprefixes[0]);
		for (int i = 0; i < nNumPrefixes; i++)
			//if (str.Left(g_szURIprefixes[i].nLength) == g_szURIprefixes[i].szURLPrefix)
			if (str.Find(g_szURIprefixes[i].szURLPrefix) >= 0)
				return TRUE;

		return FALSE;
	}

	// here we figure out if we are over a URL or not
	BOOL GridURLCell::OverURL(CPoint& pt, CString& strURL)
	{

		BOOL bOverURL = FALSE;
		CSize size = GetTextExtent(GetText());

		// Add left of cell so we know if we clicked on text or not
		pt.x += m_Rect.left;
		CPoint center = m_Rect.CenterPoint();

		if ((m_nFormat & DT_RIGHT) && pt.x >= (m_Rect.right - size.cx))
		{
			bOverURL = TRUE;
		}	
		else if ((m_nFormat & DT_CENTER) && 
				 ((center.x - (size.cx/2)) <= pt.x) && (pt.x <= (center.x + (size.cx/2))) )
		{
			bOverURL = TRUE;
		}
		else if (pt.x <= (size.cx + m_Rect.left))
		{
			bOverURL = TRUE;
		}

		if (!bOverURL)
			return FALSE;

		// We are over text - but are we over a URL?
		strURL = GetText();

		// remove left of cell so we have original point again 
		pt.x -= m_Rect.left;
		if (m_nFormat & DT_RIGHT)
		{
			int wide = m_Rect.Width() - size.cx;
			pt.x -= wide;
			if (pt.x <= 0)
				return FALSE;
		}

		if (m_nFormat & DT_CENTER)
		{
			int wide = m_Rect.Width() - size.cx;
			pt.x -= (wide/2);
			if (pt.x <= 0 || pt.x > (size.cx + (wide/2)))
				return FALSE;
		}

		// Turn point into a letter
	#if  !defined(_WIN32_WCE) || (_WIN32_WCE > 210)
		float width = (float)size.cx / (float)strURL.GetLength();
		// Find spaces before and after letter, process text between
		int ltrs = (int)((float)pt.x / width);
		int endSpace = strURL.Find(_T(' '), ltrs);
		if (endSpace != -1)
			strURL.Delete(endSpace, strURL.GetLength()-endSpace);

		int beginSpace = strURL.ReverseFind(_T(' '));
		if (beginSpace != -1)
			strURL.Delete(0, ++beginSpace);
	#endif

		// Does text have url
		return HasUrl(strURL);
	}

} // namespace SvGcl
