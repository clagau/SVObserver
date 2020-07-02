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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "Definitions/SVExtentEnums.h"
#include "SVExtentSizeStruct.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

struct SVExtentFigureStruct  
{
	SVExtentFigureStruct();
	SVExtentFigureStruct( const SVExtentFigureStruct &p_rsvValue );
	explicit SVExtentFigureStruct(RECT p_oRect);
	~SVExtentFigureStruct();

	const SVExtentFigureStruct& operator = ( const SVExtentFigureStruct &p_rsvValue );
	const SVExtentFigureStruct& operator = ( RECT p_oRect );

	const SVExtentFigureStruct& operator += ( const SVPoint<double>& rOffset );
	const SVExtentFigureStruct& operator -= ( const SVPoint<double>& rOffset );

	HRESULT Initialize();

	HRESULT IsPointOverFigure( const SVPoint<double>& rPoint ) const;

	HRESULT GetRect(RECT& p_rRect) const;
	RECT Rect() const;

	SVExtentSizeStruct Size() const;
	double Top() const;
	double Left() const;

	double Right() const;
	HRESULT SetRect( double p_dTop, double p_dLeft, double p_dBottom, double p_dRight );

	SvDef::SVExtentShapeEnum m_eShape;

	SVPoint<double> m_svTopLeft;
	SVPoint<double> m_svTopRight;
	SVPoint<double> m_svBottomRight;
	SVPoint<double> m_svBottomLeft;
	SVPoint<double> m_svCenterLeft;
	SVPoint<double> m_svCenterRight;
	SVPoint<double> m_svTopCenter;
	SVPoint<double> m_svBottomCenter;
	SVPoint<double> m_svCenter;

};
