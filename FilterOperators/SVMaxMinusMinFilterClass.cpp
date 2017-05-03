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

#pragma region Includes
#include "stdafx.h"
#include "SVMaxMinusMinFilterClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOCore/SVImageClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes


SVMaxMinusMinFilterClass::SVMaxMinusMinFilterClass( SVObjectClass* POwner, int StringResourceID )
							 :SVFilterClass( POwner, StringResourceID ) 
{
	init();
}

void SVMaxMinusMinFilterClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVFilterObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVMaxMinusMinFilterObjectType;
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
		bOk = ResetObject();
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVMaxMinusMinFilterClass::CloseObject()
{
	m_ProcBufferHandlePtr.clear();
	
	return SVFilterClass::CloseObject();
}

bool SVMaxMinusMinFilterClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_ProcBufferHandlePtr.clear();

	if( Result && nullptr != getReferenceImage() )
	{
		mProcBufferInfo = getReferenceImage()->GetImageInfo();

		mProcBufferInfo.SetOwnerImage( SV_GUID_NULL );
		mProcBufferInfo.SetOwner( SV_GUID_NULL );

		if ( S_OK != SVImageProcessingClass::CreateImageBuffer( mProcBufferInfo, m_ProcBufferHandlePtr ) )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	m_isCreated = Result;

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVMaxMinusMinFilterClass::onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if( m_pCurrentUIOPL && !m_ProcBufferHandlePtr.empty() && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_ProcMilHandle;
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		m_ProcBufferHandlePtr->GetData( l_ProcMilHandle );
		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		HRESULT l_Code;
		// Do Min...
		l_Code = SVMatroxImageInterface::Erode( l_ProcMilHandle.GetBuffer(),
			First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
			1,
			SVImageGrayScale);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}

			// Signal that something was wrong...
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		// Do Max...
		l_Code = SVMatroxImageInterface::Dilate( l_OutMilHandle.GetBuffer(),
			First ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
			1,
			SVImageGrayScale);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}

			// Signal that something was wrong...
			SetInvalid();
			rRunStatus.SetInvalid();
			return false;
		}

		// Do MaxMinusMin...
		l_Code = SVMatroxImageInterface::Arithmetic( l_OutMilHandle.GetBuffer(),
			l_OutMilHandle.GetBuffer(), 
			l_ProcMilHandle.GetBuffer(),
			SVImageSubAbs);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	SetInvalid();
	rRunStatus.SetInvalid();
	return false;
}

