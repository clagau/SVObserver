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
#include <algorithm>
#include "HLB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int margin_padding = 6;

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

LRESULT SVHorizListBox::OnAddString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (LB_ERR != lResult && LB_ERRSPACE != lResult)
	{
		CalcNewHorizExtent(reinterpret_cast<LPCTSTR>(lParam));
	}
	return lResult;
}

LRESULT SVHorizListBox::OnInsertString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (LB_ERR != lResult && LB_ERRSPACE != lResult)
	{
		CalcNewHorizExtent(reinterpret_cast<LPCTSTR>(lParam));
	}
	return lResult;
}

LRESULT SVHorizListBox::OnDeleteString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (LB_ERR != lResult && LB_ERRSPACE != lResult)
	{
		ResetHorizExtent();
	}
	return lResult;
}

LRESULT SVHorizListBox::OnSetTabStops(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (LB_ERR != lResult && LB_ERRSPACE != lResult)
	{
		ResetHorizExtent();
	}
	return lResult;
}

LRESULT SVHorizListBox::OnResetContent(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	SetHorizontalExtent(0);
	return lResult;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Hlb.cpp_v  $
 * 
 *    Rev 1.2   15 Aug 2014 15:21:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to calculate the exent for the list box horizontal scroll bar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 11:54:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:13:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 15:51:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2001 22:07:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used to diagnose issues with Coreco Acquisition process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/