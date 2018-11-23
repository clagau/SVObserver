//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
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
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVSmoothFilterClass, SVSmoothFilterClassGuid)

SVSmoothFilterClass::SVSmoothFilterClass( SVObjectClass* POwner, int StringResourceID )
					:SVFilterClass( POwner, StringResourceID ) 
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVSmoothFilterObjectType;
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
bool SVSmoothFilterClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

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

