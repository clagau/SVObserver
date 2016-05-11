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
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVSmoothFilterClass, SVSmoothFilterClassGuid )

SVSmoothFilterClass::SVSmoothFilterClass( SVObjectClass* POwner, int StringResourceID )
					:SVFilterClass( POwner, StringResourceID ) 
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVSmoothFilterObjectType;
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
BOOL SVSmoothFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	if( nullptr != m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		if( S_OK == RInputImageHandle->GetData( l_InMilHandle ) && !( l_InMilHandle.empty() ) &&
			S_OK == ROutputImageHandle->GetData( l_OutMilHandle ) && !( l_OutMilHandle.empty() ) )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;

			l_Code = SVMatroxImageInterface::Convolve( l_OutMilHandle.GetBuffer(),
				( TRUE == First ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
					SVFilterOpSmooth );

			if( SVMEE_STATUS_OK != l_Code )
			{
				// Signal that something was wrong...
				SetInvalid();
				RRunStatus.SetInvalid();
				return FALSE;
			}
		}
		else
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		return TRUE;
	}

	// Signal that something was wrong...
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

