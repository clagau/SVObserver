//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPointStruct
//* .File Name       : $Workfile:   SVExtentPointStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:06  $
//******************************************************************************

#ifndef SVEXTENTPOINTSTRUCT_H
#define SVEXTENTPOINTSTRUCT_H

#include "SVContainerLibrary/SVVector.h"

struct SVExtentPointStruct  
{
	SVExtentPointStruct();
	SVExtentPointStruct( const SVExtentPointStruct &p_rsvValue );
	SVExtentPointStruct( const POINT &p_roValue );
	SVExtentPointStruct( double l_dValueX, double l_dValueY );
	virtual ~SVExtentPointStruct();

	const SVExtentPointStruct &operator=( const SVExtentPointStruct &p_rsvValue );
	const SVExtentPointStruct &operator=( const POINT &p_roValue );

	virtual HRESULT Initialize();

	bool operator==( const SVExtentPointStruct &p_rsvValue ) const;
	bool operator!=( const SVExtentPointStruct &p_rsvValue ) const;

	const SVExtentPointStruct& operator -= (const SVExtentPointStruct& p_rPoint);
	const SVExtentPointStruct& operator += (const SVExtentPointStruct& p_rPoint);
	friend SVExtentPointStruct operator - (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2);
	friend SVExtentPointStruct operator + (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2);
	friend SVExtentPointStruct operator / (const SVExtentPointStruct& p_rPoint1, double p_Constant);

	operator POINT() const;

	double m_dPositionX;
	double m_dPositionY;
};

typedef SVVector< SVExtentPointStruct > SVExtentPointStructCArray;

#endif

