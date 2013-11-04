//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShapeFigureEditors
//* .File Name       : $Workfile:   SVMaskShapeFigureEditors.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:25:12  $
//******************************************************************************

#ifndef SVMASKSHAPEFIGUREEDITORS_H_
#define SVMASKSHAPEFIGUREEDITORS_H_
#pragma once

#include "SVFigureEditor.h"
#include "SVMaskShape.h"

class SVMaskShapeFigureEditor : public SVFigureEditor
{
public:
	HRESULT SetShape( SVMaskShape* pShape );
	virtual HRESULT PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay );
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual CSize GetSizeImpl();
	virtual CPoint GetReferencePoint();
	virtual CRect GetRect();

	CSize CalculateDeltaPos(CPoint point);

	SVMaskShape* m_pMaskShape;
	SVMaskShapeFigureEditor();	// don't create base class
};

class SVMaskShapeRectangleFigureEditor : public SVMaskShapeFigureEditor
{
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display

};

class SVMaskShapeOvalFigureEditor : public SVMaskShapeFigureEditor
{
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display

};

class SVMaskShapeDoughnutFigureEditor : public SVMaskShapeFigureEditor
{
public:
	enum
	{
		SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE = 0x0100,
		
		HTSVINNERTOP = HTTOP | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERBOTTOM = HTBOTTOM | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERLEFT = HTLEFT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERRIGHT = HTRIGHT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERTOPLEFT = HTTOPLEFT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERBOTTOMLEFT = HTBOTTOMLEFT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERTOPRIGHT = HTTOPRIGHT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,
		HTSVINNERBOTTOMRIGHT = HTBOTTOMRIGHT | SV_MASK_SHAPE_DOUGHNUT_INNER_EDGE,

		SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE = 0x0200,

		HTSVOUTERTOP = HTTOP | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERBOTTOM = HTBOTTOM | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERLEFT = HTLEFT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERRIGHT = HTRIGHT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERTOPLEFT = HTTOPLEFT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERBOTTOMLEFT = HTBOTTOMLEFT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERTOPRIGHT = HTTOPRIGHT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
		HTSVOUTERBOTTOMRIGHT = HTBOTTOMRIGHT | SV_MASK_SHAPE_DOUGHNUT_OUTER_EDGE,
	};
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display
	virtual CRect GetRect();
};

class SVMaskShapeSymmetricTrapezoidFigureEditor : public SVMaskShapeFigureEditor
{
public:
	enum
	{
		SV_MASK_SHAPE_SYMMETRIC_TRAPEZOID_OFFSET = 0x0300,
	};
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display
};

#endif	//#ifndef SVMASKSHAPEFIGUREEDITORS_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMaskShapeFigureEditors.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:25:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:28:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/