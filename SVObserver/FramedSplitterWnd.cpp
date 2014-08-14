//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SplitterWnd
//* .File Name       : $Workfile:   FramedSplitterWnd.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:23:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "FramedSplitterWnd.h"
#include "SVImageViewScroll.h"
#include "SVImageView.h"
#include "SVMainFrm.h"
#pragma endregion Includes

#pragma region Constructor
FramedSplitterWnd::FramedSplitterWnd()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void FramedSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	if( pDC && nType == splitBorder )
	{
		CRect viewRect;
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

		CMDIChildWnd *pChild = nullptr;
		if(pFrame)
		{
			pChild = dynamic_cast<CMDIChildWnd*>(pFrame->GetActiveFrame());
		}

		CView* pView = nullptr;
		if(pChild)
		{
			pView = pChild->GetActiveView();
		}

		if (pView != NULL && pView->IsKindOf(RUNTIME_CLASS(SVImageViewScroll)))
		{
			pView->GetWindowRect(&viewRect);
		}
		else if (pView != NULL && pView->IsKindOf(RUNTIME_CLASS(SVImageViewClass)))
		{
			SVImageViewScroll* psvScroll = dynamic_cast< SVImageViewScroll* >( pView->GetParent() );
			if(psvScroll)
			{
				psvScroll->GetWindowRect(&viewRect);
			}
		}
		else
		{
			return CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
		}

		CRect rect = rectArg;
		CRect rectArgScreen= rectArg;
		ClientToScreen(rectArgScreen);

		if( rectArgScreen.CenterPoint() == viewRect.CenterPoint() 
			&& abs(rectArgScreen.Size().cx - viewRect.Size().cx) <= 6 
			&& abs(rectArgScreen.Size().cy - viewRect.Size().cy) <= 6 )
		{
			const int dx = GetSystemMetrics(SM_CXBORDER);
			const int dy = GetSystemMetrics(SM_CYBORDER);
			const COLORREF blue( RGB(10, 10, 255) );
			pDC->Draw3dRect(rect, blue, blue);
			rect.InflateRect(-dx, -dy);
			pDC->Draw3dRect(rect, blue, blue);
			rect.InflateRect(-dx, -dy);
			return;
		}
	}

	CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
}

void FramedSplitterWnd::RefreshSplitBars()
{
	CRect rectInside;
	GetInsideRect(rectInside);
	DrawAllSplitBars(nullptr, rectInside.right, rectInside.bottom);
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FramedSplitterWnd.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 13:23:06   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/