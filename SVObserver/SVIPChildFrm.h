//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPSplitterFrame
//* .File Name       : $Workfile:   SVIPChildFrm.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 Aug 2014 16:03:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "FramedSplitterWnd.h"
#pragma endregion Includes

#pragma region Declarations
class SVIPDoc;
#pragma endregion Declarations

class SVIPSplitterFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE( SVIPSplitterFrame )
protected:
	// App Wizard Generated
	//{{AFX_MSG(SVIPSplitterFrame)
	afx_msg LRESULT OnUpdateAllData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateNextView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVIPSplitterFrame();	// constructor
	virtual ~SVIPSplitterFrame();

	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* PContext ) override;

public:
	void SetDefaultPaneSizes( RECT &BoundingRect );

	//{{AFX_VIRTUAL(SVIPSplitterFrame)
	public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs ) override;
	//}}AFX_VIRTUAL

	void SetViewSize( CWnd *l_pView, CSize &p_rViewSize );

	void RefreshAllSplitters();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// See comments below for a picture of how these splitters look
	FramedSplitterWnd m_oWndSplitter1;
	FramedSplitterWnd m_oWndSplitter2;
	FramedSplitterWnd m_oWndSplitter3;
	FramedSplitterWnd m_oWndSplitter4;
	FramedSplitterWnd m_oWndSplitter5;
	FramedSplitterWnd m_oWndSplitter6;

private:
	SVIPDoc* m_pIPDoc;
};

//  Below is the picture of how the splitters connect
//  The numbers indicate which splitter goes where
//  by default only the left pane of 4 will be maximized
//
//  *****************************************************************************
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       23333333333333333333333333333333333333333333333333333333333333333333*
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       23333333333333333333333333333333333333333333333333333333333333333333*
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *111111111111111111111111111111111111111111111111111111111111111111111111111*
//  *                                                                           *
//  *                                                                           *
//  *                                                                           *
//  *                                                                           *
//  *****************************************************************************
//

