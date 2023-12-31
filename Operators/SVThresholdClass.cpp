//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVUpperThresholdEquation.h"
#include "SVLowerThresholdEquation.h"
#include "SVAutoThresholdEquation.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#define SV_OC_ERROR 0xc0000000   //SV Operator Class Error.

SV_IMPLEMENT_CLASS(SVThresholdClass, SvPb::ThresholdClassId)

SVThresholdClass::SVThresholdClass( SVObjectClass* POwner, int StringResourceID )
				 :SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

void SVThresholdClass::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVThresholdObjectType;

	// Register Embedded Object(s)
	RegisterEmbeddedObject( &m_upperThresh, SvPb::UpperThresholdEId, IDS_OBJECTNAME_UPPERTHRESHOLD, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_lowerThresh, SvPb::LowerThresholdEId, IDS_OBJECTNAME_LOWERTHRESHOLD, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_threshActivate, SvPb::ThresholdActivateEId, IDS_OBJECTNAME_THRESHOLDACTIVATE, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_upperThreshActivate, SvPb::UpperThresholdActivateEId, IDS_OBJECTNAME_UPPERTHRESHOLDACTIVATE, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_lowerThreshActivate, SvPb::LowerThresholdActivateEId, IDS_OBJECTNAME_LOWERTHRESHOLDACTIVATE, false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject( &m_autoThreshold, SvPb::AutoThresholdEId, IDS_OBJECTNAME_AUTOTHRESHOLD, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_blackBackground, SvPb::BlackBackgroundEId, IDS_OBJECTNAME_BLACKBACKGROUND, false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject( &m_dAutoThresholdMultiplier, SvPb::AutoThresholdMultiplierEId, IDS_OBJECTNAME_AUTOTHRESHOLDMULTIPLIER, false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject( &m_useExternalATM, SvPb::UseExternalATMEId, IDS_OBJECTNAME_USE_EXTERN_ATM, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_useExternalLT, SvPb::UseExternalLTEId, IDS_OBJECTNAME_USE_EXTERN_LT, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_useExternalUT, SvPb::UseExternalUTEId, IDS_OBJECTNAME_USE_EXTERN_UT, false, SvOi::SVResetItemOwner, true);

	// Set Embedded defaults
	m_upperThresh.SetDefaultValue( SvDef::cDefaultToolUpperThreshold );
	m_lowerThresh.SetDefaultValue( SvDef::cDefaultToolLowerThreshold );
	m_threshActivate.SetDefaultValue( SvDef::cDefaultToolThresholdActivate, false );
	m_upperThreshActivate.SetDefaultValue( SvDef::cDefaultToolUpperThresholdActivate );
	m_lowerThreshActivate.SetDefaultValue( SvDef::cDefaultToolLowerThresholdActivate );

	m_useExternalATM.SetDefaultValue(BOOL(false));
	m_useExternalLT.SetDefaultValue(BOOL(false));
	m_useExternalUT.SetDefaultValue(BOOL(false));

	m_autoThreshold.SetDefaultValue(BOOL(false));
	m_blackBackground.SetDefaultValue( BOOL(false) );

	m_dAutoThresholdMultiplier.SetDefaultValue( 1.0 );
		
	m_histValueArraySize = 0;
	m_pixelNumber		   = 0;

	// Add equation friends...
	SVUpperThresholdEquation* pUTFriend = new SVUpperThresholdEquation( this );
	if( pUTFriend )
	{
		AddFriend( pUTFriend );
	}
	SVLowerThresholdEquation* pLTFriend = new SVLowerThresholdEquation( this );
	if( pLTFriend )
	{
		AddFriend( pLTFriend );
	}
	SVAutoThresholdEquation* pATFriend = new SVAutoThresholdEquation( this );
	if( pATFriend )
	{
		AddFriend( pATFriend );
	}

	// Identify our input type needs...

	m_inputUT.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::UpperThresholdEquationResultEId);
	registerInputObject( &m_inputUT, _T( "UpperThreshold" ), SvPb::UpperThresholdInputEId);
	m_inputUT.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_inputLT.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::LowerThresholdEquationResultEId);
	registerInputObject( &m_inputLT, _T( "LowerThreshold" ), SvPb::LowerThresholdInputEId);
	m_inputLT.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_inputATM.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::AutoThresholdEquationResultEId);
	registerInputObject( &m_inputATM, _T( "AutoThreshold" ), SvPb::AutoThresholdInputEId);
	m_inputATM.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
}

