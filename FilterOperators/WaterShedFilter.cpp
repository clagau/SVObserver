// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 16:30:20  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "WaterShedFilter.h"
#include "Definitions/TextDefineSVDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SV_IMPLEMENT_CLASS(WatershedFilter, SvPb::WatershedFilterClassId)

WatershedFilter::WatershedFilter( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

WatershedFilter::~WatershedFilter()
{
}

void WatershedFilter::init()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVWatershedFilterObjectType;

	RegisterEmbeddedObject( &m_lvoMinVariation, SvPb::WatershedFilterMinVariationEId, IDS_OBJECTNAME_WATERSHEDFILTER_MINVARIATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_lvoControlFlag, SvPb::WatershedFilterControlFlagEId, IDS_OBJECTNAME_WATERSHEDFILTER_CONTROLFLAG, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoUseMarker, SvPb::WatershedFilterUseMarkerEId,    IDS_OBJECTNAME_WATERSHEDFILTER_USEMARKER, false, SvOi::SVResetItemNone );


	// Default Values
	m_bvoUseMarker.SetDefaultValue(BOOL(false));
	m_lvoMinVariation.SetDefaultValue( 1 );
	m_lvoControlFlag.SetDefaultValue( SVImageWSWatershed + SVImageWSMinimaFill + SVImageWSRegular + SVImage4Connected );

	// Set Attributes
	const UINT cAttributes = SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable;
	m_bvoUseMarker.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_lvoMinVariation.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_lvoControlFlag.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_MarkerImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_MarkerImageInput, SvDef::WatershedMarkerImageConnectionName, SvPb::MarkerImageInputEId);
}

bool WatershedFilter::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	if (bUseMarker)
	{
		m_MarkerImageInput.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
		m_MarkerImageInput.validateInput();
	}
	else
	{
		m_MarkerImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}

	Result = ValidateLocal(pErrorMessages) && Result;

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool WatershedFilter::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	long lMinVariation;
	long lControlFlag;
	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	m_lvoMinVariation.GetValue(lMinVariation);
	m_lvoControlFlag.GetValue(lControlFlag);

	if( m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT l_Code = E_FAIL;
		if( bUseMarker )
		{
			SvIe::SVImageClass* pInputImage = m_MarkerImageInput.getInput<SvIe::SVImageClass>(true);
			if( pInputImage )
			{
				SvOi::ITRCImagePtr pMarkerBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
				
				SVMatroxBuffer milBuffer;
				if( nullptr != pMarkerBuffer && !pMarkerBuffer->isEmpty())
				{
					milBuffer = pMarkerBuffer->getHandle()->GetBuffer();
				}

				l_Code = SVMatroxImageInterface::Watershed(rOutputImageHandle->GetBuffer(),
					First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
					milBuffer,
					lMinVariation,
					static_cast<SVImageWaterShedEnum>(lControlFlag));
			}

		}
		else
		{
			l_Code = SVMatroxImageInterface::Watershed(rOutputImageHandle->GetBuffer(),
				First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
				SVMatroxBuffer(),
				lMinVariation,
				static_cast<SVImageWaterShedEnum>(lControlFlag));
		}

		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			rRunStatus.SetInvalid();
			return false;
		}

		// Success...
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	rRunStatus.SetInvalid();
	return false;
}

bool WatershedFilter::ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const
{
	bool Result = true;
	BOOL bUseMarker;
	if ( S_OK == m_bvoUseMarker.GetValue(bUseMarker) )
	{
		if( bUseMarker )
		{
			if (nullptr == m_MarkerImageInput.getInput<SvIe::SVImageClass>())
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}	
	return Result;
}

