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

#include "stdafx.h"
#include "SVValueBaseNormalizerClass.h"

// ***** Public Operators *****

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

SVValueBaseNormalizerClass::SVValueBaseNormalizerClass( const SVValueBaseNormalizerClass& S2 )
{
	(*this) = S2;
};


SVValueBaseNormalizerClass::~SVValueBaseNormalizerClass()
{
};

const SVValueBaseNormalizerClass& SVValueBaseNormalizerClass::operator=( const SVValueBaseNormalizerClass& S2 )
{
	normalMax = S2.normalMax;
	normalMin = S2.normalMin;
	normalStep = S2.normalStep;
	normalBaseSize = S2.normalBaseSize;
	rangeMax = S2.rangeMax;
	rangeMin = S2.rangeMin;
	rangeStep = S2.rangeStep;
	rangeBaseSize = S2.rangeBaseSize;

	return (*this);	
};

BOOL SVValueBaseNormalizerClass::SetNormalRange( double Min, double Max, double Step)
{
	double baseSize = calcBaseSize( Min, Max, Step );
	if( baseSize >= 1.0 )
	{
		normalMax		= Max;
		normalMin		= Min;
		normalStep		= Step;
		normalBaseSize	= baseSize;
		return TRUE;
	}
	return FALSE;
};

BOOL SVValueBaseNormalizerClass::SetRealRange( double Min, double Max, double Step)
{
	double baseSize = calcBaseSize( Min, Max, Step );
	if( baseSize >= 1.0 )
	{
		rangeMax		= Max;
		rangeMin		= Min;
		rangeStep		= Step;
		rangeBaseSize	= baseSize;
		return TRUE;
	}
	return FALSE;
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
	if( Max > Min && Step > 0.0 )
		return( ( Max - Min + 1 ) / Step );

	return( -1.0 );
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVValueBaseNormalizerClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:58:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Aug 2005 13:54:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:29:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Nov 2001 15:53:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed prototype for constuctor and operator= method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Apr 2001 20:02:10   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   New file due to header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/