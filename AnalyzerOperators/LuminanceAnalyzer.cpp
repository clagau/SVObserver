//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLuminance
//* .File Name       : $Workfile:   SVAnalyzerLuminance.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:27:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LuminanceAnalyzer.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVRange.h"
#include "Operators/SVResultLong.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( LuminanceAnalyzer, SvPb::LuminanceAnalyzerClassId);

LuminanceAnalyzer::LuminanceAnalyzer( LPCSTR ObjectName )
: ImageAnalyzer( ObjectName )
, msvlHistValueArraySize( 0 )
{
	init();
}

LuminanceAnalyzer::LuminanceAnalyzer( SVObjectClass* POwner, int StringResourceID )
: ImageAnalyzer( POwner, StringResourceID ) 
, msvlHistValueArraySize( 0 )
{
	init();
}

void LuminanceAnalyzer::init()
{
	// init () should instantiate any dynamic children and 
	//   add them to the task object list.  
	// Identify output type.
	// Identify input type needs.
	// Register any embedded objects -- all embedded objects are
	//   outputs by default.
	// Set embedded defaults.
	// Set local defaults.
	// 

	// Default taskObjectList items:


	// Identify our output type
	m_ObjectTypeInfo.m_SubType = SvPb::SVLuminanceAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvLuminanceValue, 
		SvPb::LuminanceValueEId,
		IDS_OBJECTNAME_LUMINANCE,
		false, SvOi::SVResetItemNone, false);

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvCalcStdDevValue,
		SvPb::CalcStdDevEId,
		IDS_OBJECTNAME_CALCSTDDEV,
		false, SvOi::SVResetItemNone, true);

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvVarianceValue,
		SvPb::StatVarianceEId,
		IDS_OBJECTNAME_STATVARIANCE,
		false, SvOi::SVResetItemNone, false);

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvStdDevValue,
		SvPb::StatStdDevEId,
		IDS_OBJECTNAME_STATSTDDEV,
		false, SvOi::SVResetItemNone, false);

	// Set Embedded defaults
	msvLuminanceValue.SetDefaultValue (0, TRUE);
	msvLuminanceValue.setSaveValueFlag(false);
	msvVarianceValue.SetDefaultValue (0, TRUE);
	msvVarianceValue.setSaveValueFlag(false);
	msvStdDevValue.SetDefaultValue (0, TRUE);
	msvStdDevValue.setSaveValueFlag(false);

	// Set Local

	SvOp::SVLongResult* pAnalyzerResult = new SvOp::SVLongResult(this, IDS_CLASSNAME_SVLUMINANCEANALYZERRESULT);

	if(nullptr == pAnalyzerResult)
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
	}
	else
	{
		Add(pAnalyzerResult);
	}
}

bool LuminanceAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (M_NULL == m_HistResultID)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}
	else if (0 == msvplHistValues.size())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

LuminanceAnalyzer::~LuminanceAnalyzer()
{
	LuminanceAnalyzer::CloseObject();
}

bool LuminanceAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	SvIe::SVImageClass* pImage(nullptr);
	bool bError(false);

	if (! ImageAnalyzer::CreateObject( rCreateStructure ) )
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
		bError = true;
	}
	else
	{
		pImage = getInputImage ();
		if (nullptr == pImage )
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			bError = true;
		}
	}

	if (!bError)
	{
		msvlHistValueArraySize = 1 << (pImage->getPixelDepth() & SVBufferSize);
		msvplHistValues.resize( msvlHistValueArraySize );
		if (msvplHistValues.size() == 0)
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			bError = true;
		}
	}

	if (!bError)
	{
		for( int i = 0; i < msvlHistValueArraySize; i++ )
		{
			msvplHistValues [i] = 0L;
		}
		SvIe::SVDataBufferInfoClass svData;
		svData.Length = msvlHistValueArraySize;
		svData.Type = SvIe::SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_HistResultID;
		if ( S_OK == SvIe::SVImageProcessingClass::CreateDataBuffer( &svData ) )
		{
			m_HistResultID = svData.HBuffer.milResult;
		}

		if (M_NULL == m_HistResultID)
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			bError = true;
		}
	}

	if (bError )
	{
		m_isCreated = false;
	}
	else
	{
		m_isCreated = true;
	}

	msvLuminanceValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvVarianceValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvStdDevValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvCalcStdDevValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	return m_isCreated;
}


bool LuminanceAnalyzer::CloseObject()
{

	msvplHistValues.clear();
	SVMatroxImageInterface l_lIntf;
	l_lIntf.Destroy( m_HistResultID );
	ImageAnalyzer::CloseObject ();

	return true;
}

