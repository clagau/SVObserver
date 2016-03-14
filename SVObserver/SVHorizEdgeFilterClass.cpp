//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVHorizEdgeFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:22:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVHorizEdgeFilterClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVHorizEdgeFilterClass, SVHorizEdgeFilterClassGuid )

SVHorizEdgeFilterClass::SVHorizEdgeFilterClass( SVObjectClass* POwner, int StringResourceID )
					   :SVFilterClass( POwner, StringResourceID ) 
{
	outObjectInfo.ObjectTypeInfo.SubType = SVHorzEdgeFilterObjectType;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVHorizEdgeFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Convolve( l_OutMilHandle.GetBuffer(),
			( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				SVFilterOpHorizEdge );
		if( l_Code != SVMEE_STATUS_OK )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		// Success...
		return TRUE;
	}

	// Signal that something was wrong...
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

