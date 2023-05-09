//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "PlcOutputsView.h"
#include "SVIOTabbedView.h"
#include "SVDiscreteInputsView.h"
#include "SVDiscreteOutputsView.h"
#include "SVRemoteInputsView.h"
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

BOOL SVIOTabbedView::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	TVisualObject *pIOTab = new TVisualObject(SVIOViewID, _T("IO"), pContext, RUNTIME_CLASS(TTabWnd));

	TVisualObject* pDiscreteInputsView = new TVisualObject(SVIODiscreteInputsViewID, _T("Discrete Inputs"), pContext, RUNTIME_CLASS(SVDiscreteInputsView));
	TVisualObject* pDiscreteOutputsView = new TVisualObject(SVIODiscreteOutputsViewID, _T("Discrete Outputs"), pContext, RUNTIME_CLASS(SVDiscreteOutputsView));
	TVisualObject* pPlcOutputsView = new TVisualObject(SVIOPlcOutputsViewID, _T("PLC Outputs"), pContext, RUNTIME_CLASS(PlcOutputsView));

	TVisualObject* pRemoteInputsView = new TVisualObject(SVIORemoteInputsViewID, _T("Remote Inputs"), pContext, RUNTIME_CLASS(SVRemoteInputsView));
	TVisualObject* pRemoteMonitorListView = new TVisualObject(SVRemoteMonitorListViewID, _T("Monitor List"), pContext, RUNTIME_CLASS(MonitorListView));
	TVisualObject* pGlobalConstantView = new TVisualObject(SVGlobalConstantViewID, _T("Global Constants"), pContext, RUNTIME_CLASS(GlobalConstantView));

	m_Framework.Add(pIOTab);
	m_Framework.Add(pIOTab, pDiscreteInputsView);
	m_Framework.Add(pIOTab, pRemoteInputsView);

	m_Framework.Add(pIOTab, pDiscreteOutputsView);
	m_Framework.Add(pIOTab, pPlcOutputsView);

	m_Framework.Add(pIOTab, pRemoteMonitorListView);
	m_Framework.Add(pIOTab, pGlobalConstantView);

	BOOL l_bRet = m_Framework.Create(this);

	return l_bRet;
}

// SVIOTabbedView message handlers
void SVIOTabbedView::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
	m_Framework.Destroy();
}

