//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVEdgeDetectFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:57:48  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVEdgeDetectFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVEdgeDetectFilterClass, SvPb::EdgeDetectFilterClassId)

SVEdgeDetectFilterClass::SVEdgeDetectFilterClass( SVObjectClass* POwner, int StringResourceID )
						:SVFilterClass( POwner, StringResourceID ) 
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVEdgeDetectFilterObjectType;
}

SVEdgeDetectFilterClass::~SVEdgeDetectFilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVEdgeDetectFilterClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if(m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Convolve(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
				SVFilterOpEdge );
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

