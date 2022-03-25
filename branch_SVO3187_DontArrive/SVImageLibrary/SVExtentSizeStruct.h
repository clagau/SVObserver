//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentSizeStruct
//* .File Name       : $Workfile:   SVExtentSizeStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:14  $
//******************************************************************************

#pragma once

struct SVExtentSizeStruct
{
	SVExtentSizeStruct();
	SVExtentSizeStruct( const SVExtentSizeStruct& p_rsvValue );
	explicit SVExtentSizeStruct(const SIZE& p_roValue);
	SVExtentSizeStruct( double p_dCX, double p_dCY );
	~SVExtentSizeStruct();

	const SVExtentSizeStruct& operator = ( const SVExtentSizeStruct& p_rsvValue );
	const SVExtentSizeStruct& operator = ( const SIZE& p_roValue );

	HRESULT Initialize();

	bool operator == ( const SVExtentSizeStruct& p_rsvValue ) const;
	bool operator != ( const SVExtentSizeStruct& p_rsvValue ) const;

	operator SIZE() const;

	double m_dCX;
	double m_dCY;
};

