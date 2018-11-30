//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROIFigureEditor
//* .File Name       : $Workfile:   SVROIFigureEditor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:01:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageInfoClass.h"

#include "SVFigureEditor.h"
#pragma endregion Includes

// MODELLED AFTER THE MASK SHAPE SYSTEM

// performs the drawing logic
class SVROIShape
{
public:
	HRESULT DrawOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb, COLORREF rgbInner = -1 );
	CRect GetRect();
	void  SetRect( const CRect& rect );
	CRect GetParentImageRect();
	HRESULT SetImageInfo( const SVImageInfoClass& svImageInfo );
	HRESULT GetImageInfo( SVImageInfoClass& svImageInfo ) const;

protected:

	HRESULT TranslateToDisplay( CRect rectViewport, CRect rectDisplay, CRect& rectShape );

	CRect m_Rect;
	SVImageInfoClass m_svImageInfo;

	bool m_bImageInfoChanged;
	bool m_bImageInfoSet;

	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect);
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVPoint<double>& rPoint);
};

// performs the resizing logic
class SVROIFigureEditor : public SVFigureEditor
{
public:
	SVROIFigureEditor();
	virtual ~SVROIFigureEditor();
	virtual HRESULT PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay ) override;

	// provide an interface to the underlying shape object
	virtual CRect GetRect() override;	// in SVFigureEditor
	void  SetRect( const CRect& rect );

	HRESULT SetImageInfo( const SVImageInfoClass& svImageInfo );
	HRESULT GetImageInfo( SVImageInfoClass& svImageInfo ) const;

	CRect GetParentImageRect();

protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point) override;
	virtual CSize GetSizeImpl() override;
	virtual CPoint GetReferencePoint() override;
	virtual CSize GetHotSpotSize() override;

	virtual SVHitTestStruct HitTest( CPoint point ) override;	// for cursor display

	CSize CalculateDeltaPos(CPoint point);

	SVROIShape* m_pShape;
};

