//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HLB
//* .File Name       : $Workfile:   Hlb.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 Aug 2014 15:21:14  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "SVHorizListBox.h"

//undefine max to be able to use std::max instead of a macro
#undef max

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	constexpr int margin_padding = 6;

	SVHorizListBox::SVHorizListBox()
	{
	}

	SVHorizListBox::~SVHorizListBox()
	{
	}

	BEGIN_MESSAGE_MAP(SVHorizListBox, CListBox)
		//{{AFX_MSG_MAP(SVHorizListBox)
		//}}AFX_MSG_MAP
		ON_MESSAGE(LB_ADDSTRING, OnAddString)
		ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
		ON_MESSAGE(LB_DELETESTRING, OnDeleteString)
		ON_MESSAGE(LB_SETTABSTOPS, OnSetTabStops)
		ON_MESSAGE(LB_RESETCONTENT, OnResetContent)
	END_MESSAGE_MAP()

	int SVHorizListBox::CalcHorizExtent(CDC* pDC, LPCTSTR text) const
	{
		CSize size;
		if (0 == (GetStyle() & LBS_USETABSTOPS))
		{
			size = pDC->GetTextExtent(text, static_cast<int>(_tcslen(text)));
		}
		else
		{
			// Expand tabs as well
			size = pDC->GetTabbedTextExtent(text, static_cast<int>(_tcslen(text)), 0, nullptr);
		}
		size.cx += margin_padding;
		return size.cx;
	}

	void SVHorizListBox::ResetHorizExtent()
	{
		if (!GetCount())
		{
			SetHorizontalExtent(0);
		}
		else
		{
			CWaitCursor waitCursor;
			CDC *pDC = GetDC();
			ASSERT(pDC);

			CFont* pOldFont = pDC->SelectObject(GetFont());

			int extent = 0;
			for (int i = 0; i < GetCount(); i++)
			{
				CString text;
				GetText(i, text);
				extent = std::max(extent, CalcHorizExtent(pDC, text));
			}
			pDC->SelectObject(pOldFont);
			ReleaseDC(pDC);
			SetHorizontalExtent(extent);
		}
	}

	void SVHorizListBox::CalcNewHorizExtent(LPCTSTR text)
	{
		CDC *pDC = GetDC();
		CFont* pOldFont = pDC->SelectObject(GetFont());
		int extent = CalcHorizExtent(pDC, text);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		if (extent > GetHorizontalExtent())
		{
			SetHorizontalExtent(extent);
		}
	}

	LRESULT SVHorizListBox::OnAddString(WPARAM , LPARAM lParam)
	{
		LRESULT lResult = Default();
		if (LB_ERR != lResult && LB_ERRSPACE != lResult)
		{
			CalcNewHorizExtent(reinterpret_cast<LPCTSTR>(lParam));
		}
		return lResult;
	}

	LRESULT SVHorizListBox::OnInsertString(WPARAM , LPARAM lParam)
	{
		LRESULT lResult = Default();
		if (LB_ERR != lResult && LB_ERRSPACE != lResult)
		{
			CalcNewHorizExtent(reinterpret_cast<LPCTSTR>(lParam));
		}
		return lResult;
	}

	LRESULT SVHorizListBox::OnDeleteString(WPARAM , LPARAM )
	{
		LRESULT lResult = Default();
		if (LB_ERR != lResult && LB_ERRSPACE != lResult)
		{
			ResetHorizExtent();
		}
		return lResult;
	}

	LRESULT SVHorizListBox::OnSetTabStops(WPARAM , LPARAM )
	{
		LRESULT lResult = Default();
		if (LB_ERR != lResult && LB_ERRSPACE != lResult)
		{
			ResetHorizExtent();
		}
		return lResult;
	}

	LRESULT SVHorizListBox::OnResetContent(WPARAM , LPARAM )
	{
		LRESULT lResult = Default();
		SetHorizontalExtent(0);
		return lResult;
	}
} //namespace SvMc
