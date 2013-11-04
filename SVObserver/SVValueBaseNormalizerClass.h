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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVValueBaseNormalizerClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:55:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Aug 2005 13:54:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 17:29:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2001 15:53:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed prototype for constuctor and operator= method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2001 15:26:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
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