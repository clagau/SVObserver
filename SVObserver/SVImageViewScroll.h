//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageViewScroll
//* .File Name       : $Workfile:   SVImageViewScroll.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Sep 2014 12:14:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

#pragma region Declarations
class SVImageViewClass;
class SVObjectWriter;
#pragma endregion Declarations

class SVImageViewScroll : public CScrollView
{
	DECLARE_DYNCREATE( SVImageViewScroll )

	//{{AFX_VIRTUAL(SVImageViewScroll)
	public:
	virtual void OnInitialUpdate() override;
	virtual BOOL Create(LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DWStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext = nullptr) override;
	virtual void OnDraw(CDC* pDC) override;
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

public:
	SVImageViewScroll();
	virtual ~SVImageViewScroll();

	void GetParameters(SVObjectWriter& rWriter);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	bool CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	void SetViewSize( CSize &p_rcSize );
	void SetImageSize( SIZE p_oSize );

	//************************************
	// Method:    ImageIsEmpty
	// Description:  Return true if image is empty
	// Returns:   bool
	//************************************
	bool ImageIsEmpty() const;

	//************************************
	// Method:    IsZoomAllowed
	//  Description:  Return true if zooming is enabled
	// Returns:   bool
	//************************************
	bool IsZoomAllowed() const;

	//************************************
	// Method:    UpdateZoomToolbar
	// Description:  Updates the zoom toolbar but only when the view has the focus
	// Returns:   void
	//************************************
	void UpdateZoomToolbar();

protected:
	//{{AFX_MSG(SVImageViewScroll)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd( CDC* p_pDC );
	afx_msg void OnZoomMinus();
	afx_msg void OnUpdateZoomMinus(CCmdUI *pCmdUI);
	afx_msg void OnZoomPlus();
	afx_msg void OnUpdateZoomPlus(CCmdUI *pCmdUI);
	afx_msg void OnZoomFit();
	afx_msg void OnUpdateZoomFit(CCmdUI *pCmdUI);
	afx_msg void OnZoomOne();
	afx_msg void OnUpdateZoomOne(CCmdUI *pCmdUI);
	afx_msg void OnZoomSliderMoved();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SVImageViewClass* m_pView;

	CPoint m_oOldMaxScrollPoint;
	CPoint m_oOldScrollPoint;
	HBRUSH m_hWindowBackgroundColor;

	SIZE m_SizeTotal;
};

