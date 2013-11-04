//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageGraphROI
//* .File Name       : $Workfile:   SVDlgImageGraphROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:42  $
//******************************************************************************

#pragma once
#ifndef __DLGIMAGEGRAPHROI_H__
#define __DLGIMAGEGRAPHROI_H__

#include "SVDlgImageGraph.h"

class SVDlgImageGraphROI : public SVDlgImageGraphClass
{
public:

	HRESULT SetROI(const CRect &p_roRect);
	HRESULT GetROI(CRect &p_roRect);

protected:
	//{{AFX_MSG(SVDlgImageGraphROI)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateToolFigure();
	void DrawFigure();
	void UpdateDisplay();

	BOOL UpdatePickType( CPoint point );
	LPCTSTR GetPickCursor();

	bool IsCenterHit( CPoint point );
	bool IsLeftEdgeHit( CPoint point );	// Left
	bool IsRightEdgeHit( CPoint point );	// Right
	bool IsTopEdgeHit( CPoint point );	// Top
	bool IsBottomEdgeHit( CPoint point );	// Bottom
	bool IsTopLeftCornerHit( CPoint point );	// Left Top 
	bool IsTopRightCornerHit( CPoint point );	// Right Top
	bool IsBottomLeftCornerHit( CPoint point );	// Left Bottom
	bool IsBottomRightCornerHit( CPoint point );	// Right Bottom

protected:
	enum PickTypeEnum
	{
		PickTypeUnknown = 0,
		PickTypeMove = 1,
		PickTypeResizeRight,
		PickTypeResizeLeft,
		PickTypeResizeBottom,
		PickTypeResizeTop,
		PickTypeResizeBottomRight,
		PickTypeResizeTopRight,
		PickTypeResizeBottomLeft,
		PickTypeResizeTopLeft,

	};

	PickTypeEnum m_ePickType;
	LPCTSTR m_szCursorId;
	CPoint m_ptPickPoint;

	CRect m_oRect;

	//SVExtentClass m_extentView;
	SVDrawObjectClass m_roi;

	void ScaleDisplayToReal(CRect& p_roRect);
	void ScaleRealToDisplay(CRect& p_roRect);

//	SVDoubleValueObjectClass* pExtentLeft;
//	SVDoubleValueObjectClass* pExtentTop;
//	SVDoubleValueObjectClass* pExtentWidth;
//	SVDoubleValueObjectClass* pExtentHeight;


};


#endif	//#ifndef __DLGIMAGEGRAPHROI_H__

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageGraphROI.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:42   bWalter
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
 *    Rev 1.3   21 Jun 2007 12:27:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 May 2005 14:57:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 13:49:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Aug 2003 08:49:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
