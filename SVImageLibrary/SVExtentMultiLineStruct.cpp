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
//Moved to precompiled header: #include <math.h>
#include "SVExtentMultiLineStruct.h"
#include "SVImageExtentClass.h"

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


