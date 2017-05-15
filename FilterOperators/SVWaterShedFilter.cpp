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
#include "SVOCore/SVImageProcessingClass.h"
#include "SVOCore/SVTaskObjectList.h"
#include "SVOCore/SVImageClass.h"
#include "SVWaterShedFilter.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "SVObjectLibrary/SVClsIds.h"

#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVWatershedFilterObjectType;

	RegisterEmbeddedObject( &m_lvoMinVariation,		SVWatershedFilterMinVariationGuid, IDS_OBJECTNAME_WATERSHEDFILTER_MINVARIATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_lvoControlFlag,		SVWatershedFilterControlFlagGuid, IDS_OBJECTNAME_WATERSHEDFILTER_CONTROLFLAG, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoUseMarker,        SVWatershedFilterUseMarkerGuid,    IDS_OBJECTNAME_WATERSHEDFILTER_USEMARKER, false, SvOi::SVResetItemNone );


	// Default Values
	m_bvoUseMarker.SetDefaultValue( FALSE, TRUE);
	m_lvoMinVariation.SetDefaultValue( 1, TRUE );
	m_lvoControlFlag.SetDefaultValue( SVImageWSWatershed + SVImageWSMinimaFill + SVImageWSRegular + SVImage4Connected, TRUE );

	// Set Attributes
	const UINT cAttributes = SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE;
	m_bvoUseMarker.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lvoMinVariation.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_lvoControlFlag.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_MarkerImageInfo.SetInputObjectType( SVImageObjectType );
	m_MarkerImageInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_MarkerImageInfo, SvOi::WatershedMarkerImageConnectionName );

	
	// Set default inputs and outputs
	addDefaultInputObjects();

}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVWatershedFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	return bOk;
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
bool SVWatershedFilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// Force a copy forward to keep the display correct
	m_lvoMinVariation.CopyLastSetValue( rRunStatus.m_lResultDataIndex );
	m_lvoControlFlag.CopyLastSetValue( rRunStatus.m_lResultDataIndex );

	long lMinVariation;
	long lControlFlag;
	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	m_lvoMinVariation.GetValue(lMinVariation);
	m_lvoControlFlag.GetValue(lControlFlag);

	SVSmartHandlePointer ImageHandle;

	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		HRESULT l_Code;
		if( bUseMarker && m_MarkerImageInfo.IsConnected() )
		{
			SVImageClass* pInputImage = ( SVImageClass* )m_MarkerImageInfo.GetInputObjectInfo().m_pObject;
			if( pInputImage )
			{
				pInputImage->GetImageHandle( ImageHandle );
				
				SVImageBufferHandleImage l_MilHandle;
				if( !( ImageHandle.empty() ) )
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
			if( !( ImageHandle.empty() ) )
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

bool SVWatershedFilterClass::ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const
{
	bool Result = true;
	BOOL bUseMarker;
	if ( S_OK == m_bvoUseMarker.GetValue(bUseMarker) )
	{
		if( bUseMarker )
		{
			if (!m_MarkerImageInfo.IsConnected() || nullptr == m_MarkerImageInfo.GetInputObjectInfo().m_pObject)
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

