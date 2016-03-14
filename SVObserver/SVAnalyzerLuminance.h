//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLuminanceAnalyzerClass
//* .File Name       : $Workfile:   SVAnalyzerLuminance.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:22  $
//******************************************************************************

#ifndef SVANALYZERLUMINANCE_H
#define SVANALYZERLUMINANCE_H

#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVAnalyzer.h"

////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLuminanceAnalyzerClass
//* Note(s)    : Luminance Analyzer.
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/



////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVLuminanceAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVLuminanceAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVLuminanceAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVLuminanceAnalyzerClass( LPCSTR ObjectName /* = "Luminace Analyzer" */ );
	SVLuminanceAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLUMINANCEANALYZER );

public:
   long             msvlHistValueArraySize;

   SVLongValueObjectClass msvLuminanceValue;

	SVDoubleValueObjectClass msvVarianceValue;
	SVDoubleValueObjectClass msvStdDevValue;
	SVBoolValueObjectClass msvCalcStdDevValue;

	virtual        ~SVLuminanceAnalyzerClass();

	virtual BOOL   CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

/*- CloseObject () ------------------------------------------*/
/*- This function should only be called from the destructor  */
/*- and from derived functions CloseObject () functions. ----*/
	virtual BOOL   CloseObject();

    SVResultClass* GetResultObject();
   
	virtual BOOL   OnValidate ();


private:
/*- USED BY CONSTRUCTORS ------------------------------------*/
/*- Common initialization routine used by all constructors. -*/
   void init();
/*- End of USED BY CONSTRUCTORS. ----------------------------*/

protected:
	virtual double calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares );

   virtual BOOL   onRun( SVRunStatusClass& RRunStatus );

/*- USED BY onRun () ------------------------------------------*/
/*- These values are only used by the Run () function, and   */
/*- are maintained here to avoid time loss in allocating and */
/*- freeing these buffers for each function call. -----------*/
    SVMatroxImageResult        msvHistResultID;
	SVMatroxLongArray          msvplHistValues;
/*- End of USED BY Run (). ----------------------------------*/

};



#endif   // _SVANALYZERLUMINANCE_H

