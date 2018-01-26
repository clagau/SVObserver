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

//@TODO[gra][8.00][17.01.2018]: This class should be moved to the Analyzer project when created

class SVValueBaseNormalizerClass
{
public:
	SVValueBaseNormalizerClass();
	SVValueBaseNormalizerClass( const SVValueBaseNormalizerClass& rRhs );

	~SVValueBaseNormalizerClass();

	const SVValueBaseNormalizerClass& operator=( const SVValueBaseNormalizerClass& rRhs );

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

