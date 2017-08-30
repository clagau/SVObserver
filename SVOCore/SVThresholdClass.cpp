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
#include "SVDataBuffer.h"
#include "SVImageProcessingClass.h"
#include "SVTool.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVObjectLibrary\SVClsIds.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVThresholdObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_upperThresh, SVUpperThresholdObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_lowerThresh, SVLowerThresholdObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_threshActivate, SVThresholdActivateObjectGuid, IDS_OBJECTNAME_THRESHOLDACTIVATE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_upperThreshActivate, SVUpperThresholdActivateObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLDACTIVATE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_lowerThreshActivate, SVLowerThresholdActivateObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLDACTIVATE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_autoThreshold, SVAutoThresholdObjectGuid, IDS_OBJECTNAME_AUTOTHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_blackBackground, SVBlackBackgroundObjectGuid, IDS_OBJECTNAME_BLACKBACKGROUND, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_dAutoThresholdMultiplier, SVAutoThresholdMultiplierObjectGuid, IDS_OBJECTNAME_AUTOTHRESHOLDMULTIPLIER, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_useExternalATM, SVUseExternalATMObjectGuid, IDS_OBJECTNAME_USE_EXTERN_ATM, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_useExternalLT, SVUseExternalLTObjectGuid, IDS_OBJECTNAME_USE_EXTERN_LT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_useExternalUT, SVUseExternalUTObjectGuid, IDS_OBJECTNAME_USE_EXTERN_UT, false, SvOi::SVResetItemOwner );

	// Set Embedded defaults
	m_upperThresh.SetDefaultValue( SvOi::cDefaultToolUpperThreshold );
	m_lowerThresh.SetDefaultValue( SvOi::cDefaultToolLowerThreshold );
	m_threshActivate.SetDefaultValue( SvOi::cDefaultToolThresholdActivate, false );
	m_upperThreshActivate.SetDefaultValue( SvOi::cDefaultToolUpperThresholdActivate );
	m_lowerThreshActivate.SetDefaultValue( SvOi::cDefaultToolLowerThresholdActivate );

	m_useExternalATM.SetDefaultValue(BOOL(false));
	m_useExternalLT.SetDefaultValue(BOOL(false));
	m_useExternalUT.SetDefaultValue(BOOL(false));

	m_autoThreshold.SetDefaultValue(BOOL(false));
	m_blackBackground.SetDefaultValue( BOOL(false) );

	m_dAutoThresholdMultiplier.SetDefaultValue( 1.0 );
		
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