void LuminanceAnalyzer::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	bool isNoError = true;

	inserter = SvOi::ParameterPairForML(msvLuminanceValue.GetCompleteName(), msvLuminanceValue.getObjectId());
	SvOp::SVRange* pRangeObject = dynamic_cast<SvOp::SVRange*>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVRangeObjectType)));
	if (nullptr != pRangeObject)
	{
		pRangeObject->addEntriesToMonitorList(inserter);
	}
	else
	{
		isNoError = false;
	}

	if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
		rMessages.push_back(Msg);
	}
}

bool LuminanceAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	__int64          sum(0);
	long             lNbrPixels(0);
	double			 value = 0.0;
	bool  LastError(false);

	while (1)
	{
		///////////////////////////////////////////////////
		// Clear Variance and Standard Deviation
		///////////////////////////////////////////////////
		msvVarianceValue.SetValue(value);
		msvStdDevValue.SetValue(value);

		if( !__super::onRun( rRunStatus, pErrorMessages ) )
		{
			LastError = true;
			break;
		}

		SvIe::SVImageClass* pInputImage = getInputImage(true);

		if(nullptr == pInputImage)
		{
        
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		if( nullptr == pImageBuffer || pImageBuffer->isEmpty())
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVMatroxImageInterface l_lImageIntf;

		HRESULT MatroxResult = l_lImageIntf.Histogram(m_HistResultID, pImageBuffer->getHandle()->GetBuffer() );
		if( S_OK != MatroxResult )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MatroxResult = l_lImageIntf.GetResult(m_HistResultID,  msvplHistValues );


		if( S_OK != MatroxResult )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;

		}

		for (size_t lI = 0; lI < msvplHistValues.size(); lI++)
		{
			sum = sum + (msvplHistValues [lI] * lI);
			lNbrPixels = lNbrPixels + msvplHistValues [lI];
		}

		if (lNbrPixels == 0)
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if ( S_OK != msvLuminanceValue.SetValue(static_cast<long> (sum / lNbrPixels)) )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////
		// Get calculate StdDeviation flag
		///////////////////////////////////////////////////
		BOOL calcStdDev = FALSE;

		if ( S_OK != msvCalcStdDevValue.GetValue( calcStdDev ))
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if (calcStdDev)
		{
			double accumulatedSquares = 0.0;	// for calculating variance

			for (size_t lI = 0; lI < msvplHistValues.size(); lI++)
			{
				///////////////////////////////////////////////////
				// Accumulate the Squares of the luminance
				///////////////////////////////////////////////////
				value = ( double )( ( double )msvplHistValues [lI] * ( double )lI * ( double )lI );
				accumulatedSquares += value;
			}

			///////////////////////////////////////////////////////////////////
			// Calculate Standard Deviation on the Histogram
			// First need to calculate the Variance
			///////////////////////////////////////////////////////////////////
			double numberOfSamples = ( double )lNbrPixels;
			if( numberOfSamples > 1 )
			{
				///////////////////////////////////////////////////////////////////
				// Caluculate the average
				///////////////////////////////////////////////////////////////////
				double averageValue = ( double )( sum / numberOfSamples );
				value = calculateVariance( numberOfSamples, averageValue, accumulatedSquares );

				///////////////////////////////////////////////////////////////////
				// Save Variance
				///////////////////////////////////////////////////////////////////
				if (S_OK != msvVarianceValue.SetValue(value))
				{
					LastError = true;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						pErrorMessages->push_back(Msg);
					}
					break;
				}

				///////////////////////////////////////////////////////////////////
				// Standard Deviation is the positive square root of the Variance
				// Note: Variance cannot be negative !!!
				///////////////////////////////////////////////////////////////////
				if( value )
				{
					if (value < 0)
					{
						LastError = true;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
							pErrorMessages->push_back(Msg);
						}
						break;
					}
					else
						value = fabs( sqrt( value ) );
				}
				///////////////////////////////////////////////////////////////////
				// Save Standard Deviation
				///////////////////////////////////////////////////////////////////
				if (S_OK != msvStdDevValue.SetValue(value))
				{
					LastError = true;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						pErrorMessages->push_back(Msg);
					}
					break;
				}
			}
		}

		break;
	}

	if( LastError )  
	{
		rRunStatus.SetInvalid();
		return false;
	}

	return true;
}

double LuminanceAnalyzer::calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares )
{
	///////////////////////////////////////////////////////////////////
	// Variance is a cumulative measure of the
	// Squares of the difference of all the data values from the Mean
	// Note: Calculating Sample Variation not Population Variance
	///////////////////////////////////////////////////////////////////
	double averageTimesSamples = aAverageValue * aNumberOfSamples; 
	double averageTimesSamplesSquared = (averageTimesSamples * averageTimesSamples);

	double value =
		( 1.0 / ( aNumberOfSamples * ( aNumberOfSamples - 1.0 ) ) ) *
		( aNumberOfSamples * aAccumulatedSquares - ( averageTimesSamplesSquared) );

	return value;
}

} //namespace SvAo
