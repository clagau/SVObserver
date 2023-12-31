//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LuminanceAnalyzer
//* .File Name       : $Workfile:   SVAnalyzerLuminance.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:22  $
//******************************************************************************

#pragma once

#pragma region Inlcudes
#include "ImageAnalyzer.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Inlcudes

namespace SvAo
{

class LuminanceAnalyzer : public ImageAnalyzer
{
	SV_DECLARE_CLASS

public:
	explicit LuminanceAnalyzer( LPCSTR ObjectName /* = "Luminace Analyzer" */ );
	LuminanceAnalyzer( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVLUMINANCEANALYZER );

	virtual ~LuminanceAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

/*- CloseObject () ------------------------------------------*/
/*- This function should only be called from the destructor  */
/*- and from derived functions CloseObject () functions. ----*/
	virtual bool CloseObject() override;

	 virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	 virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

public:
	long msvlHistValueArraySize;

	SvVol::SVLongValueObjectClass msvLuminanceValue;

	SvVol::SVDoubleValueObjectClass msvVarianceValue;
	SvVol::SVDoubleValueObjectClass msvStdDevValue;
	SvVol::SVBoolValueObjectClass msvCalcStdDevValue;

private:
/*- USED BY CONSTRUCTORS ------------------------------------*/
/*- Common initialization routine used by all constructors. -*/
   void init();
/*- End of USED BY CONSTRUCTORS. ----------------------------*/

protected:
	double calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares );

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

/*- USED BY onRun () ------------------------------------------*/
/*- These values are only used by the Run () function, and   */
/*- are maintained here to avoid time loss in allocating and */
/*- freeing these buffers for each function call. -----------*/
	__int64        m_HistResultID = M_NULL;
	std::vector<long>          msvplHistValues;
/*- End of USED BY Run (). ----------------------------------*/
};

} //namespace SvAo
