//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueBaseNormalizerClass
//* .File Name       : $Workfile:   SVValueBaseNormalizerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:58:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVValueBaseNormalizerClass.h"
#pragma endregion Includes

SVValueBaseNormalizerClass::SVValueBaseNormalizerClass()
{
	// Defaults
	normalMax = 1023.0;
	normalMin = 0.0;
	normalStep = 1.0;
	normalBaseSize = 1024.0;
	rangeMax = 255.0;
	rangeMin = 0.0;
	rangeStep = 1.0;
	rangeBaseSize = 256.0;
};

SVValueBaseNormalizerClass::SVValueBaseNormalizerClass( const SVValueBaseNormalizerClass& rRhs )
{
	(*this) = rRhs;
};


SVValueBaseNormalizerClass::~SVValueBaseNormalizerClass()
{
};

const SVValueBaseNormalizerClass& SVValueBaseNormalizerClass::operator=( const SVValueBaseNormalizerClass& rRhs )
{
	normalMax = rRhs.normalMax;
	normalMin = rRhs.normalMin;
	normalStep = rRhs.normalStep;
	normalBaseSize = rRhs.normalBaseSize;
	rangeMax = rRhs.rangeMax;
	rangeMin = rRhs.rangeMin;
	rangeStep = rRhs.rangeStep;
	rangeBaseSize = rRhs.rangeBaseSize;

	return (*this);	
};

bool SVValueBaseNormalizerClass::SetNormalRange( double Min, double Max, double Step)
{
	double baseSize = calcBaseSize( Min, Max, Step );
	if( baseSize >= 1.0 )
	{
		normalMax		= Max;
		normalMin		= Min;
		normalStep		= Step;
		normalBaseSize	= baseSize;
		return true;
	}
	return false;
};

bool SVValueBaseNormalizerClass::SetRealRange( double Min, double Max, double Step)
{
	double baseSize = calcBaseSize( Min, Max, Step );
	if( baseSize >= 1.0 )
	{
		rangeMax		= Max;
		rangeMin		= Min;
		rangeStep		= Step;
		rangeBaseSize	= baseSize;
		return true;
	}
	return false;
};

double SVValueBaseNormalizerClass::GetNormalRangeBaseSize()
{
	return normalBaseSize;
};

double SVValueBaseNormalizerClass::GetNormalRangeMax()
{
	return normalMax;
};

double SVValueBaseNormalizerClass::GetNormalRangeMin()
{
	return normalMin;
};

double SVValueBaseNormalizerClass::GetNormalRangeStepWidth()
{
	return normalStep;
};

double SVValueBaseNormalizerClass::GetRealRangeBaseSize()
{
	return rangeBaseSize;
};

double SVValueBaseNormalizerClass::GetRealRangeMax()
{
	return rangeMax;
};

double SVValueBaseNormalizerClass::GetRealRangeMin()
{
	return rangeMin;
};

double SVValueBaseNormalizerClass::GetRealRangeStepWidth()
{
	return rangeStep;
};

double SVValueBaseNormalizerClass::CalcRealValue( double XN )
{
	return( ( rangeBaseSize / normalBaseSize ) * ( XN - normalMin ) + rangeMin );
};

double SVValueBaseNormalizerClass::CalcNormalizedValue( double X )
{
	return( ( normalBaseSize / rangeBaseSize ) * ( X - rangeMin ) + normalMin );
};

double SVValueBaseNormalizerClass::calcBaseSize( double Min, double Max, double Step )
{
	if (Max > Min && Step > 0.0)
	{
		return((Max - Min + 1) / Step);
	}

	return -1.0;
};

