//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentFigureStruct
//* .File Name       : $Workfile:   SVExtentFigureStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:46  $
//******************************************************************************

#ifndef SVEXTENTFIGURESTRUCT_H
#define SVEXTENTFIGURESTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVExtentEnums.h"
#include "SVExtentPointStruct.h"
#include "SVExtentSizeStruct.h"

struct SVExtentFigureStruct  
{
	SVExtentFigureStruct();
	SVExtentFigureStruct( const SVExtentFigureStruct &p_rsvValue );
	SVExtentFigureStruct( RECT p_oRect );
	virtual ~SVExtentFigureStruct();

	const SVExtentFigureStruct& operator = ( const SVExtentFigureStruct &p_rsvValue );
	const SVExtentFigureStruct& operator = ( RECT p_oRect );

	const SVExtentFigureStruct& operator += ( const SVExtentPointStruct& p_rsvOffset );
	const SVExtentFigureStruct& operator -= ( const SVExtentPointStruct& p_rsvOffset );

	virtual HRESULT Initialize();

	HRESULT IsPointOverFigure( const SVExtentPointStruct& p_rPoint ) const;

	HRESULT GetRect(RECT& p_rRect) const;
	RECT Rect() const;
	HRESULT GetRectSize( SVExtentSizeStruct& p_rSize ) const;
	SVExtentSizeStruct Size() const;
	double Top() const;
	double Left() const;
	double Bottom() const;
	double Right() const;
	HRESULT SetRect( double p_dTop, double p_dLeft, double p_dBottom, double p_dRight );
	HRESULT SetRectTop( double p_dTop );
	HRESULT SetRectLeft( double p_dTop );
	HRESULT SetRectRight( double p_dTop );
	HRESULT SetRectBottom( double p_dTop );
	HRESULT SetRectCenter( SVExtentPointStruct p_svCenter );

	SVExtentShapeEnum m_eShape;

	SVExtentPointStruct m_svTopLeft;
	SVExtentPointStruct m_svTopRight;
	SVExtentPointStruct m_svBottomRight;
	SVExtentPointStruct m_svBottomLeft;
	SVExtentPointStruct m_svCenterLeft;
	SVExtentPointStruct m_svCenterRight;
	SVExtentPointStruct m_svTopCenter;
	SVExtentPointStruct m_svBottomCenter;
	SVExtentPointStruct m_svCenter;

};

typedef SVVector< SVExtentFigureStruct > SVExtentFigureStructCArray;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentFigureStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:39:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2012 12:19:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent structures to provide a hit test methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 15:41:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2005 14:36:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operator +, operator -
 * added RECT conversions
 * added m_svCenter
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:02:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
