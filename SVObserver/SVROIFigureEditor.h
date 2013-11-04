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

#ifndef SVROIFIGUREEDITOR_H_
#define SVROIFIGUREEDITOR_H_
#pragma once

#include "SVImageLibrary/SVImageInfoClass.h"

#include "SVFigureEditor.h"

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
//	HRESULT Refresh();
protected:

	HRESULT TranslateToDisplay( CRect rectViewport, CRect rectDisplay, CRect& rectShape );

	// always render on supplied DC, don't need these
	//SVDIBITMAPINFO m_dib;
	//CDC m_RenderDC;

	CRect m_Rect;
	SVImageInfoClass m_svImageInfo;

	bool m_bImageInfoChanged;
	bool m_bImageInfoSet;

	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect);
	static HRESULT TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentPointStruct& rPoint);
};

// performs the resizing logic
class SVROIFigureEditor : public SVFigureEditor
{
public:
	SVROIFigureEditor();
	virtual ~SVROIFigureEditor();
	virtual HRESULT PaintOverlay( CDC& dc, CRect rectViewport, CRect rectDisplay );

	// provide an interface to the underlying shape object
	//*
	virtual CRect GetRect();	// in SVFigureEditor
	void  SetRect( const CRect& rect );
	//HRESULT Refresh();

	HRESULT SetImageInfo( const SVImageInfoClass& svImageInfo );
	HRESULT GetImageInfo( SVImageInfoClass& svImageInfo ) const;

	CRect GetParentImageRect();
	//*/

protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual CSize GetSizeImpl();
	virtual CPoint GetReferencePoint();
	virtual CSize GetHotSpotSize();

	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display

	CSize CalculateDeltaPos(CPoint point);


	SVROIShape* m_pShape;
};

/*
class SVMaskShapeRectangleFigureEditor : public SVMaskShapeFigureEditor
{
protected:
	virtual HRESULT MouseMove(UINT nFlags, CPoint point);
	virtual SVHitTestStruct HitTest( CPoint point );	// for cursor display

};
*/


#endif	//#ifndef SVMASKSHAPEFIGUREEDITORS_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVROIFigureEditor.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:01:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 17:33:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Jul 2007 07:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2006 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Aug 2006 11:25:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
