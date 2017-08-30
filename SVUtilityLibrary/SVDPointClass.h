//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointClass
//* .File Name       : $Workfile:   SVDPointClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVString.h"
#pragma endregion Includes

/**
@SVObjectName Double Point

@SVObjectOverview

@SVObjectOperations

*/
class SVDPointClass  
{
public:
	SVDPointClass();
	SVDPointClass( const POINT &Point );
	SVDPointClass( const SVDPointClass &Point );
	SVDPointClass( double dX, double dY );
	SVDPointClass( const VARIANT& p_rVariant );

	void GetValue( POINT &Point ) const;

	~SVDPointClass();

	operator POINT () const;
	operator _variant_t () const;

	SVString ToString() const;
	
	SVDPointClass& operator=( const POINT &Point );
	SVDPointClass& operator=( const SVDPointClass &Point );
	SVDPointClass& operator=( const VARIANT& p_rVariant );

	double x;
	double y;

	bool operator==( const SVDPointClass &Point ) const;
	bool operator!=( const SVDPointClass &Point ) const;

private:
	void local_initialize();
};

