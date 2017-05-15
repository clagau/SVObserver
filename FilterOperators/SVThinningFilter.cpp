// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThinningFilter
// * .File Name       : $Workfile:   SVThinningFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:37:36  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVThinningFilter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVThinningFilterClass::SVThinningFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVThinningFilterClass::~SVThinningFilterClass()
{
}

void SVThinningFilterClass::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVThinningFilterObjectType;

	RegisterEmbeddedObject( &m_lvoItterations, SVThinningFilterItterationsGuid, IDS_OBJECTNAME_THINNINGFILTER_ITTERATIONS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoGrayOn, SVThinningFilterGrayOnGuid, IDS_OBJECTNAME_THINNINGFILTER_GRAYON, false, SvOi::SVResetItemNone );

	m_lvoItterations.SetDefaultValue( 1L, true );
	m_bvoGrayOn.SetDefaultValue( false, true );


	const UINT cAttributes = SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE;
	m_lvoItterations.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_bvoGrayOn.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	// Set default inputs and outputs
	addDefaultInputObjects();

}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVThinningFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVThinningFilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	// Force a copy forward to keep the display correct
	m_lvoItterations.CopyLastSetValue( rRunStatus.m_lResultDataIndex );
	m_bvoGrayOn.CopyLastSetValue( rRunStatus.m_lResultDataIndex );

	long lItterations;
	long lMode;
	BOOL bGrayOn;

	m_bvoGrayOn.GetValue(bGrayOn);
	m_lvoItterations.GetValue(lItterations);
	
	lMode = bGrayOn ? SVValueGrayscale : SVValueBinary;
	
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Thin( l_OutMilHandle.GetBuffer(), 
			First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
			lItterations,
			static_cast<SVImageOperationTypeEnum>(lMode));	
		
		
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