SVThresholdClass::~SVThresholdClass()
{
	SVThresholdClass::CloseObject();
}

bool SVThresholdClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_threshActivate.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_blackBackground.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	m_upperThreshActivate.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_lowerThreshActivate.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_useExternalLT.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_useExternalUT.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	
	m_autoThreshold.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	m_useExternalATM.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	m_upperThresh.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	m_lowerThresh.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	m_dAutoThresholdMultiplier.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );

	bOk = Rebuild() && bOk;

	for (size_t j = 0; j < m_friendList.size(); j++)
	{
		if (nullptr != m_friendList[j])
		{
			switch (m_friendList[j]->GetObjectSubType())
			{
				case SvPb::SVUpperThresholdEquationObjectType:
					m_pUTEquation = m_friendList[j];
					break;
				case SvPb::SVLowerThresholdEquationObjectType:
					m_pLTEquation = m_friendList[j];
					break;
				case SvPb::SVAutoThresholdEquationObjectType:
					m_pATEquation = m_friendList[j];
					break;
				default:
					break;
			}
		}
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVThresholdClass::CloseObject()
{
	SVMatroxImageInterface::Destroy( m_histResultID );

	m_HistValueArray.clear();
	m_histValueArraySize = 0;

	return SVUnaryImageOperatorClass::CloseObject();
}

bool SVThresholdClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	std::initializer_list<SvOl::InputObject*> inputList
	{
		&m_inputATM,
		&m_inputUT,
		&m_inputLT
	};
	
	for (auto* pEntry : inputList)
	{
		pEntry->validateInput();
		// pEntry cannot be nullptr as the InputList are member variable addresses
		if (nullptr == pEntry->getInput<SvVol::SVDoubleValueObjectClass>())
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}
	}

	if( Result && !Rebuild() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	BOOL bThreshActive = false;
	m_threshActivate.GetValue(bThreshActive);
	m_autoThreshold.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive ? SvOi::AddAttribute : SvOi::RemoveAttribute);

	BOOL bAutoThreshold, bUseAutoExternal, bUpperThreshActive, bUseExternalUT, bLowerThreshActive, bUseExternalLT;
	m_autoThreshold.GetValue(bAutoThreshold);
	m_blackBackground.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && bAutoThreshold ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_dAutoThresholdMultiplier.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && bAutoThreshold ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_useExternalATM.GetValue(bUseAutoExternal);
	m_useExternalATM.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && bAutoThreshold ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_upperThreshActivate.GetValue(bUpperThreshActive);
	m_upperThreshActivate.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_useExternalUT.GetValue(bUseExternalUT);
	m_useExternalUT.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && bUpperThreshActive ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_lowerThreshActivate.GetValue(bLowerThreshActive);
	m_lowerThreshActivate.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_useExternalLT.GetValue(bUseExternalLT);
	m_useExternalLT.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && bLowerThreshActive ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_upperThresh.setSaveValueFlag(!bAutoThreshold && bUpperThreshActive && !bUseExternalUT);
	m_upperThresh.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && false == bUseExternalUT && bUpperThreshActive ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	m_lowerThresh.setSaveValueFlag(!bAutoThreshold && bLowerThreshActive && !bUseExternalLT);
	m_lowerThresh.SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && false == bUseExternalLT && bLowerThreshActive ? SvOi::AddAttribute : SvOi::RemoveAttribute);

	if (nullptr != m_pATEquation)
	{
		m_pATEquation->SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && bAutoThreshold && bUseAutoExternal ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	}

	if (nullptr != m_pUTEquation)
	{
		m_pUTEquation->SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && bUpperThreshActive && bUseExternalUT ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	}

	if (nullptr != m_pLTEquation)
	{
		m_pLTEquation->SetObjectAttributesAllowed(SvPb::viewable, bThreshActive && false == bAutoThreshold && bLowerThreshActive && bUseExternalLT ? SvOi::AddAttribute : SvOi::RemoveAttribute);
	}

	return Result;
}

