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

#include "SVContainerLibrary/SVVector.h"
#include "SVExtentEnums.h"
#include "SVExtentPointStruct.h"
#include "SVExtentSizeStruct.h"

struct SVExtentFigureStruct  
{
	SVExtentFigureStruct();
	SVExtentFigureStruct( const SVExtentFigureStruct &p_rsvValue );
	SVExtentFigureStruct( RECT p_oRect );
	~SVExtentFigureStruct();

	const SVExtentFigureStruct& operator = ( const SVExtentFigureStruct &p_rsvValue );
	const SVExtentFigureStruct& operator = ( RECT p_oRect );

	const SVExtentFigureStruct& operator += ( const SVExtentPointStruct& p_rsvOffset );
	const SVExtentFigureStruct& operator -= ( const SVExtentPointStruct& p_rsvOffset );

	HRESULT Initialize();

	HRESULT IsPointOverFigure( const SVExtentPointStruct& p_rPoint ) const;

	HRESULT GetRect(RECT& p_rRect) const;
	RECT Rect() const;

	SVExtentSizeStruct Size() const;
	double Top() const;
	double Left() const;

	double Right() const;
	HRESULT SetRect( double p_dTop, double p_dLeft, double p_dBottom, double p_dRight );

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

