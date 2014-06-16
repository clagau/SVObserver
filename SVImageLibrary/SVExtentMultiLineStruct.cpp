//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentMultiLineStruct
//* .File Name       : $Workfile:   SVExtentMultiLineStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2014 13:46:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentMultiLineStruct.h"
#include "SVImageExtentClass.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVExtentMultiLineStruct::SVExtentMultiLineStruct()
: m_InspectionID()
, m_ToolID()
, m_AnalyzerID()
, m_ObjectID()
, m_ToolSetDrawFlag( 0 )
, m_ToolDrawFlag( 0 )
, m_Passed( false )
, m_Warned( false )
, m_Failed( false )
, m_Color( 0 )
, m_StringPoint()
, m_csString()
, m_svLineArray()
{
}

SVExtentMultiLineStruct::SVExtentMultiLineStruct( const SVExtentMultiLineStruct &p_rsvValue )
: m_InspectionID( p_rsvValue.m_InspectionID )
, m_ToolID( p_rsvValue.m_ToolID )
, m_AnalyzerID( p_rsvValue.m_AnalyzerID )
, m_ObjectID( p_rsvValue.m_ObjectID )
, m_ToolSetDrawFlag( p_rsvValue.m_ToolSetDrawFlag )
, m_ToolDrawFlag( p_rsvValue.m_ToolDrawFlag )
, m_Passed( p_rsvValue.m_Passed )
, m_Warned( p_rsvValue.m_Warned )
, m_Failed( p_rsvValue.m_Failed )
, m_Color( p_rsvValue.m_Color )
, m_StringPoint( p_rsvValue.m_StringPoint )
, m_csString( p_rsvValue.m_csString )
, m_svLineArray( p_rsvValue.m_svLineArray )
{
}

SVExtentMultiLineStruct::~SVExtentMultiLineStruct()
{
	Initialize();
}

const SVExtentMultiLineStruct &SVExtentMultiLineStruct::operator=( const SVExtentMultiLineStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_InspectionID = p_rsvValue.m_InspectionID;
		m_ToolID = p_rsvValue.m_ToolID;
		m_AnalyzerID = p_rsvValue.m_AnalyzerID;
		m_ObjectID = p_rsvValue.m_ObjectID;

		m_ToolSetDrawFlag = p_rsvValue.m_ToolSetDrawFlag;
		m_ToolDrawFlag = p_rsvValue.m_ToolDrawFlag;

		m_Passed = p_rsvValue.m_Passed;
		m_Warned = p_rsvValue.m_Warned;
		m_Failed = p_rsvValue.m_Failed;

		m_Color = p_rsvValue.m_Color;

		m_StringPoint = p_rsvValue.m_StringPoint;
		m_csString = (LPCTSTR)(p_rsvValue.m_csString);

		m_svLineArray.Copy(p_rsvValue.m_svLineArray);
	}	
	return *this;
}

void SVExtentMultiLineStruct::Assign( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color )
{
	Initialize();

	m_Color = p_Color;

	AssignExtentFigure( p_rsvValue, p_Color );
}

