//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdge2FilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:59:34  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVLaplacianEdge2FilterClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes


SVLaplacianEdge2FilterClass::SVLaplacianEdge2FilterClass( SVObjectClass* POwner, int StringResourceID )
						    :SVFilterClass( POwner, StringResourceID ) 
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLaplacianEdge2FilterObjectType;
}

SVLaplacianEdge2FilterClass::~SVLaplacianEdge2FilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVLaplacianEdge2FilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Convolve( l_OutMilHandle.GetBuffer(),
				First  ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				SVFilterOpLaplacianEdge2 );
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

