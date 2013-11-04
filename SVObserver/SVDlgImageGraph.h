//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImageGraph
//* .File Name       : $Workfile:   SVDlgImageGraph.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:24  $
//******************************************************************************

#ifndef SVDLGIMAGEGRAPH_H
#define SVDLGIMAGEGRAPH_H

#include "SVDlgImage.h"		// base class
#include "SVDrawObject.h"
#include "SVObjectLibrary/SVObjectCPointArrayClass.h"

enum SVGraphNormalizeModeEnum
{
	SVGraphNormalizeNone,		// None
	SVGraphNormalize1D,			// Normalize x and y separately
	SVGraphNormalize2D,			// Normalize x and y together ( square )
	SVGraphScale,				// Scale X and Y
	SVGraphScaleAndFill			// Scale X and Y and Fill in between
};

struct SVDrawObjectVectorStruct
{
	SVGraphNormalizeModeEnum normalizeMode; // Normalization Mode
	SVDrawObjectClass m_svDrawObject;	// Point set and Draw pen
	SVObjectCPointArrayClass* pPointVector;	// Normalized points
};

typedef SVVector< SVDrawObjectVectorStruct > SVDrawObjectVectorList;

class SVDlgImageGraphClass : public SVDlgImageClass
{
public:
	SVDlgImageGraphClass();

	virtual ~SVDlgImageGraphClass();

protected:
	// Generated message map functions
	//{{AFX_MSG(SVDlgImageGraphClass)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	void init( SVImageClass* pImage );
	void init( const SVImageInfoClass* pImageInfo, SVSmartHandlePointer pImageHandle );

	virtual BOOL AddPoints( SVDrawObjectListClass* PDrawObjectList, SVGraphNormalizeModeEnum mode );
	virtual BOOL AddPoints( const SVDrawObjectClass &p_rsvDrawObject, SVGraphNormalizeModeEnum mode, bool l_bRedraw = true );
	virtual BOOL ClearPoints();

	BOOL Normalize( SVDrawObjectVectorStruct& RDrawObjectVector );

	void Scale( SVDrawObjectVectorStruct& RDrawObjectVector );

	void RefreshPoints();

protected:
	void CopyPoints( const SVCPointArray& rPointArray, SVObjectCPointArrayClass* pPointVector );

	void TransparentStretchBlit( CDC* pDC, CBitmap& rBitmap, COLORREF transparentColor );

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	//{{AFX_VIRTUAL(SVDlgImageGraphClass)
	//}}AFX_VIRTUAL

public:
	CPoint m_ptPosition;				// used to adjust point set
	BOOL m_xorMode;

protected:
	SVDrawObjectVectorList			m_drawList;
	double m_dxMult;
	double m_dyMult;
	long m_lxDisp;
	long m_lyDisp;
	long m_lyClient;

	double m_dxScale;
	double m_dyScale;

	HPEN m_hDefaultPen;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImageGraph.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Feb 2011 14:39:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Nov 2010 14:35:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 09:01:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Jun 2005 08:11:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed forward declaration of SVObjectCPointArrayClass to #include
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 13:49:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   12 Aug 2003 09:01:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ClearPoints function
 * added m_ prefix to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Aug 2003 12:58:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added new init method
 * removed dead parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:09:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 13:40:22   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Change all occurences of SVLongVector to SVObjectDoubleArrayClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Apr 17 2001 15:58:54   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Sep 2000 16:28:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2000 11:53:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in. New Dialog class to display an Image with a graph and figures overlayed on the image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
