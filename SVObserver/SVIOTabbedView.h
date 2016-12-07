//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOTabbedView
//* .File Name       : $Workfile:   SVIOTabbedView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Apr 2014 17:00:30  $
//******************************************************************************

#pragma once

#include "SVMFCControls\VisualFx.h"

// SVIOTabbedView frame
enum SVTabbedViewSplitterIDEnum
{
	SVIOViewID = 1,
	SVIODiscreteInputsViewID = 2,
	SVIORemoteInputsViewID = 3,
	SVIODiscreteOutputsViewID = 5,
	SVRemoteOutputsViewID = 8,
	SVRemoteMonitorListViewID = 9,
	SVGlobalConstantViewID = 10,
};

class SVIOTabbedView : public CMDIChildWnd
{
	DECLARE_DYNCREATE(SVIOTabbedView)
protected:
	SVIOTabbedView();           // protected constructor used by dynamic creation
	virtual ~SVIOTabbedView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	TVisualFramework m_Framework;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
	CWnd* GetActivePane(int* /*pRow*/, int* /*pCol*/);

	afx_msg void OnDestroy();
};