void SVExtentMultiLineStruct::AssignExtentFigure( SVExtentFigureStruct &p_rsvValue, COLORREF p_Color )
{
	SVExtentLineStruct l_svLine;

	m_svLineArray.clear();

	l_svLine.m_dwColor = p_Color;

	switch( p_rsvValue.m_eShape )
	{
		case SVExtentShapeArrow:
		{
			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = -90.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopLeft );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 0.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svCenterRight );

			m_svLineArray.SetAtGrow( 1, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 90.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomLeft );

			m_svLineArray.SetAtGrow( 2, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = -135.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopRight );

			m_svLineArray.SetAtGrow( 3, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 135.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomRight );

			m_svLineArray.SetAtGrow( 4, l_svLine );

			break;
		}
		case SVExtentShapeLine:
		case SVExtentShapeLineHorizontal:
		{
			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 0.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svCenterLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svCenterRight );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			break;
		}
		case SVExtentShapeLineVertical:
		{
			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 90.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svTopCenter );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomCenter );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			break;
		}
		case SVExtentShapeRectangle:
		{
			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 0.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svTopLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopRight );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 90.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svTopLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomLeft );

			m_svLineArray.SetAtGrow( 1, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 0.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomRight );

			m_svLineArray.SetAtGrow( 2, l_svLine );

			l_svLine.m_bIsAngleValid = true;
			l_svLine.m_dAngle = 90.0;
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svTopRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svBottomRight );

			m_svLineArray.SetAtGrow( 3, l_svLine );

			break;
		}
		case SVExtentShapePie:
		{
			SVExtentPointStruct l_svPoint;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360.0;
			}

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / ( l_dInnerRadius >= 1.0 ? l_dInnerRadius : 1.0 );
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 1, l_svLine );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			long l_lIndex = 0;

			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius, l_dLoopAngle );

				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle += l_dStep;
			}

			l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius, l_dMinAngle );

			l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

			m_svLineArray.SetAtGrow( 2, l_svLine );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) /( l_dOuterRadius >= 1.0 ? l_dOuterRadius : 1.0 );
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			l_lIndex = 0;

			l_svLine.m_svPointArray.RemoveAll();

			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius, l_dLoopAngle );

				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle -= l_dStep;
			}

			l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius, l_dMaxAngle );

			l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

			m_svLineArray.SetAtGrow( 3, l_svLine );

			break;
		}
		case SVExtentShapeCircle:
		{
			SVExtentPointStruct l_svPoint;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360.0;
			}

			l_dStartAngle += 360.0;

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dInnerRadius;
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 1, l_svLine );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			long l_lIndex = 0;

			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0) , l_dLoopAngle );

				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle += l_dStep;
			}

			l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0) , l_dMinAngle );

			l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

			m_svLineArray.SetAtGrow( 2, l_svLine );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dOuterRadius;
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			l_lIndex = 0;

			l_svLine.m_svPointArray.RemoveAll();

			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0), l_dLoopAngle );

				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle -= l_dStep;
			}

			l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0), l_dMaxAngle );

			l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

			m_svLineArray.SetAtGrow( 3, l_svLine );

			break;
		}
		case SVExtentShapeDoubleCircle:
		{
			SVExtentPointStruct l_svPoint;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( p_rsvValue.m_svTopCenter, p_rsvValue.m_svBottomRight );

			if ( l_dStartAngle < l_dEndAngle )
			{
				l_dEndAngle -= 360.0;
			}

			l_dStartAngle += 360.0;

			double l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dInnerRadius;
			double l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svBottomRight );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );

			m_svLineArray.SetAtGrow( 1, l_svLine );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			long l_lIndex = 0;

			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius  , l_dLoopAngle );
				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle += l_dStep;
			}

			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dInnerRadius +  4.0 , l_dLoopAngle );
				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );

				l_dLoopAngle += l_dStep;
			}

			m_svLineArray.SetAtGrow( 2, l_svLine );

			l_dStep = ( l_dEndAngle - l_dStartAngle ) / l_dOuterRadius;
			l_dLoopStep = ::fabs( l_dStep );

			if( l_dLoopStep < 5.0 )
			{
				l_dLoopStep = 5.0;

				if ( l_dStep < 0.0 )
				{
					l_dStep = -5.0;
				}
				else
				{
					l_dStep = 5.0;
				}
			}
			
			l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			l_dLoopAngle = l_dEndAngle;

			l_lIndex = 0;

			l_svLine.m_svPointArray.RemoveAll();

			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius , l_dLoopAngle );
				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );
			}

			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				l_svPoint = SVRotatePoint( p_rsvValue.m_svTopCenter, l_dOuterRadius + 4.0 , l_dLoopAngle );
				l_svLine.m_svPointArray.SetAtGrow( l_lIndex++, l_svPoint );
			}

			m_svLineArray.SetAtGrow( 3, l_svLine );

			break;
		}
		case SVExtentShapeHexagon:
		{
			l_svLine.m_svPointArray.SetAtGrow( 0, p_rsvValue.m_svTopLeft );
			l_svLine.m_svPointArray.SetAtGrow( 1, p_rsvValue.m_svTopCenter );
			l_svLine.m_svPointArray.SetAtGrow( 2, p_rsvValue.m_svTopRight );
			l_svLine.m_svPointArray.SetAtGrow( 3, p_rsvValue.m_svCenterRight );
			l_svLine.m_svPointArray.SetAtGrow( 4, p_rsvValue.m_svBottomRight );
			l_svLine.m_svPointArray.SetAtGrow( 5, p_rsvValue.m_svBottomCenter );
			l_svLine.m_svPointArray.SetAtGrow( 6, p_rsvValue.m_svBottomLeft );
			l_svLine.m_svPointArray.SetAtGrow( 7, p_rsvValue.m_svCenterLeft );
			l_svLine.m_svPointArray.SetAtGrow( 8, p_rsvValue.m_svTopLeft );

			m_svLineArray.SetAtGrow( 0, l_svLine );

			break;
		}
	}
}

HRESULT SVExtentMultiLineStruct::Initialize()
{
	HRESULT l_hrOk = m_StringPoint.Initialize();
	
	m_InspectionID.clear();
	m_ToolID.clear();
	m_AnalyzerID.clear();
	m_ObjectID.clear();

	m_ToolSetDrawFlag = 0;
	m_ToolDrawFlag = 0;

	m_Passed = false;
	m_Warned = false;
	m_Failed = false;

	m_Color = 0;

	m_csString.Empty();

	m_svLineArray.RemoveAll();

	return l_hrOk;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentMultiLineStruct.cpp_v  $
 * 
 *    Rev 1.2   12 Jun 2014 13:46:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed integer constants to floating point where they should have been floating point constants.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jan 2014 07:40:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Protected AssignExtendFigure where divide by zero could occur.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:39:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Aug 2012 16:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix polar unwrap drawling problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Jul 2012 07:53:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Chaged the functionality of the assign method and added a new assign method that just updated the figure information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Jul 2012 15:11:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated assignment of color data to one location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Jul 2012 13:50:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the ability to set the color when adding a figure to the drawing data object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jul 2012 08:22:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names and updated methods to const to make variables more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 15:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jun 2007 11:09:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jun 2005 10:02:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Mar 2005 09:03:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support the new Perspective Warp Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
