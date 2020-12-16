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
#include "ThinningFilter.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SV_IMPLEMENT_CLASS(ThinningFilter, SvPb::ThinningFilterClassId)

ThinningFilter::ThinningFilter( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

ThinningFilter::~ThinningFilter()
{
}

void ThinningFilter::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVThinningFilterObjectType;

	RegisterEmbeddedObject( &m_lvoItterations, SvPb::ThinningFilterItterationsEId, IDS_OBJECTNAME_THINNINGFILTER_ITTERATIONS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoGrayOn, SvPb::GrayOnEId, IDS_OBJECTNAME_GRAYON, false, SvOi::SVResetItemNone );

	m_lvoItterations.SetDefaultValue( 1L );
	m_bvoGrayOn.SetDefaultValue(BOOL(false));


	const UINT cAttributes = SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable;
	m_lvoItterations.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_bvoGrayOn.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool ThinningFilter::onRun(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{ 
	long lItterations;
	long lMode;
	BOOL bGrayOn;

	m_bvoGrayOn.GetValue(bGrayOn);
	m_lvoItterations.GetValue(lItterations);
	
	lMode = bGrayOn ? SVValueGrayscale : SVValueBinary;
	
	if( m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Thin(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			lItterations,
			static_cast<SVImageOperationTypeEnum>(lMode));	
		
		
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

