//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSmoothFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:03:36  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSmoothFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVSmoothFilterClass, SvPb::SmoothFilterClassId)

SVSmoothFilterClass::SVSmoothFilterClass( SVObjectClass* POwner, int StringResourceID )
					:SVFilterClass( POwner, StringResourceID ) 
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVSmoothFilterObjectType;
}

SVSmoothFilterClass::~SVSmoothFilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVSmoothFilterClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if( nullptr != m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle &&
		!(rInputImageHandle->empty() ) && !(rOutputImageHandle->empty() )	)
	{
		HRESULT l_Code;

		l_Code = SVMatroxImageInterface::Convolve(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			SVFilterOpSmooth );

		if( S_OK != l_Code )
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

