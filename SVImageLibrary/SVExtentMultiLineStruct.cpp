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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <math.h>
#include "SVExtentMultiLineStruct.h"
#include "SVExtentFigureStruct.h"
#include "SVImageExtentClass.h"
#pragma endregion Includes

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

SVExtentMultiLineStruct::SVExtentMultiLineStruct( const SVExtentMultiLineStruct &rValue )
: m_InspectionID( rValue.m_InspectionID )
, m_ToolID( rValue.m_ToolID )
, m_AnalyzerID( rValue.m_AnalyzerID )
, m_ObjectID( rValue.m_ObjectID )
, m_ToolSetDrawFlag( rValue.m_ToolSetDrawFlag )
, m_ToolDrawFlag( rValue.m_ToolDrawFlag )
, m_Passed( rValue.m_Passed )
, m_Warned( rValue.m_Warned )
, m_Failed( rValue.m_Failed )
, m_Color( rValue.m_Color )
, m_StringPoint( rValue.m_StringPoint )
, m_csString( rValue.m_csString )
, m_svLineArray( rValue.m_svLineArray )
, m_bDrawFigureHatched( rValue.m_bDrawFigureHatched )
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
		m_csString = p_rsvValue.m_csString;

		m_svLineArray = p_rsvValue.m_svLineArray;
		m_bDrawFigureHatched = p_rsvValue.m_bDrawFigureHatched;
	}	
	return *this;
}

void SVExtentMultiLineStruct::Assign(const SVExtentFigureStruct &rValue, COLORREF Color )
{
	Initialize();

	m_Color = Color;

	AssignExtentFigure( rValue, Color );
}

void SVExtentMultiLineStruct::AssignExtentFigure(const SVExtentFigureStruct &rValue, COLORREF Color )
{
	SVExtentLineStruct svLine;

	m_svLineArray.clear();

	svLine.m_dwColor = Color;

	switch( rValue.m_eShape )
	{
		case SvDef::SVExtentShapeArrow:
		{
			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = -90.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopLeft );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 0.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterLeft );
			svLine.m_PointVector.push_back( rValue.m_svCenterRight );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 90.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterLeft );
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = -135.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterRight );
			svLine.m_PointVector.push_back( rValue.m_svTopRight );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 135.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterRight );
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeLine:
		case SvDef::SVExtentShapeLineHorizontal:
		{
			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 0.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svCenterLeft );
			svLine.m_PointVector.push_back( rValue.m_svCenterRight );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeLineVertical:
		{
			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 90.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );
			svLine.m_PointVector.push_back( rValue.m_svBottomCenter );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeRectangle:
		{
			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 0.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svTopLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopRight );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 90.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svTopLeft );
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 0.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );

			m_svLineArray.push_back( svLine );

			svLine.m_bIsAngleValid = true;
			svLine.m_dAngle = 90.0;
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svTopRight );
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapePie:
		{
			SVPoint<double> point;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svBottomRight );

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
			
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;


			svLine.m_PointVector.clear();
			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius, l_dLoopAngle );

				svLine.m_PointVector.push_back( point );

				l_dLoopAngle += l_dStep;
			}

			point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius, l_dMinAngle );

			svLine.m_PointVector.push_back( point );

			m_svLineArray.push_back( svLine );

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

			svLine.m_PointVector.clear();

			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius, l_dLoopAngle );

				svLine.m_PointVector.push_back( point );

				l_dLoopAngle -= l_dStep;
			}

			point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius, l_dMaxAngle );

			svLine.m_PointVector.push_back( point );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeCircle:
		{
			SVPoint<double> point;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svBottomRight );

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
			
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			double l_dMinAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dStartAngle : l_dEndAngle;
			double l_dMaxAngle = ( l_dStartAngle < l_dEndAngle ) ? l_dEndAngle : l_dStartAngle;
			double l_dLoopAngle = l_dStartAngle;

			svLine.m_PointVector.clear();
			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0) , l_dLoopAngle );

				svLine.m_PointVector.push_back( point );

				l_dLoopAngle += l_dStep;
			}

			point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0) , l_dMinAngle );

			svLine.m_PointVector.push_back( point );

			m_svLineArray.push_back( svLine );

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

			svLine.m_PointVector.clear();
			for( l_dAngle = l_dMinAngle; l_dAngle < l_dMaxAngle; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0), l_dLoopAngle );

				svLine.m_PointVector.push_back( point );

				l_dLoopAngle -= l_dStep;
			}

			point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius + (l_dAngle > l_dMinAngle + 360.0 ? 4.0 : 0.0), l_dMaxAngle );

			svLine.m_PointVector.push_back( point );

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeDoubleCircle:
		{
			SVPoint<double> point;

			double l_dAngle( 0.0 );

			double l_dStartAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomRight );
			double l_dEndAngle = SVGetRotationAngle( rValue.m_svTopCenter, rValue.m_svBottomLeft );

			double l_dInnerRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svCenterRight );
			double l_dOuterRadius = SVGetRadius( rValue.m_svTopCenter, rValue.m_svBottomRight );

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
			
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );

			m_svLineArray.push_back( svLine );

			double l_dLoopAngle = l_dStartAngle;

			svLine.m_PointVector.clear();
			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius  , l_dLoopAngle );
				svLine.m_PointVector.push_back( point );

				l_dLoopAngle += l_dStep;
			}

			for( l_dAngle = 0; l_dAngle < 360.0; l_dAngle += l_dLoopStep )
			{
				point = SVRotatePoint( rValue.m_svTopCenter, l_dInnerRadius +  4.0 , l_dLoopAngle );
				svLine.m_PointVector.push_back( point );

				l_dLoopAngle += l_dStep;
			}

			m_svLineArray.push_back( svLine );

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
			
			l_dLoopAngle = l_dEndAngle;

			svLine.m_PointVector.clear();
			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius , l_dLoopAngle );
				svLine.m_PointVector.push_back( point );
			}

			for( l_dAngle = 0 ; l_dAngle < 360.0 ; l_dAngle += l_dLoopStep )
			{
				l_dLoopAngle -= l_dStep;

				point = SVRotatePoint( rValue.m_svTopCenter, l_dOuterRadius + 4.0 , l_dLoopAngle );
				svLine.m_PointVector.push_back( point );
			}

			m_svLineArray.push_back( svLine );

			break;
		}
		case SvDef::SVExtentShapeHexagon:
		{
			svLine.m_PointVector.clear();
			svLine.m_PointVector.push_back( rValue.m_svTopLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopCenter );
			svLine.m_PointVector.push_back( rValue.m_svTopRight );
			svLine.m_PointVector.push_back( rValue.m_svCenterRight );
			svLine.m_PointVector.push_back( rValue.m_svBottomRight );
			svLine.m_PointVector.push_back( rValue.m_svBottomCenter );
			svLine.m_PointVector.push_back( rValue.m_svBottomLeft );
			svLine.m_PointVector.push_back( rValue.m_svCenterLeft );
			svLine.m_PointVector.push_back( rValue.m_svTopLeft );

			m_svLineArray.push_back( svLine );

			break;
		}
	}
}

void SVExtentMultiLineStruct::Initialize()
{
	m_StringPoint.clear();
	
	m_InspectionID.clear();
	m_ToolID.clear();
	m_AnalyzerID.clear();
	m_ObjectID.clear();

	m_ToolSetDrawFlag = 0;
	m_ToolDrawFlag = 0;

	m_Passed = false;
	m_Warned = false;
	m_Failed = false;
	m_bDrawFigureHatched = false;

	m_Color = 0;

	m_csString.clear();

	m_svLineArray.clear();
}


