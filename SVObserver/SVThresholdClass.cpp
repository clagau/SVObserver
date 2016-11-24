//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVThresholdClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 Oct 2014 07:26:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVThresholdClass.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "SVUpperThresholdEquation.h"
#include "SVLowerThresholdEquation.h"
#include "SVAutoThresholdEquation.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOCore/SVDataBuffer.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVTool.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#define SV_OC_ERROR 0xc0000000   //SV Operator Class Error.

SV_IMPLEMENT_CLASS( SVThresholdClass, SVThresholdClassGuid )

SVThresholdClass::SVThresholdClass( SVObjectClass* POwner, int StringResourceID )
				 :SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

void SVThresholdClass::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVThresholdObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_upperThresh, SVUpperThresholdObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_lowerThresh, SVLowerThresholdObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_threshActivate, SVThresholdActivateObjectGuid, IDS_OBJECTNAME_THRESHOLDACTIVATE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_upperThreshActivate, SVUpperThresholdActivateObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLDACTIVATE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_lowerThreshActivate, SVLowerThresholdActivateObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLDACTIVATE, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_autoThreshold, SVAutoThresholdObjectGuid, IDS_OBJECTNAME_AUTOTHRESHOLD, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_blackBackground, SVBlackBackgroundObjectGuid, IDS_OBJECTNAME_BLACKBACKGROUND, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_dAutoThresholdMultiplier, SVAutoThresholdMultiplierObjectGuid, IDS_OBJECTNAME_AUTOTHRESHOLDMULTIPLIER, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_useExternalATM, SVUseExternalATMObjectGuid, IDS_OBJECTNAME_USE_EXTERN_ATM, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_useExternalLT, SVUseExternalLTObjectGuid, IDS_OBJECTNAME_USE_EXTERN_LT, false, SVResetItemOwner );
	RegisterEmbeddedObject( &m_useExternalUT, SVUseExternalUTObjectGuid, IDS_OBJECTNAME_USE_EXTERN_UT, false, SVResetItemOwner );

	// Set Embedded defaults
	m_upperThresh.SetDefaultValue( SvOi::cDefaultToolUpperThreshold, TRUE );
	m_lowerThresh.SetDefaultValue( SvOi::cDefaultToolLowerThreshold, TRUE );
	m_threshActivate.SetDefaultValue( SvOi::cDefaultToolThresholdActivate, FALSE );
	m_upperThreshActivate.SetDefaultValue( SvOi::cDefaultToolUpperThresholdActivate, TRUE );
	m_lowerThreshActivate.SetDefaultValue( SvOi::cDefaultToolLowerThresholdActivate, TRUE );

	m_useExternalATM.SetDefaultValue( FALSE, TRUE );
	m_useExternalLT.SetDefaultValue( FALSE, TRUE );
	m_useExternalUT.SetDefaultValue( FALSE, TRUE );

	m_autoThreshold.SetDefaultValue( FALSE, TRUE );
	m_blackBackground.SetDefaultValue( FALSE, TRUE );

	m_dAutoThresholdMultiplier.SetDefaultValue( 1.0, TRUE );
		
	m_histValueArraySize = 0;
	m_pixelNumber		   = 0;

	// Add equation friends...
	SVUpperThresholdEquationClass* pUTFriend = new SVUpperThresholdEquationClass( this );
	if( pUTFriend )
		AddFriend( pUTFriend->GetUniqueObjectID() );
	SVLowerThresholdEquationClass* pLTFriend = new SVLowerThresholdEquationClass( this );
	if( pLTFriend )
		AddFriend( pLTFriend->GetUniqueObjectID() );
	SVAutoThresholdEquationClass* pATFriend = new SVAutoThresholdEquationClass( this );
	if( pATFriend )
		AddFriend( pATFriend->GetUniqueObjectID() );

	// Identify our input type needs...

	m_inputUT.SetInputObjectType( SVUpperThresholdEquationResultObjectGuid, SVDoubleValueObjectType );
	m_inputUT.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputUT, _T( "UpperThreshold" ) );

	m_inputLT.SetInputObjectType( SVLowerThresholdEquationResultObjectGuid, SVDoubleValueObjectType );
	m_inputLT.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputLT, _T( "LowerThreshold" ) );

	m_inputATM.SetInputObjectType( SVAutoThresholdEquationResultObjectGuid, SVDoubleValueObjectType );
	m_inputATM.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputATM, _T( "AutoThreshold" ) );

	addDefaultInputObjects();
}

