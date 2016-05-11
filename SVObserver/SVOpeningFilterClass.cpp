//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVOpeningFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:00:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVOpeningFilterClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVOpeningFilterClass, SVOpeningFilterClassGuid )

SVOpeningFilterClass::SVOpeningFilterClass( SVObjectClass* POwner, int StringResourceID )
					 :SVFilterClass( POwner, StringResourceID ) 
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVOpeningFilterObjectType;
}

SVOpeningFilterClass::~SVOpeningFilterClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVOpeningFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );

		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Open( l_OutMilHandle.GetBuffer(),
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				1,
				SVImageGrayScale );
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

