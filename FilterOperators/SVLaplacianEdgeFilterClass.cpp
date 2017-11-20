//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdgeFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:59:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVLaplacianEdgeFilterClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVLaplacianEdgeFilterClass, SVLaplacianEdgeFilterClassGuid)

SVLaplacianEdgeFilterClass::SVLaplacianEdgeFilterClass( SVObjectClass* POwner, int StringResourceID )
						   :SVFilterClass( POwner, StringResourceID ) 
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVLaplacianEdgeFilterObjectType;
}

SVLaplacianEdgeFilterClass::~SVLaplacianEdgeFilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVLaplacianEdgeFilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Convolve( l_OutMilHandle.GetBuffer(),
				First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				SVFilterOpLaplacianEdge );
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

