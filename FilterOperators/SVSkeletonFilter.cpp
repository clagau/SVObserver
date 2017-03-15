// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSkeletonFilter
// * .File Name       : $Workfile:   SVSkeletonFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 15:07:18  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSkeletonFilter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVSkeletonFilterClass::SVSkeletonFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVSkeletonFilterObjectType;
}

SVSkeletonFilterClass::~SVSkeletonFilterClass()
{
}

bool SVSkeletonFilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Thin( l_OutMilHandle.GetBuffer(),
			First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
			SVValueImageToSkeleton,
			SVImageBinary);

		if( l_Code != SVMEE_STATUS_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return false;
		}

		// Success...
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	SetInvalid();
	RRunStatus.SetInvalid();
	return false;
}

