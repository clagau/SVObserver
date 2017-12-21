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
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVWaterShedFilter.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVObjectLibrary/SVClsIds.h"

#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SV_IMPLEMENT_CLASS(SVWatershedFilterClass, SVWatershedFilterClassGuid)

SVWatershedFilterClass::SVWatershedFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVWatershedFilterClass::~SVWatershedFilterClass()
{
}

void SVWatershedFilterClass::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVWatershedFilterObjectType;

	RegisterEmbeddedObject( &m_lvoMinVariation,		SVWatershedFilterMinVariationGuid, IDS_OBJECTNAME_WATERSHEDFILTER_MINVARIATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_lvoControlFlag,		SVWatershedFilterControlFlagGuid, IDS_OBJECTNAME_WATERSHEDFILTER_CONTROLFLAG, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoUseMarker,        SVWatershedFilterUseMarkerGuid,    IDS_OBJECTNAME_WATERSHEDFILTER_USEMARKER, false, SvOi::SVResetItemNone );


	// Default Values
	m_bvoUseMarker.SetDefaultValue(BOOL(false));
	m_lvoMinVariation.SetDefaultValue( 1 );
	m_lvoControlFlag.SetDefaultValue( SVImageWSWatershed + SVImageWSMinimaFill + SVImageWSRegular + SVImage4Connected );

	// Set Attributes
	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE;
	m_bvoUseMarker.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lvoMinVariation.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_lvoControlFlag.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_MarkerImageInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_MarkerImageInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_MarkerImageInfo, SvDef::WatershedMarkerImageConnectionName );

	
	// Set default inputs and outputs
	addDefaultInputObjects();

}

bool SVWatershedFilterClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	Result = ValidateLocal(pErrorMessages) && Result;

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVWatershedFilterClass::onRun( bool First, SVImageBufferHandlePtr rInputImageHandle, SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	long lMinVariation;
	long lControlFlag;
	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	m_lvoMinVariation.GetValue(lMinVariation);
	m_lvoControlFlag.GetValue(lControlFlag);

	SVImageBufferHandlePtr ImageHandle;

	if( m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		rInputImageHandle->GetData( l_InMilHandle );
		rOutputImageHandle->GetData( l_OutMilHandle );
		
		HRESULT l_Code;
		if( bUseMarker && m_MarkerImageInfo.IsConnected() )
		{
			SVImageClass* pInputImage = getInputImage();
			if( pInputImage )
			{
				pInputImage->GetImageHandle( ImageHandle );
				
				SVImageBufferHandleImage l_MilHandle;
				if( nullptr != ImageHandle )
				{
					ImageHandle->GetData( l_MilHandle );
				}

				l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
					First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
					l_MilHandle.GetBuffer(),
					lMinVariation,
					static_cast<SVImageWaterShedEnum>(lControlFlag));
			}

		}
		else
		{
			SVImageBufferHandleImage l_MilHandle;
			if (nullptr != ImageHandle)
			{
				ImageHandle->GetData( l_MilHandle );
			}

			l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
				First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				l_MilHandle.GetBuffer(),
				lMinVariation,
				static_cast<SVImageWaterShedEnum>(lControlFlag));
		}

		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			SetInvalid();
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	SetInvalid();
	rRunStatus.SetInvalid();
	return false;
}

SVImageClass* SVWatershedFilterClass::getInputImage() const
{
	if (m_MarkerImageInfo.IsConnected() && nullptr != m_MarkerImageInfo.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVImageClass*> (m_MarkerImageInfo.GetInputObjectInfo().getObject());
	}

	return nullptr;
}


bool SVWatershedFilterClass::ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const
{
	bool Result = true;
	BOOL bUseMarker;
	if ( S_OK == m_bvoUseMarker.GetValue(bUseMarker) )
	{
		if( bUseMarker )
		{
			if (nullptr == getInputImage())
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}	
	return Result;
}

