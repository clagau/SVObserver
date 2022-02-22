//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVErosionFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:21:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVErosionFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS(SVErosionFilterClass, SvPb::ErosionFilterClassId)

SVErosionFilterClass::SVErosionFilterClass( SVObjectClass* POwner, int StringResourceID )
					 :SVFilterClass( POwner, StringResourceID ) 
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVErosionFilterObjectType;
}

SVErosionFilterClass::~SVErosionFilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVErosionFilterClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if (m_pCurrentUIOPL && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Erode(rOutputImageHandle->GetBuffer(),
				First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
				1,
				SVImageGrayScale );
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

