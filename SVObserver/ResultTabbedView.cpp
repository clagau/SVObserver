//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A child window with a tab-control for the Result Window.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ResultTabbedView.h"
#include "ResultTableListCtrl.h"
#include "SVIPChildFrm.h"
#include "SVIPDoc.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(ResultTabbedView, CView)

BEGIN_MESSAGE_MAP(ResultTabbedView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, &ResultTabbedView::OnChangingActiveTab)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
ResultTabbedView::ResultTabbedView() :
	m_pIPDoc(nullptr)
{
}

ResultTabbedView::~ResultTabbedView()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL ResultTabbedView::GetParameters(SVObjectWriter& rWriter)
{
	BOOL bOk = TRUE;

	CRect Rect;
	GetWindowRect(Rect);

	_variant_t svVariant;

	svVariant = Rect.Height();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = Rect.Width();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();

	return bOk;
}

BOOL ResultTabbedView::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = FALSE;

	CSize l_Size;

	_variant_t svVariant;

	bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant);
		if (bOk)
		{
			l_Size.cx = svVariant;
		}
	}

	if (bOk)
	{
		SetViewSize(l_Size);
	}

	return bOk;
}// end SetParameters

BOOL ResultTabbedView::CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = FALSE;

	CSize l_Size;

	_variant_t svVariant;

	CRect l_cRect;

	GetWindowRect(l_cRect);

	bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant);
		if (bOk)
		{
			l_Size.cx = svVariant;
		}
	}

	if (bOk)
	{
		if (l_Size.cx != l_cRect.Width() || l_Size.cy != l_cRect.Height())
		{
			SetViewSize(l_Size);

			GetWindowRect(l_cRect);
		}
	}

	return bOk;
}// end CheckParameters

void ResultTabbedView::SetViewSize(CSize &Size)
{
	CWnd *pWnd = this;
	SVIPSplitterFrame *pSplitterFrame = nullptr;

	do
	{
		pWnd = pWnd->GetParent();

		if (nullptr != pWnd)
		{
			pSplitterFrame = dynamic_cast<SVIPSplitterFrame *>(pWnd);
		}
	} while (nullptr == pSplitterFrame && nullptr != pWnd);

	if (nullptr != pSplitterFrame)
	{
		pSplitterFrame->SetViewSize(this, Size);
	}
}
#pragma endregion Public Methods

#pragma region Private Methods
void ResultTabbedView::OnDraw(CDC* pDC)
{
	m_TabCtrl.RedrawWindow();
	switch (m_TabCtrl.GetActiveTab())
	{
	case 0:
		m_ResultList.updateList(m_pIPDoc);
		break;
	case 1:
		m_ResultTableList.updateList(m_pIPDoc);
		break;

	default:
		break;
	}
	__super::OnDraw(pDC);
};

int ResultTabbedView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_pIPDoc = dynamic_cast<SVIPDoc*> (GetDocument());

	CRect Rect;
	GetClientRect(Rect);

	if (!m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D_ROUNDED, Rect, this, 1))
	{
		return -1;
	}

	m_ResultList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, Rect, &m_TabCtrl, 1);
	m_ResultTableList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER , Rect, &m_TabCtrl, 2);

	m_TabCtrl.AddTab(&m_ResultList, _T("Result"));
	m_TabCtrl.AddTab(&m_ResultTableList, _T("Table Result"));
	m_TabCtrl.SetActiveTab(0);

	return 0;
}

void ResultTabbedView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_TabCtrl.m_hWnd))
	{
		CRect Rect;
		GetClientRect(Rect);
		m_TabCtrl.MoveWindow(Rect);
	}
}

BOOL ResultTabbedView::OnEraseBkgnd(CDC* pDC)
{
	//! This avoids unnecessary flicker of the result view
	return true;
}
#pragma endregion Private Methods


afx_msg LRESULT ResultTabbedView::OnChangingActiveTab(WPARAM wParam, LPARAM lParam)
{
	//! wParam contains the active tab index
	switch (wParam)
	{
	case 0:
		m_ResultList.updateList(m_pIPDoc);
		break;
	case 1:
		m_ResultTableList.updateList(m_pIPDoc);
		break;

	default:
		break;
	}

	return 0;
}
