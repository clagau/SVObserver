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

#ifndef SVDLGIMAGESHAPE_H_
#define SVDLGIMAGESHAPE_H_
#pragma once

#include "SVOGui\SVDlgImage.h"
#include "SVFigureEditor.h"
#include "SVDrawObject.h"

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
	~SVDlgImageEditor();

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


#endif //#ifndef SVDLGIMAGESHAPE_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageEditor.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:13:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Jun 2007 10:38:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2007 12:27:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Aug 2006 15:48:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2006 16:03:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:56:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
