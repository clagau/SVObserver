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

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

#ifndef _SVVALUEBASENORMALIZERCLASS_H
#define _SVVALUEBASENORMALIZERCLASS_H

//******************************************************************************
//* OBJECT DEFINITION
//******************************************************************************
class SVValueBaseNormalizerClass
{
public:
	SVValueBaseNormalizerClass();
	SVValueBaseNormalizerClass( const SVValueBaseNormalizerClass& S2 );

	~SVValueBaseNormalizerClass();

	const SVValueBaseNormalizerClass& operator=( const SVValueBaseNormalizerClass& S2 );

	BOOL SetNormalRange( double Min, double Max, double Step = 1.0 );
	BOOL SetRealRange( double Min, double Max, double Step = 1.0 );

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

#endif   // _SVVALUEBASENORMALIZERCLASS_H