bool SVThresholdClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject( pCreateStructure );

	// Set / Reset Printable Flag
	m_threshActivate.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_blackBackground.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_upperThreshActivate.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lowerThreshActivate.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_useExternalLT.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_useExternalUT.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	
	m_autoThreshold.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_useExternalATM.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_upperThresh.SetObjectAttributesAllowed( SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_lowerThresh.SetObjectAttributesAllowed( SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_dAutoThresholdMultiplier.SetObjectAttributesAllowed( SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );

	bOk = Rebuild();

	m_isCreated = bOk;

	return bOk;
}

bool SVThresholdClass::CloseObject()
{
	if( !m_histResultID.empty() )
	{
		
		SVMatroxImageInterface::Destroy( m_histResultID );
	}

	m_HistValueArray.clear();
	m_histValueArraySize = 0;

	return SVUnaryImageOperatorClass::CloseObject();
}

bool SVThresholdClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if( !Rebuild() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

SVBoolValueObjectClass& SVThresholdClass::GetThresholdActivateAttribute()
{
	return m_threshActivate;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalUT()
{
	if( m_inputUT.IsConnected() && m_inputUT.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) m_inputUT.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalLT()
{
	if( m_inputLT.IsConnected() && m_inputLT.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) m_inputLT.GetInputObjectInfo().m_pObject;

	return nullptr;
}

SVDoubleValueObjectClass* SVThresholdClass::getExternalATM()
{
	if( m_inputATM.IsConnected() && m_inputATM.GetInputObjectInfo().m_pObject )
		return ( SVDoubleValueObjectClass* ) m_inputATM.GetInputObjectInfo().m_pObject;

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVThresholdClass::onRun( bool First, 
							  SVSmartHandlePointer RInputImageHandle, 
							  SVSmartHandlePointer ROutputImageHandle, 
							  SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	// Binarizing: lowerThresh <= x <= m_upperThresh		--> 255 
	//	 		   otherwise							--> 0

	// Auto Threshold Multiplier Range					--> 0.0 ... 2.0

	
	HRESULT l_Code = 0;
	HRESULT l_RetCode = 0;


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

		BOOL bThreshActive	    = false;
		BOOL bAutoThreshold     = false;
		
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
		BOOL bUpperThreshActive = false;
		BOOL bLowerThreshActive = false;
		BOOL bBlackBackground   = false;
		BOOL bUseExternalLT     = false;
		BOOL bUseExternalUT     = false;
		BOOL bUseExternalATM    = false;

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

			runStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
			runStatus.Images = rRunStatus.Images;
			runStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

			runFriends( runStatus );
			if( ! runStatus.IsValid() )
			{
				l_Code = -15403;
				break;
			}
		}

		if( bAutoThreshold )
		{

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
						m_dAutoThresholdMultiplier.SetValue(dTemp);
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
					m_upperThresh.SetValue(upper);
					m_lowerThresh.SetValue(lower);
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
					double dUpper = 0.0;
					SVDoubleValueObjectClass* pExtUTValue = getExternalUT();
					if( pExtUTValue )
					{
						pExtUTValue->GetValue( dUpper );
						upper = static_cast<long> (dUpper);
						// Range check...
						upper = (0 > upper) ? 0 : upper;
						upper = (255 < upper) ? 255 : upper;
						m_upperThresh.SetValue(upper);
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
					double dLower = 0.0;
					SVDoubleValueObjectClass* pExtLTValue = getExternalLT();
					if( pExtLTValue )
					{
						pExtLTValue->GetValue( dLower );
						lower = static_cast<LONG> (dLower);
						// Range check...
						lower = (0 > lower) ? 0 : lower;
						lower = (255 < lower) ? 255 : lower;
						m_lowerThresh.SetValue(lower);
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
		rRunStatus.SetInvalid();

		SVStringVector msgList;
		msgList.push_back(_T("SVThresholdClass::onRun"));
		
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( static_cast<DWORD> (l_Code), SvStl::Tid_ErrorIn, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetCriticalFailure();

		return false;
	}
	else if (l_Code == 15402)
	{
//-		15402 indicates that Thresholds were not enabled!
//-		False indicates that Thresholds were not run.... for whatever reason.

		return false;
	}


	return true;
}

bool SVThresholdClass::Rebuild()
{
	bool Result = ( nullptr != GetTool() );

	if( Result )
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
			Result = (S_OK == SVImageProcessingClass::DestroyDataBuffer( &svData ));
		}

		Result = Result && (S_OK == SVImageProcessingClass::CreateDataBuffer( &svData ));

		if ( Result )
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
				Result = false;
			}
		}
		else
		{
			Result = false;

			m_histValueArraySize = 0;
		}
	}

	BOOL l_bAutoValue = false;
	BOOL l_bATMValue = false;

	BOOL l_bLTValue = false;
	BOOL l_bUTValue = false;

	m_autoThreshold.GetValue( l_bAutoValue );
	m_useExternalATM.GetValue( l_bATMValue );

	m_useExternalLT.GetValue( l_bLTValue );
	m_useExternalUT.GetValue( l_bUTValue );

	SvOi::SetAttributeType AddRemoveType = l_bATMValue ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
	m_dAutoThresholdMultiplier.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	AddRemoveType = (l_bAutoValue || l_bLTValue) ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
	m_lowerThresh.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	AddRemoveType = (l_bAutoValue || l_bUTValue) ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
	m_upperThresh.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	return Result;
}

