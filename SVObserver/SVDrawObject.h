//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDrawObject
//* .File Name       : $Workfile:   SVDrawObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 Jan 2014 11:32:46  $
//******************************************************************************

#ifndef SVDRAWOBJECT_H
#define SVDRAWOBJECT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVExtentFigureStruct.h"
#include "SVImageLibrary/SVExtentLineStruct.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVImageLibrary/SVDrawContext.h"

//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

// Calculates a new set of points (PDestinyPointVector) rotated by the degrees specified in "RotationAngle"
// around "RRotationCenter" and then scales the output points by a factor specified by "lZoomFactor". 
// lZoomFactor may have values from -16 ( 1/16th scale) to 16 (16 times scale), except 0.
BOOL SVCalculateRotation( POINT* PSourcePointVector, POINT* PDestinyPointVector, 
                          int Count, const POINT& RRotationCenter, double RotationAngle, 
													long lZoomFactor = 0 );

// Calculates a new point rotated by the degrees specified in "RotationAngle" around
// "RRotationCenter" and then scales the output points by a factor specified by "lZoomFactor". 
// lZoomFactor may have values from -16 ( 1/16th scale) to 16 (16 times scale), except 0.
POINT SVCalculateRotation( const POINT& RSourcePoint, const POINT& RRotationCenter, 
                           double RotationAngle, long lZoomFactor = 0 );


typedef SVVector< CPoint > SVCPointArray;


class SVDrawObjectClass
{
public:
	SVDrawObjectClass();
	SVDrawObjectClass( const SVDrawObjectClass &p_rsvObject );
	virtual ~SVDrawObjectClass();

	const SVDrawObjectClass &operator=( const SVDrawObjectClass &p_rsvObject );

	virtual void AddExtentLineData( SVExtentLineStruct p_svLine, int PenStyle = PS_SOLID );
	virtual void AddExtentFigureData( SVExtentFigureStruct p_svFigure );

	virtual BOOL Draw( SVDrawContext* PDrawContext );
	virtual BOOL DrawHatch( SVDrawContext* PDrawContext, SVCPointArray& Last );

	// If BUseThisPen == FALSE, the current selected Pen 
	// of the DC will be used to Draw.
	BOOL SetDrawPen( BOOL BUseThisPen = FALSE, int PenStyle = PS_SOLID, int PenWidth = 1, COLORREF PenColor = RGB( 255, 255, 255 ) );

	void Transform( SVDrawContext* PDrawContext );

	void ClearObject();
	int AddPoint( const POINT p_oPoint );
	int AddPoint( const CPoint& RPoint );
	void SetPointAtGrow( int Index, const CPoint& RPoint );
	void SetPointAtGrow( int Index, POINT p_oPoint );
	void SetListSize( int NewSize );
	int GetListSize();
 
	CPoint GetPointAt( int Index );
	CPoint GetCalcPointAt( int Index );

	// SEJ April 17 2000
	const SVCPointArray& GetPointArray();
	
	// SEJ April 17 2000
	HGDIOBJ GetDrawPen();
	COLORREF GetPenColor() const;

	// Copy another CPoint Array ( Type: SVCPointArray ) into this draw object.
	void CopyList( const SVCPointArray& RSrc );

	void SetDrawSinglePoints( bool p_bDrawSinglePoints );
	bool DrawSinglePoints() const;

protected:

	BOOL beginDraw( SVDrawContext* PDrawContext );
	void endDraw( HDC DC );

	
	SVCPointArray	 points;
	SVCPointArray	 calcPoints;

	HGDIOBJ drawPen;	// If NULL the current selected pen of the DC
						// will be used!

	HGDIOBJ oldPen;

	BOOL m_BUseThisPen;
	int m_PenStyle;
	int m_PenWidth;
	COLORREF m_PenColor;
	bool m_bDrawSinglePoints;
};

typedef SVVector< SVDrawObjectClass > SVDrawObjectClassArray;



/////////////////////////////////////////////////////////////////////////////
// SVPolyLineDrawObjectClass 


class SVDrawObjectListClass
{
public:
	SVDrawObjectListClass() {m_bDrawFigureHatched = false;};
	
	virtual ~SVDrawObjectListClass() {};

	virtual void Add( SVDrawObjectListClass &p_rsvObject )
	{
		m_svDrawObjectArray.Append( p_rsvObject.m_svDrawObjectArray );
	};

	virtual void Add( SVDrawObjectClass &p_rsvObject )
	{
		m_svDrawObjectArray.Add( p_rsvObject );// only adds SVDrawObjectClass data and does not add array data
	};

