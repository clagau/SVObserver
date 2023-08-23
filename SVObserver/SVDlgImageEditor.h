//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageGraphShape
//* .File Name       : $Workfile:   SVDlgImageEditor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:13:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDrawObject.h"
#include "SVOGui/SVDlgImage.h"
#include "SVOGuiUtility/SVFigureEditor.h"
#pragma endregion Includes

struct SVDlgImageOverlayOptions
{
	bool bFixed;
	bool bFullClient;
	bool bFlipVertical;
	bool bNormalizeY_ROI;
	bool bScaleX;
	bool bScaleY;
	CRect rectMaxXY;
	CSize sizeROI;
	SVDlgImageOverlayOptions() : bFixed(true), bFullClient(true), bFlipVertical(false), bNormalizeY_ROI(false), bScaleX(true), bScaleY(true) {}
};

class SVDlgImageEditor : public SvOg::SVDlgImageClass
{
public:
	struct OverlayStruct
	{
		SVDlgImageOverlayOptions  options;      // 
		SVDrawObject  svDrawObject; // Original Point set and Draw pen
		std::vector<POINT>       vecPoints;    // Normalized points
	};

	typedef std::vector<OverlayStruct> SVOverlayStructVectorType;


	SVDlgImageEditor();
	virtual ~SVDlgImageEditor();

	HRESULT SetFigureEditor( SVFigureEditor* pFigureEditor);
	CPoint GetMouseCoordinates();	// in image coordinates, not screen

	HRESULT ClearOverlayPoints();
	HRESULT AddOverlayPoints(const SVDrawObjectList& rDrawObjectList, SVDlgImageOverlayOptions options );
	HRESULT AddOverlayPoints(const SVDrawObject& rDrawObject, SVDlgImageOverlayOptions options );

protected:
	virtual void OnPaintOverlay(CPaintDC& dc) override;
	//{{AFX_MSG(SVDlgImageEditor)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSetZoom() override;


	LPCTSTR GetPickCursor( UINT nHitTest );
	CPoint TranslateClientCoordinatesToImageCoordinates(CPoint point);

	void Normalize( OverlayStruct& rOverlay );

	void DrawOverlay(CPaintDC& dc);

protected:

	UINT m_uiHitTest;
	LPCTSTR m_szCursorId;
	CPoint m_ptPickPoint;
	CPoint m_ptLastMousePosition;
	SVFigureEditor* m_pFigureEditor;
	SVOverlayStructVectorType  m_drawList;

	HPEN m_hDefaultPen;
}; // end class SVDlgImageEditor

