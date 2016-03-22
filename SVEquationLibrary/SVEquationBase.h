//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationBase
//* .File Name       : $Workfile:   SVEquationBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:22:54  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>

/**
@SVObjectName Equation Base

@SVObjectOverview This abstract object is used to define the operations required by the LEX and YACC objects that allow access the main application data.

@SVObjectOperations The operations will be used to interface into the main application data and notate which application elements the LEX and YACC will need to access.

The Add Symbol operator notifies the equation class to register the specified item as an input into the equation.

The Get Property Value operator returns the value of the specified token by its symbol index.

The Get Subscripted Property Value operator returns the value at a particular subscript of the specified token by its symbol index.  If a default value is provided, it will be returned if the subscript is invalid.

The Get Array Values operator fills the provided array with the values of the specified token by its symbol index.

*/
class SVEquationBase
{
public:
	virtual ~SVEquationBase();

	virtual int AddSymbol( const char* name ) = 0;

	virtual double GetPropertyValue( int iSymbolIndex ) = 0;
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex, double dDefault ) = 0;
	virtual double GetSubscriptedPropertyValue( int iSymbolIndex, int iIndex ) = 0;
	virtual HRESULT GetArrayValues( int iSymbolIndex, std::vector< double >& values ) = 0;

};

