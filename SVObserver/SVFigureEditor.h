//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFigureEditor
//* .File Name       : $Workfile:   SVFigureEditor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:33:30  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

class SVFigureEditor
{
public:
	virtual ~SVFigureEditor();	// this is a base class

	bool IsCaptured();

	HRESULT OnMouseMove(UINT nFlags, CPoint point);
	HRESULT OnLButtonDown(UINT nFlags, CPoint point);
	HRESULT OnLButtonUp(UINT nFlags, CPoint point);
	UINT OnHitTest( CPoint point );	// visual hit test

	HRESULT Draw( HDC hDC ) const;
	virtual HRESULT PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay );

	HRESULT SetOverlayColor( COLORREF rgbOutlineColor );
	HRESULT SetOverlayColor( COLORREF rgbOuterOutlineColor, COLORREF rgbInnerOutlineColor );

	CSize GetSize();

protected:
	SVFigureEditor();	// don't create instances of base class

	static int f_iHotSpotSizeX;  // size is twice this
	static int f_iHotSpotSizeY;  // size is twice this

	struct SVHitTestStruct
	{
		UINT nHitTest;	// lower byte contains cursor (visual) hit test (#defined in <winuser.h>), upper bytes contain additional specification
		UINT CursorHT() {return nHitTest & 0xFF;}

		SVHitTestStruct() {nHitTest = HTNOWHERE;}
		SVHitTestStruct(const SVHitTestStruct& rhs) {nHitTest = rhs.nHitTest;}
		explicit SVHitTestStruct(UINT p_nHitTest) { nHitTest = p_nHitTest; }
		const SVHitTestStruct& operator = (UINT p_nHitTest) {nHitTest = p_nHitTest; return *this;}
	};

	virtual HRESULT MouseMove(UINT nFlags, CPoint point) = 0; // this will only be called if the shape has the capture
	virtual SVHitTestStruct HitTest( CPoint point ) = 0;
	virtual CSize   GetSizeImpl() = 0;
	virtual CPoint GetReferencePoint() = 0;
	virtual CRect GetRect() = 0;
	virtual CSize GetHotSpotSize();

	HRESULT LButtonDown(UINT nFlags, CPoint point);
	HRESULT LButtonUp(UINT nFlags, CPoint point);

	CRect GetHotSpot( CPoint pt );
	std::shared_ptr<CRgn> GetHotSpot( CPoint pt1, CPoint pt2 );

	SVHitTestStruct m_HitType;	// use this for actual hit type ( can have different hit points for the same cursor )
	CPoint m_ptMouseDown;	// original mouse press location
	CPoint m_ptReference;	// original reference point
	CSize  m_sizeMouseDelta;	// difference between original reference point and mouse press location
	CRect m_rectOriginal;
	UINT m_uiMouseFlags;
	bool m_bCapture;

	COLORREF m_rgbOuterOutlineColor;
	COLORREF m_rgbInnerOutlineColor;
	bool m_bUseInnerOutline;
};

