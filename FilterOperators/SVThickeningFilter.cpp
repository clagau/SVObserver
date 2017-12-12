// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilter
// * .File Name       : $Workfile:   SVThickeningFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:36:40  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVThickeningFilter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SV_IMPLEMENT_CLASS(SVThickeningFilterClass, SVThickeningFilterClassGuid)

SVThickeningFilterClass::SVThickeningFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVThickeningFilterClass::~SVThickeningFilterClass()
{
}

void SVThickeningFilterClass::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVThickeningFilterObjectType;

	RegisterEmbeddedObject( &m_lvoItterations, SVThickeningFilterItterationsGuid, IDS_OBJECTNAME_THICKENINGFILTER_ITTERATIONS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoGrayOn, SVThickeningFilterGrayOnGuid, IDS_OBJECTNAME_THICKENINGFILTER_GRAYON, false, SvOi::SVResetItemNone );

	m_lvoItterations.SetDefaultValue( 1L );
	m_bvoGrayOn.SetDefaultValue(BOOL(false));


	const UINT cAttributes = SvDef::SV_PRINTABLE | SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE;
	m_lvoItterations.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_bvoGrayOn.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	// Set default inputs and outputs
	addDefaultInputObjects();

}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVThickeningFilterClass::onRun( bool First, SVImageBufferHandlePtr rInputImageHandle, SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	long lItterations;
	long lMode;
	BOOL bGrayOn;

	m_bvoGrayOn.GetValue(bGrayOn);

	m_lvoItterations.GetValue(lItterations);

	lMode = bGrayOn ? SVValueGrayscale : SVValueBinary;

	if( m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		rInputImageHandle->GetData( l_InMilHandle );
		rOutputImageHandle->GetData( l_OutMilHandle );
		
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Thick( l_OutMilHandle.GetBuffer(), 
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