SvVol::SVBoolValueObjectClass& SVThresholdClass::GetThresholdActivateAttribute()
{
	return m_threshActivate;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVThresholdClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle,
							  SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	// Binarizing: lowerThresh <= x <= m_upperThresh		--> 255 
	//	 		   otherwise							--> 0

	// Auto Threshold Multiplier Range					--> 0.0 ... 2.0

	
	HRESULT l_Code = 0;
	HRESULT l_RetCode = 0;

	while (1)
	{
		if (!m_pCurrentUIOPL || nullptr == rInputImageHandle || nullptr == rOutputImageHandle )
		{
			l_Code = -15401;
			break;
		}

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
					( First == TRUE ) ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer() );
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
			SvIe::RunStatus runStatus;

			runStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
			runStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

			runFriends( runStatus );
			rRunStatus.m_triggerRecord = std::move(runStatus.m_triggerRecord);
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
					SvVol::SVDoubleValueObjectClass* pExtATMValue = m_inputATM.getInput<SvVol::SVDoubleValueObjectClass>(true);
					if(nullptr != pExtATMValue)
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

				long autoThreshold = ( long ) ( ( ( double ) ( sum / m_pixelNumber ) ) * dTemp );

				if( bBlackBackground )
				{
					upper = 255L;
					lower = autoThreshold;
					if( lower > 255L )
						lower = 255L;
				}
				else
				{
					upper = autoThreshold;
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
					SvVol::SVDoubleValueObjectClass* pExtUTValue = m_inputUT.getInput<SvVol::SVDoubleValueObjectClass>(true);
					if(nullptr != pExtUTValue)
					{
						double dUpper = 0.0;
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
					SvVol::SVDoubleValueObjectClass* pExtLTValue = m_inputLT.getInput<SvVol::SVDoubleValueObjectClass>(true);
					if(nullptr != pExtLTValue)
					{
						double dLower = 0.0;
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
			l_Code = SVMatroxImageInterface::Binarize(rOutputImageHandle->GetBuffer(),
				( First == TRUE ) ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
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
			l_Code = SVMatroxImageInterface::Binarize(rOutputImageHandle->GetBuffer(),
				( First == TRUE ) ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
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
		rRunStatus.SetInvalid();

		SvDef::StringVector msgList;
		msgList.push_back(_T("SVThresholdClass::onRun"));
		
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( static_cast<DWORD> (l_Code), SvStl::Tid_ErrorIn, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
		SvIe::SVDataBufferInfoClass svData;

		long l_lPixelDepth = (nullptr != getOutputImage()) ? getOutputImage()->getPixelDepth() : 0;

		if( l_lPixelDepth < 0 )
		{
			l_lPixelDepth = 8;
		}

		svData.Length = 1 << ( l_lPixelDepth & SVBufferSize );
		svData.Type = SvIe::SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_histResultID;

		Result = (S_OK == SvIe::SVImageProcessingClass::DestroyDataBuffer( &svData ));
		Result = Result && (S_OK == SvIe::SVImageProcessingClass::CreateDataBuffer( &svData ));

		if ( Result )
		{
			m_histResultID = svData.HBuffer.milResult;
		}

		if( M_NULL != m_histResultID )
		{
				m_HistValueArray.clear();
				m_histValueArraySize = 0;

			m_histValueArraySize = svData.Length;

			m_HistValueArray.resize( m_histValueArraySize ) ;
			SVImageExtentClass l_svExtents;

			double l_dWidth = 0.0;
			double l_dHeight = 0.0;

			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
			if ( nullptr != pTool && 
				S_OK == pTool->GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, l_dWidth ) &&
				S_OK == pTool->GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, l_dHeight ) )
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
	m_dAutoThresholdMultiplier.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

	AddRemoveType = (l_bAutoValue || l_bLTValue) ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
	m_lowerThresh.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

	AddRemoveType = (l_bAutoValue || l_bUTValue) ? SvOi::SetAttributeType::RemoveAttribute : SvOi::SetAttributeType::AddAttribute;
	m_upperThresh.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

	return Result;
}

} //namespace SvOp