SVThresholdClass::~SVThresholdClass()
{
	CloseObject();
}

BOOL SVThresholdClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVUnaryImageOperatorClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_threshActivate.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_blackBackground.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_upperThreshActivate.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lowerThreshActivate.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_useExternalLT.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_useExternalUT.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	
	m_autoThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_useExternalATM.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_upperThresh.ObjectAttributesAllowedRef() |= SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lowerThresh.ObjectAttributesAllowedRef() |= SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_dAutoThresholdMultiplier.ObjectAttributesAllowedRef() |= SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	bOk = S_OK == Rebuild();

	m_isCreated = bOk;

	return bOk;
}

BOOL SVThresholdClass::CloseObject()
{
	if( !m_histResultID.empty() )
	{
		
		SVMatroxImageInterface::Destroy( m_histResultID );
	}

	m_HistValueArray.clear();
	m_histValueArraySize = 0;

	return SVUnaryImageOperatorClass::CloseObject();
}

HRESULT SVThresholdClass::ResetObject()
{
	HRESULT l_hrOk = SVUnaryImageOperatorClass::ResetObject();

	if( S_OK != Rebuild() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVBoolValueObjectClass& SVThresholdClass::GetThresholdActivateAttribute()
{
	return m_threshActivate;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalUT()
{
	if( m_inputUT.IsConnected() && m_inputUT.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) m_inputUT.GetInputObjectInfo().PObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalLT()
{
	if( m_inputLT.IsConnected() && m_inputLT.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) m_inputLT.GetInputObjectInfo().PObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalATM()
{
	if( m_inputATM.IsConnected() && m_inputATM.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) m_inputATM.GetInputObjectInfo().PObject;

	return nullptr;
}

BOOL SVThresholdClass::OnValidate() 
{
	return SVUnaryImageOperatorClass::OnValidate();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVThresholdClass::onRun( BOOL First, 
							  SVSmartHandlePointer RInputImageHandle, 
							  SVSmartHandlePointer ROutputImageHandle, 
							  SVRunStatusClass& RRunStatus )
{ 
	// Binarizing: lowerThresh <= x <= m_upperThresh		--> 255 
	//	 		   otherwise							--> 0

	// Auto Threshold Multiplier Range					--> 0.0 ... 2.0

	
	SVMatroxImageInterface::SVStatusCode l_Code = 0;
	SVMatroxImageInterface::SVStatusCode l_RetCode = 0;


	SVImageBufferHandleImage l_InMilHandle;
	SVImageBufferHandleImage l_OutMilHandle;

	while (1)
	{
		if (!m_pCurrentUIOPL || RInputImageHandle.empty() || ROutputImageHandle.empty() )
		{
			l_Code = -15401;
			break;
		}

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		BOOL bThreshActive	    = FALSE;
		BOOL bAutoThreshold     = FALSE;
		
		// Get the state of the threshold activates
		m_threshActivate.GetValue( bThreshActive );
		m_autoThreshold.GetValue( bAutoThreshold );

		if (!bThreshActive)
		{
//-			Threshold not enabled.
//-			15002 is not negative because the threshold not being enabled is not
//-			considered an error.
			l_Code = 15402;
			break;
		}

		if (bAutoThreshold || saveHistogram)
		{
				// Read histogram...
				l_RetCode = SVMatroxImageInterface::Histogram( m_histResultID,
					( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer() );
				if (l_RetCode & SV_OC_ERROR)
				{
					l_Code = -15404;
					break;
				}


				l_RetCode = SVMatroxImageInterface::GetResult( m_histResultID, m_HistValueArray );
				if (l_RetCode & SV_OC_ERROR)
				{
					l_Code = -15405;
					break;
				}
		}


		long lower = 0;
		long upper = 0;
		BOOL bUpperThreshActive = FALSE;
		BOOL bLowerThreshActive = FALSE;
		BOOL bBlackBackground   = FALSE;
		BOOL bUseExternalLT     = FALSE;
		BOOL bUseExternalUT     = FALSE;
		BOOL bUseExternalATM    = FALSE;

		// Get current Threshold Flags...
		m_upperThreshActivate.GetValue( bUpperThreshActive );
		m_lowerThreshActivate.GetValue( bLowerThreshActive );
		m_blackBackground.GetValue( bBlackBackground );
		m_useExternalLT.GetValue( bUseExternalLT );
		m_useExternalUT.GetValue( bUseExternalUT );
		m_useExternalATM.GetValue( bUseExternalATM );
			
		if( bUseExternalLT || bUseExternalUT || bUseExternalATM )
		{
			// Run equation friends...
			SVRunStatusClass runStatus;

			runStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
			runStatus.Images = RRunStatus.Images;
			runStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

			runFriends( runStatus );
			if( ! runStatus.IsValid() )
			{
				l_Code = -15403;
				break;
			}
		}

		if( bAutoThreshold )
		{
			ASSERT( m_HistValueArray.size() > 0 );
			ASSERT( m_histValueArraySize );

			if( /*m_pHistValueArray &&*/ m_histValueArraySize && m_pixelNumber )
			{
				__int64 sum = 0;
				for(  int i = 0; i < m_histValueArraySize; ++ i )
					sum += ( ( __int64 ) m_HistValueArray[ i ] ) * ( ( __int64 ) i );

				// Calculate average and auto threshold value...
				double dTemp;
				m_dAutoThresholdMultiplier.GetValue( dTemp );
				if( bUseExternalATM )
				{
					SVDoubleValueObjectClass* pExtATMValue = getExternalATM();
					if( pExtATMValue )
					{
						pExtATMValue->GetValue( dTemp );
						// Range check...
						if( dTemp < 0.0 )
							dTemp = 0.0;
						if( dTemp > 2.0 )
							dTemp = 2.0;
						m_dAutoThresholdMultiplier.SetValue( RRunStatus.m_lResultDataIndex, dTemp );
					}
				}

				long m_autoThreshold = ( long ) ( ( ( double ) ( sum / m_pixelNumber ) ) * dTemp );

				if( bBlackBackground )
				{
					upper = 255L;
					lower = m_autoThreshold;
					if( lower > 255L )
						lower = 255L;
				}
				else
				{
					upper = m_autoThreshold;
					if( upper > 255L )
						upper = 255L;
					lower = 0L;
				}
					m_upperThresh.SetValue( RRunStatus.m_lResultDataIndex, upper );
					m_lowerThresh.SetValue( RRunStatus.m_lResultDataIndex, lower );
			} // if( /*m_pHistValueArray &&*/ m_histValueArraySize && m_pixelNumber )
		} // if( bAutoThreshold )
		else
		{
			// Get current Threshold values...
			m_lowerThresh.GetValue( lower );
			m_upperThresh.GetValue( upper );

			if( bUpperThreshActive )
			{
				if( bUseExternalUT )
				{
					double dUpp = 0.0;
					SVDoubleValueObjectClass* pExtUTValue = getExternalUT();
					if( pExtUTValue )
					{
						pExtUTValue->GetValue( dUpp );
						upper = ( long ) dUpp;
						// Range check...
						if( upper < 0 )
							upper = 0;
						if( upper > 255 )
							upper = 255;
						m_upperThresh.SetValue( RRunStatus.m_lResultDataIndex, upper );
					}
				}
			}
			else
			{
				upper = 255L;
			}
			
			if( bLowerThreshActive )
			{
				if( bUseExternalLT )
				{
					double dLow = 0.0;
					SVDoubleValueObjectClass* pExtLTValue = getExternalLT();
					if( pExtLTValue )
					{
						pExtLTValue->GetValue( dLow );
						lower = ( long ) dLow;
						// Range check...
						if( lower < 0 )
							lower = 0;
						if( lower > 255 )
							lower = 255;
						m_lowerThresh.SetValue( RRunStatus.m_lResultDataIndex, lower );
					}
				}
			}
			else
			{
				lower = 0L;			
			}
		}
				
		if( upper >= lower )
		{
			l_Code = SVMatroxImageInterface::Binarize(l_OutMilHandle.GetBuffer(), 
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				SVECondInRange,
				lower,
				upper );
			if (l_RetCode & SV_OC_ERROR)
			{
				l_Code = -15406;
				break;
			}

		} // if( upper >= lower )
		else
		{
			l_Code = SVMatroxImageInterface::Binarize(l_OutMilHandle.GetBuffer(), 
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				SVECondOutRange,
				upper,
				lower );
			if (l_RetCode & SV_OC_ERROR)
			{
				l_Code = -15407;
				break;
			}
		}
		// Success...
		break;
	} // while (1)

	// Signal threshold operator was not running...
	if (l_Code & SV_OC_ERROR)
	{
		assert (0);
		SetInvalid();
		RRunStatus.SetInvalid();

		SVStringArray msgList;
		msgList.push_back(_T("SVThresholdClass::onRun"));
		
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (l_Code), SvOi::Tid_ErrorIn, msgList, SvStl::SourceFileParams(StdMessageParams) );

		RRunStatus.SetCriticalFailure();

		return FALSE;
	}
	else
	if (l_Code == 15402)
	{
//-		15402 indicates that Thresholds were not enabled!
//-		False indicates that Thresholds were not run.... for whatever reason.

		return FALSE;
	}
	else 
		return TRUE;
}

HRESULT SVThresholdClass::Rebuild()
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == GetTool() )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		// &&&
		SVDataBufferInfoClass svData;

		long l_lPixelDepth = getReferenceImage()->getPixelDepth();

		if( l_lPixelDepth < 0 )
		{
			l_lPixelDepth = 8;
		}

		svData.Length = 1 << ( l_lPixelDepth & SVBufferSize );
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_histResultID;

		if( !m_histResultID.empty() )
		{
			l_hrOk = SVImageProcessingClass::DestroyDataBuffer( &svData );
		}

		if ( S_OK == l_hrOk )
		{
			l_hrOk = SVImageProcessingClass::CreateDataBuffer( &svData );
		}

		if ( S_OK == l_hrOk )
		{
			m_histResultID = svData.HBuffer.milResult;
		}

		if( !m_histResultID.empty() )
		{
				m_HistValueArray.clear();
				m_histValueArraySize = 0;

			m_histValueArraySize = svData.Length;

			m_HistValueArray.resize( m_histValueArraySize ) ;
			SVImageExtentClass l_svExtents;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
			if ( pTool && S_OK == pTool->GetImageExtent( l_svExtents ) &&
				S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_dWidth ) &&
				S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight ) )
			{
				// Recalculate pixel number...
				m_pixelNumber = ( ( __int64 ) l_dWidth ) * ( ( __int64 ) l_dHeight );
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;

			m_histValueArraySize = 0;
		}
	}

	BOOL l_bAutoValue = FALSE;
	BOOL l_bATMValue = FALSE;

	BOOL l_bLTValue = FALSE;
	BOOL l_bUTValue = FALSE;

	m_autoThreshold.GetValue( l_bAutoValue );
	m_useExternalATM.GetValue( l_bATMValue );

	m_useExternalLT.GetValue( l_bLTValue );
	m_useExternalUT.GetValue( l_bUTValue );

	if ( l_bATMValue )
	{
		m_dAutoThresholdMultiplier.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_dAutoThresholdMultiplier.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	if ( l_bAutoValue || l_bLTValue )
	{
		m_lowerThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_lowerThresh.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	if ( l_bAutoValue || l_bUTValue )
	{
		m_upperThresh.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_upperThresh.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	return l_hrOk;
}

