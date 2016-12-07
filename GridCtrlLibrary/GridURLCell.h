//******************************************************************************
//* Copyright (c) 1998-2005
//* All Rights Reserved
//* Author           : Chris Maunder <chris@codeproject.com>
//* License          : The Code Project Open License (CPOL)
//* From             : http://www.codeproject.com/Articles/8/MFC-Grid-control
//******************************************************************************
//* .Module Name     : GridURLCell
//* .File Name       : $Workfile:   GridURLCell.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:34  $
//******************************************************************************

// GridURLCell.h: interface for the CGridURLCell class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#pragma region Includes
#include "GridCellBase.h"
#pragma endregion Includes

namespace Seidenader { namespace GridCtrlLibrary
{
	typedef struct 
	{
		LPCTSTR szURLPrefix;
		size_t  nLength;
	} URLStruct;

	class CGridURLCell : public CGridCell  
	{
		DECLARE_DYNCREATE(CGridURLCell)

	public:
		CGridURLCell();
		virtual ~CGridURLCell();

		virtual BOOL     Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE) override;
		virtual BOOL     Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar) override;
		virtual LPCTSTR  GetTipText() const override { return nullptr; }
		void SetAutoLaunchUrl(BOOL bLaunch = TRUE) { m_bLaunchUrl = bLaunch;	}
		BOOL GetAutoLaunchUrl() { return m_bLaunchUrl;	}

	protected:
		virtual BOOL OnSetCursor() override;
		virtual void OnClick(CPoint PointCellRelative) override;

		BOOL HasUrl(CString str);
		BOOL OverURL(CPoint& pt, CString& strURL);

	protected:
	#ifndef _WIN32_WCE
		static HCURSOR g_hLinkCursor;		// Hyperlink mouse cursor
		HCURSOR GetHandCursor();
	#endif
		static URLStruct g_szURIprefixes[];

	protected:
		COLORREF m_clrUrl;
		BOOL     m_bLaunchUrl;
		CRect    m_Rect;
	};

} /*namespace GridCtrlLibrary*/ } /*namespace Seidenader*/

namespace SvGcl = Seidenader::GridCtrlLibrary;