	virtual void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, bool p_bDoNotUseLineColor = false )
	{
		long l_lCount = static_cast< long >( p_svMultiLine.m_svLineArray.GetSize() );

		for( long l = 0; l < l_lCount; l++ )
		{
			SVDrawObjectClass l_svDrawObject;

			l_svDrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[ l ] );

			if( p_bDoNotUseLineColor )
			{
				l_svDrawObject.SetDrawPen( FALSE );
			}

			m_svDrawObjectArray.Add( l_svDrawObject );
		}
	};

	virtual void AddExtentMultiLineData( SVExtentMultiLineStruct p_svMultiLine, int PenStyle )
	{
		long l_lCount = static_cast< long >( p_svMultiLine.m_svLineArray.GetSize() );

		for( long l = 0; l < l_lCount; l++ )
		{
			SVDrawObjectClass l_svDrawObject;

			l_svDrawObject.AddExtentLineData( p_svMultiLine.m_svLineArray[ l ], PenStyle );

			m_svDrawObjectArray.Add( l_svDrawObject );
		}
	};

	virtual BOOL Draw( SVDrawContext* PDrawContext )
	{
		BOOL RetVal = TRUE;

		if( m_bDrawFigureHatched )
		{
			long iLastYPointDrawn = -9999; // force the first line to be drawn.
			SVCPointArray Last;
			for( int i = 0; i < m_svDrawObjectArray.GetSize(); ++i )
			{
				if( PDrawContext->ShouldDrawPoints( iLastYPointDrawn, m_svDrawObjectArray[i].GetPointAt(0).y  ) )
				{
					RetVal = m_svDrawObjectArray[ i ].DrawHatch( PDrawContext, Last ) && RetVal; 
				}
			}
		}
		else
		{
			for( int i = 0; i < m_svDrawObjectArray.GetSize(); ++i )
			{
				RetVal = m_svDrawObjectArray[ i ].Draw( PDrawContext ) && RetVal; 
			}
		}
	
		return RetVal;
	};

	// This function will not delete the contents!
	// Embed your DrawObjects instead of construct them dynamically!
	void Flush()
	{
		m_svDrawObjectArray.RemoveAll();
	};

	int GetSize() const
	{
		return static_cast< int >( m_svDrawObjectArray.GetSize() );
	}

	virtual SVDrawObjectClass GetAt( int i ) const
	{
		return m_svDrawObjectArray.GetAt( i );
	}
	bool m_bDrawFigureHatched;

protected:
	SVDrawObjectClassArray m_svDrawObjectArray;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDrawObject.h_v  $
 * 
 *    Rev 1.2   15 Jan 2014 11:32:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  882
 * SCR Title:  Fix Mask - Zoom bug (e109)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DrawHatch function for drawing mask overlay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:20:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   27 Jul 2012 08:13:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   08 Nov 2010 14:35:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   16 Dec 2009 09:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   31 Dec 2008 11:34:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DrawSinglePoints method
 * Added GetPenColor method
 * Revised assignment operator to preserve drawSinglePoints flag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   09 Sep 2005 10:30:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed BuildMaskLines to draw every line and changed meaning of the drawObjectList Flag to be DrawHatchedFigure instead of LossOnZoom.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   24 Aug 2005 16:09:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed problem with draw pen.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Aug 2005 15:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   modified Draw of SVDrawObjectListClass so zoom will work with the overlayed mask figure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   18 May 2005 14:03:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to draw single pixels with a draw object.  To support drawing the mask.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   17 Feb 2005 13:52:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   08 Jul 2003 11:59:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVTemplate.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   22 Apr 2003 09:16:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   10 Jan 2003 14:08:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes so the SVDrawObjectClass can work in a consistent manner with SVDrawObjectListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   04 Apr 2002 09:51:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Apr 17 2001 16:01:44   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   26 Apr 2000 14:01:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetPointArray method.
 * Added GetDrawPen method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   14 Apr 2000 10:48:24   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  120
 * SCR Title:  Pattern Analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added the function 'GetAt' to SVDrawObjectClass 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   Mar 01 2000 11:56:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to use SVCPointArray type.
 * Revised to have virtual destructors.
 * Added another Copy( PointArray ) function.
 * Removed obsolete comment blocks.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   25 Feb 2000 16:56:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVDrawObjectClass::beginDraw to use SVDrawContext.
 * Revised all calls to beginDraw, passing only the pointer to the draw context.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 16:57:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   08 Feb 2000 11:00:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the functions SVCalculateRotation,
 * beginDraw, and all the Draw functions to accept a scaling factor (long
 * lZoomFactor) which can be a value from -16 to 16 (except 0). If the calling
 * function does not supply this parameter, it defaults to 0 (no scaling).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:27:30   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 1999 19:43:58   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Modified Draw Objects to be embedable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Aug 1999 17:35:20   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added Flush() Method in Draw Object List for embedded DrawObjects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Aug 1999 21:15:00   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Ported to 3.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
