//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueBaseNormalizerClass
//* .File Name       : $Workfile:   SVValueBaseNormalizerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:55:34  $
//******************************************************************************

#pragma once


class SVValueBaseNormalizerClass
{
public:
	SVValueBaseNormalizerClass();
	SVValueBaseNormalizerClass( const SVValueBaseNormalizerClass& S2 );

	~SVValueBaseNormalizerClass();

	const SVValueBaseNormalizerClass& operator=( const SVValueBaseNormalizerClass& S2 );

	bool SetNormalRange( double Min, double Max, double Step = 1.0 );
	bool SetRealRange( double Min, double Max, double Step = 1.0 );

	double GetNormalRangeBaseSize();
	double GetNormalRangeMax();
	double GetNormalRangeMin();
	double GetNormalRangeStepWidth();
	double GetRealRangeBaseSize();
	double GetRealRangeMax();
	double GetRealRangeMin();
	double GetRealRangeStepWidth();

	double CalcRealValue( double XN );
	double CalcNormalizedValue( double X );

protected:
	double calcBaseSize( double Min, double Max, double Step );

//******************************************************************************
//* Attributes(s):
//******************************************************************************
protected:
	double normalMax;
	double normalMin;
	double normalStep;
	double normalBaseSize;
	double rangeMax;
	double rangeMin;
	double rangeStep;
	double rangeBaseSize;
};

