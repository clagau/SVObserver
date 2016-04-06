//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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
#include "SVOGui\SVDlgImage.h"
#include "SVFigureEditor.h"
#include "SVDrawObject.h"
#pragma endregion Includes

class SVDrawObjectListClass;

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
		SVDrawObjectClass         svDrawObject; // Original Point set and Draw pen
		std::vector<CPoint>       vecPoints;    // Normalized points
	};

	typedef std::vector< OverlayStruct > SVOverlayStructVectorType;


	SVDlgImageEditor();
	virtual ~SVDlgImageEditor();

	HRESULT SetFigureEditor( SVFigureEditor* pFigureEditor);
	//HRESULT SetROI(const CRect& p_roRect);
	//HRESULT GetROI(CRect& p_roRect);
	CPoint GetMouseCoordinates();	// in image coordinates, not screen

	HRESULT ClearOverlayPoints();
	HRESULT AddOverlayPoints( SVDrawObjectListClass* pDrawObjectList, SVDlgImageOverlayOptions options );
	HRESULT AddOverlayPoints( SVDrawObjectClass& rsvDrawObject, SVDlgImageOverlayOptions options );

protected:
	virtual void OnPaintOverlay(CPaintDC& dc);
	//{{AFX_MSG(SVDlgImageEditor)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSetZoom();


	LPCTSTR GetPickCursor( UINT nHitTest );
	CPoint TranslateClientCoordinatesToImageCoordinates(CPoint point);

	void Normalize( OverlayStruct& rOverlay );
	void CopyOverlayPoints( const SVCPointArray& rsvaPoints, std::vector<CPoint>& rvecPoints );

	void DrawOverlay(CPaintDC& dc);

protected:

	UINT m_uiHitTest;
	LPCTSTR m_szCursorId;
	CPoint m_ptPickPoint;
	CPoint m_ptLastMousePosition;

	//void ScaleDisplayToReal(CRect& p_rRect);
	//void ScaleRealToDisplay(CRect& p_rRect);

	SVFigureEditor* m_pFigureEditor;

	//std::vector < std::vector< CPoint > > m_vecFixedOverlayPoints;
	SVOverlayStructVectorType  m_drawList;

	HPEN m_hDefaultPen;

}; // end class SVDlgImageEditor

