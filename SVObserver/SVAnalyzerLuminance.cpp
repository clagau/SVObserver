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
#include "SVDataBuffer.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVResultLong.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
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
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLuminanceAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvLuminanceValue, 
		SVLuminanceValueObjectGuid, 
		IDS_OBJECTNAME_LUMINANCE,
		false, SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvCalcStdDevValue,
		SVCalcStdDevObjectGuid,
		IDS_OBJECTNAME_CALCSTDDEV,
		false, SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvVarianceValue,
		SVStatVarianceObjectGuid,
		IDS_OBJECTNAME_STATVARIANCE,
		false, SVResetItemNone );

	// Register Embedded Objects
	RegisterEmbeddedObject( 
		&msvStdDevValue,
		SVStatStdDevObjectGuid,
		IDS_OBJECTNAME_STATSTDDEV,
		false, SVResetItemNone );

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


BOOL SVLuminanceAnalyzerClass::OnValidate ()
{
	bool Valid(true);
	if (!SVImageAnalyzerClass::OnValidate ())
	{
		//Error code set inside SVImageAnalyzerClass::OnValidate ()
		Valid = false;
	}
	else if (msvHistResultID.empty())
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16094);
		Valid = false;	
	}
	else if (0 == msvplHistValues.size())
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16095);
		Valid = false;	
	}

	if (false == Valid)
   {
       SetInvalid();
       return false;
   }
   m_isObjectValid.SetValue (1, true);
   return true;
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
		if ( S_OK == SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) )
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

	msvLuminanceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvVarianceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvStdDevValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvCalcStdDevValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

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


SVResultClass* SVLuminanceAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = nullptr;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	return( pAnalyzerResult );
}

BOOL SVLuminanceAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVImageClass     *pInputImage(nullptr);
	__int64          sum(0);
	size_t           lI(0);
	long             lNbrPixels(0);
	double			 value = 0.0;

	SVMatroxImageInterface::SVStatusCode l_Code;
	bool  LastError(false);

	while (1)
	{
		///////////////////////////////////////////////////
		// Clear Variance and Standard Deviation
		///////////////////////////////////////////////////
		msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, value );
		msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, value );

		if( !SVImageAnalyzerClass::onRun( RRunStatus ) )
		{
			break;
		}

		pInputImage = getInputImage ();

		if( ! pInputImage )
		{
			SetInvalid ();            

			SvStl::MessageMgrStd  Ex( SvStl::LogAndDisplay );
			Ex.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16099);
			LastError = true;
			break;
		}

		SVSmartHandlePointer ImageHandle;

		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			SvStl::MessageMgrStd  Ex( SvStl::LogAndDisplay );
			Ex.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16100);
			LastError = true;
			break;
		}

		SVImageBufferHandleImage l_MilBuffer;

		HRESULT l_Status = ImageHandle->GetData( l_MilBuffer );

		if( S_OK != l_Status )
		{
			SvStl::MessageMgrStd  Ex( SvStl::LogAndDisplay );
			Ex.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16101);
			LastError = true;
			break;
		}

		SVMatroxImageInterface l_lImageIntf;

		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilBuffer.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16102);
			LastError = true;
			break;
		}

		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );


		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16103);
			LastError = true;
			break;

		}

		for (lI = 0; lI < msvplHistValues.size(); lI++)
		{
			sum = sum + (msvplHistValues [lI] * lI);
			lNbrPixels = lNbrPixels + msvplHistValues [lI];
		}

		if (lNbrPixels == 0)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16104);
			LastError = true;
			break;
		}

		if ( S_OK != msvLuminanceValue.SetValue( RRunStatus.m_lResultDataIndex, (long)(sum / lNbrPixels) ) )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16105);
			LastError = true;
			break;
		}

		///////////////////////////////////////////////////
		// Get calculate StdDeviation flag
		///////////////////////////////////////////////////
		BOOL calcStdDev = FALSE;

		if ( S_OK != msvCalcStdDevValue.GetValue( calcStdDev ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16106);
			LastError = true;
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
				if (S_OK != msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, value ))
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16107);
					LastError = true;
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
						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16108);
						LastError = true;
						break;
					}
					else
						value = fabs( sqrt( value ) );
				}
				///////////////////////////////////////////////////////////////////
				// Save Standard Deviation
				///////////////////////////////////////////////////////////////////
				if (S_OK != msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, value ))
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16109);
					LastError = true;
					break;
				}
			}
		}

		break;
	}

	if(	LastError )  
	{
		SetInvalid ();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	return TRUE;
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

