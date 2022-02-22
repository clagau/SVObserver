//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SplitterWnd
//* .File Name       : $Workfile:   FramedSplitterWnd.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Sep 2014 12:10:06  $
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

FramedSplitterWnd::~FramedSplitterWnd()
{
}

#pragma endregion Constructor

#pragma region Protected Methods
void FramedSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	if( pDC && splitBorder == nType )
	{
		CRect viewRect;
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

		bool bDrawBlueFrame = false;
		CMDIChildWnd *pChild = nullptr;
		if(pFrame)
		{
			pChild = dynamic_cast<CMDIChildWnd*>(pFrame->GetActiveFrame());
		}

		CView* pView = nullptr;
		if(nullptr != pChild)
		{
			pView = pChild->GetActiveView();
		}

		SVImageViewScroll* pScrollView = dynamic_cast<SVImageViewScroll*>(pView);
		if(nullptr == pScrollView )
		{
			pScrollView = dynamic_cast<SVImageViewScroll*>(pView->GetParent());
		}

		if(nullptr != pScrollView)
		{
			pScrollView->GetWindowRect(&viewRect);
			bDrawBlueFrame = pScrollView->IsZoomAllowed();
		}

		if(bDrawBlueFrame)
		{
			CRect rectArgScreen= rectArg;
			ClientToScreen(rectArgScreen);
			bDrawBlueFrame = 	( rectArgScreen.CenterPoint() == viewRect.CenterPoint() 
								&& abs(rectArgScreen.Size().cx - viewRect.Size().cx) <= 6 
								&& abs(rectArgScreen.Size().cy - viewRect.Size().cy) <= 6 );

			if(bDrawBlueFrame)
			{
				CRect rect = rectArg;
				const int dx = GetSystemMetrics(SM_CXBORDER);
				const int dy = GetSystemMetrics(SM_CYBORDER);
				const COLORREF blue( RGB(10, 10, 255) );
				pDC->Draw3dRect(rect, blue, blue);
				rect.InflateRect(-dx, -dy);
				pDC->Draw3dRect(rect, blue, blue);
				return;
			}
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

