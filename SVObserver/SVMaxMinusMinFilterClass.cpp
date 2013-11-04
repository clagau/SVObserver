//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVMaxMinusMinFilterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:00:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVMaxMinusMinFilterClass.h"
#include "SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"

SV_IMPLEMENT_CLASS( SVMaxMinusMinFilterClass, SVMaxMinusMinFilterClassGuid )

SVMaxMinusMinFilterClass::SVMaxMinusMinFilterClass( SVObjectClass* POwner, int StringResourceID )
							 :SVFilterClass( POwner, StringResourceID ) 
{
	init();
}

void SVMaxMinusMinFilterClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVFilterObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVMaxMinusMinFilterObjectType;
}

SVMaxMinusMinFilterClass::~SVMaxMinusMinFilterClass()
{
	CloseObject();
}

BOOL SVMaxMinusMinFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVFilterClass::CreateObject( PCreateStructure );

	if( bOk )
	{
		bOk = ResetObject() == S_OK;
	}

	isCreated = bOk;

	return bOk;
}

BOOL SVMaxMinusMinFilterClass::CloseObject()
{
	m_ProcBufferHandlePtr.clear();
	
	return SVFilterClass::CloseObject();
}

HRESULT SVMaxMinusMinFilterClass::ResetObject()
{
	HRESULT l_hrOk = SVFilterClass::ResetObject();

	m_ProcBufferHandlePtr.clear();

	if( l_hrOk == S_OK && getReferenceImage() != NULL )
	{
		mProcBufferInfo = getReferenceImage()->GetImageInfo();

		mProcBufferInfo.SetOwnerImage( SV_GUID_NULL );
		mProcBufferInfo.SetOwner( SV_GUID_NULL );

		l_hrOk = SVImageProcessingClass::Instance().CreateImageBuffer( mProcBufferInfo, m_ProcBufferHandlePtr );
	}

	isCreated = l_hrOk == S_OK;

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVMaxMinusMinFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	if( m_pCurrentUIOPL && !m_ProcBufferHandlePtr.empty() && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_ProcMilHandle;
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		m_ProcBufferHandlePtr->GetData( l_ProcMilHandle );
		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		SVMatroxImageInterface::SVStatusCode l_Code;
		// Do Min...
		l_Code = SVMatroxImageInterface::Erode( l_ProcMilHandle.GetBuffer(),
			( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
			1,
			SVImageGrayScale);


		if( l_Code != SVMEE_STATUS_OK )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		// Do Max...
		l_Code = SVMatroxImageInterface::Dilate( l_OutMilHandle.GetBuffer(),
			( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
			1,
			SVImageGrayScale);


		if( l_Code != SVMEE_STATUS_OK )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		// Do MaxMinusMin...
		l_Code = SVMatroxImageInterface::Arithmetic( l_OutMilHandle.GetBuffer(),
			l_OutMilHandle.GetBuffer(), 
			l_ProcMilHandle.GetBuffer(),
			SVImageSubAbs);


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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMaxMinusMinFilterClass.cpp_v  $
 * 
 *    Rev 1.0   09 Aug 2013 12:00:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/