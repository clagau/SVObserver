//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOTabbedView
//* .File Name       : $Workfile:   SVIOTabbedView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   17 Apr 2014 17:00:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVIOTabbedView.h"
#include "SVDiscreteOutputsView.h"
#include "SVDiscreteInputsView.h"

#include "SVRemoteInputsView.h"
#include "SVRemoteOutputsView.h"
#include "MonitorListView.h"
#include "GlobalConstantView.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVIOTabbedView, CMDIChildWnd)

SVIOTabbedView::SVIOTabbedView()
{
}

SVIOTabbedView::~SVIOTabbedView()
{
}

BEGIN_MESSAGE_MAP(SVIOTabbedView, CMDIChildWnd)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL SVIOTabbedView::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	TVisualObject *pIOTab = new TVisualObject( SVIOViewID, _T("IO"), pContext, RUNTIME_CLASS(TTabWnd) ); //

	TVisualObject *pInputsView = new TVisualObject( SVIODiscreteInputsViewID, _T("Discrete Inputs"), pContext, RUNTIME_CLASS( SVDiscreteInputsView ) );
	TVisualObject *pRemoteInputsView = new TVisualObject( SVIORemoteInputsViewID, _T("Remote Inputs"), pContext, RUNTIME_CLASS( SVRemoteInputsView ) );

	TVisualObject *pOutputsView = new TVisualObject( SVIODiscreteOutputsViewID, _T("Discrete Outputs"), pContext, RUNTIME_CLASS( SVDiscreteOutputsView ) );

	TVisualObject *pRemoteOutputsView = new TVisualObject( SVRemoteOutputsViewID, _T("Remote Outputs"), pContext, RUNTIME_CLASS( SVRemoteOutputsView ) );
	TVisualObject *pRemoteMonitorListView = new TVisualObject( SVRemoteMonitorListViewID, _T("Monitor List"), pContext, RUNTIME_CLASS( MonitorListView ) );
	TVisualObject *pGlobalConstantView = new TVisualObject( SVGlobalConstantViewID, _T("Global Constants"), pContext, RUNTIME_CLASS( GlobalConstantView ) );

	m_Framework.Add( pIOTab );
	m_Framework.Add( pIOTab, pInputsView );
	m_Framework.Add( pIOTab, pRemoteInputsView );

	m_Framework.Add( pIOTab, pOutputsView );

	m_Framework.Add( pIOTab, pRemoteOutputsView );
	m_Framework.Add( pIOTab, pRemoteMonitorListView );
	m_Framework.Add( pIOTab, pGlobalConstantView );

	BOOL l_bRet = m_Framework.Create(this);

	return l_bRet;
}

CWnd* SVIOTabbedView::GetActivePane(int* /*pRow*/, int* /*pCol*/)
{
	ASSERT_VALID(this);

	// attempt to use active view of frame window
	CWnd* pView = nullptr;
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT_VALID(pFrameWnd);
	pView = pFrameWnd->GetActiveView();

	// failing that, use the current focus
	if (nullptr == pView)
	{
		pView = GetFocus();
	}
	return pView;
}

// SVIOTabbedView message handlers
void SVIOTabbedView::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
	m_Framework.Destroy();
}

