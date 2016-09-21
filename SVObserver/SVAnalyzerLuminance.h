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

#pragma once

#pragma region Inlcudes
#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVImageAnalyzerClass.h"
#pragma endregion Inlcudes

class SVLuminanceAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVLuminanceAnalyzerClass );

public:
	SVLuminanceAnalyzerClass( LPCSTR ObjectName /* = "Luminace Analyzer" */ );
	SVLuminanceAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVLUMINANCEANALYZER );

	virtual ~SVLuminanceAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

/*- CloseObject () ------------------------------------------*/
/*- This function should only be called from the destructor  */
/*- and from derived functions CloseObject () functions. ----*/
	virtual BOOL  CloseObject();

    SVResultClass* GetResultObject();
   
	virtual BOOL OnValidate();

public:
	long msvlHistValueArraySize;

	SVLongValueObjectClass msvLuminanceValue;

	SVDoubleValueObjectClass msvVarianceValue;
	SVDoubleValueObjectClass msvStdDevValue;
	SVBoolValueObjectClass msvCalcStdDevValue;

private:
/*- USED BY CONSTRUCTORS ------------------------------------*/
/*- Common initialization routine used by all constructors. -*/
   void init();
/*- End of USED BY CONSTRUCTORS. ----------------------------*/

protected:
	virtual double calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares );

   virtual BOOL onRun( SVRunStatusClass& RRunStatus );

/*- USED BY onRun () ------------------------------------------*/
/*- These values are only used by the Run () function, and   */
/*- are maintained here to avoid time loss in allocating and */
/*- freeing these buffers for each function call. -----------*/
    SVMatroxImageResult        msvHistResultID;
	SVMatroxLongArray          msvplHistValues;
/*- End of USED BY Run (). ----------------------------------*/
};
