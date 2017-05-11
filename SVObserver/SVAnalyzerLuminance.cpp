//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVAnalyzerLuminance.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVAnalyzerLuminanceSetup.h"
#include "SVOCore/SVDataBuffer.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVResultLong.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"

#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLuminanceAnalyzerClass, SVLuminanceAnalyzerClassGuid );

SVLuminanceAnalyzerClass::SVLuminanceAnalyzerClass( LPCSTR ObjectName )
: SVImageAnalyzerClass( ObjectName )
, msvlHistValueArraySize( 0 )
{
	init();
}

SVLuminanceAnalyzerClass::SVLuminanceAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
, msvlHistValueArraySize( 0 )
{
	init();
}

void SVLuminanceAnalyzerClass::init()
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
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLuminanceAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvLuminanceValue, 
		SVLuminanceValueObjectGuid, 
		IDS_OBJECTNAME_LUMINANCE,
		false, SvOi::SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvCalcStdDevValue,
		SVCalcStdDevObjectGuid,
		IDS_OBJECTNAME_CALCSTDDEV,
		false, SvOi::SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvVarianceValue,
		SVStatVarianceObjectGuid,
		IDS_OBJECTNAME_STATVARIANCE,
		false, SvOi::SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvStdDevValue,
		SVStatStdDevObjectGuid,
		IDS_OBJECTNAME_STATSTDDEV,
		false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	msvLuminanceValue.SetDefaultValue (0, TRUE);
	msvVarianceValue.SetDefaultValue (0, TRUE);
	msvStdDevValue.SetDefaultValue (0, TRUE);

	// Set default inputs and outputs
	addDefaultInputObjects();


	// Set Local

	SVLongResultClass* pAnalyzerResult = 
		new SVLongResultClass( true, this, IDS_CLASSNAME_SVLUMINANCEANALYZERESULT );

	if(nullptr == pAnalyzerResult)
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16092);
	}
	else
	{
		Add( pAnalyzerResult );
	}
}

bool SVLuminanceAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);

	if (msvHistResultID.empty())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16094, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}
	else if (0 == msvplHistValues.size())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16095, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	return Valid;
}

SVLuminanceAnalyzerClass::~SVLuminanceAnalyzerClass()
{
	CloseObject ();
}

BOOL SVLuminanceAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	SVImageClass* pSVImage(nullptr);
	bool bError(false);

	if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16095);
		bError = true;
	}
	else
	{
		pSVImage = getInputImage ();
		if (nullptr == pSVImage )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16096);
			bError = true;
		}
	}

	if (!bError)
	{
		msvlHistValueArraySize = 1 << (pSVImage->getPixelDepth() & SVBufferSize);
		msvplHistValues.resize( msvlHistValueArraySize );
		if (msvplHistValues.size() == 0)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16097);
			bError = true;
		}
	}

	if (!bError)
	{
		for( int i = 0; i < msvlHistValueArraySize; i++ )
		{
			msvplHistValues [i] = 0L;
		}
		SVDataBufferInfoClass svData;
		svData.Length = msvlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = msvHistResultID;
		if ( S_OK == SVImageProcessingClass::CreateDataBuffer( &svData ) )
		{
			msvHistResultID = svData.HBuffer.milResult;
		}

		if (msvHistResultID.empty())
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16098);
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

	msvLuminanceValue.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msvVarianceValue.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msvStdDevValue.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msvCalcStdDevValue.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	return m_isCreated;
}


BOOL SVLuminanceAnalyzerClass::CloseObject()
{

	msvplHistValues.clear();
	SVMatroxImageInterface l_lIntf;
	l_lIntf.Destroy( msvHistResultID );
	SVImageAnalyzerClass::CloseObject ();

	return TRUE;
}


SvOi::IObjectClass* SVLuminanceAnalyzerClass::GetResultObject()
{
	SvOi::IObjectClass* pAnalyzerResult = nullptr;

	// Find the result Object in Our List
	pAnalyzerResult = GetAt(0);

	return( pAnalyzerResult );
}

bool SVLuminanceAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVImageClass     *pInputImage(nullptr);
	__int64          sum(0);
	size_t           lI(0);
	long             lNbrPixels(0);
	double			 value = 0.0;

	HRESULT MatroxResult;
	bool  LastError(false);

	while (1)
	{
		///////////////////////////////////////////////////
		// Clear Variance and Standard Deviation
		///////////////////////////////////////////////////
		msvVarianceValue.SetValue( value, rRunStatus.m_lResultDataIndex );
		msvStdDevValue.SetValue( value, rRunStatus.m_lResultDataIndex );

		if( !__super::onRun( rRunStatus, pErrorMessages ) )
		{
			LastError = true;
			break;
		}

		pInputImage = getInputImage ();

		if( ! pInputImage )
		{
			SetInvalid ();            
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16099, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVSmartHandlePointer ImageHandle;

		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16100, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVImageBufferHandleImage l_MilBuffer;

		HRESULT l_Status = ImageHandle->GetData( l_MilBuffer );

		if( S_OK != l_Status )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16101, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVMatroxImageInterface l_lImageIntf;

		MatroxResult = l_lImageIntf.Histogram(msvHistResultID, l_MilBuffer.GetBuffer() );
		if( S_OK != MatroxResult )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16102, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MatroxResult = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );


		if( S_OK != MatroxResult )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16103, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;

		}

		for (lI = 0; lI < msvplHistValues.size(); lI++)
		{
			sum = sum + (msvplHistValues [lI] * lI);
			lNbrPixels = lNbrPixels + msvplHistValues [lI];
		}

		if (lNbrPixels == 0)
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16104, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if ( S_OK != msvLuminanceValue.SetValue( static_cast<long> (sum / lNbrPixels), rRunStatus.m_lResultDataIndex ) )
		{
			LastError = true;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16105, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16106, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if (calcStdDev)
		{
			double accumulatedSquares = 0.0;	// for calculating variance

			for (lI = 0; lI < msvplHistValues.size(); lI++)
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
				if (S_OK != msvVarianceValue.SetValue( value, rRunStatus.m_lResultDataIndex ))
				{
					LastError = true;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16107, GetUniqueObjectID() );
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
							SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16108, GetUniqueObjectID() );
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
				if (S_OK != msvStdDevValue.SetValue( value, rRunStatus.m_lResultDataIndex ))
				{
					LastError = true;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16109, GetUniqueObjectID() );
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
		SetInvalid ();
		rRunStatus.SetInvalid();
		return false;
	}

	return true;
}

double SVLuminanceAnalyzerClass::calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares )
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

