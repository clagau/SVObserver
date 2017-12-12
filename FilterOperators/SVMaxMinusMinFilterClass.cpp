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
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVMaxMinusMinFilterClass, SVMaxMinusMinFilterClassGuid)

SVMaxMinusMinFilterClass::SVMaxMinusMinFilterClass( SVObjectClass* POwner, int StringResourceID )
							 :SVFilterClass( POwner, StringResourceID ) 
{
	init();
}

void SVMaxMinusMinFilterClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVFilterObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVMaxMinusMinFilterObjectType;
}

SVMaxMinusMinFilterClass::~SVMaxMinusMinFilterClass()
{
	CloseObject();
}

bool SVMaxMinusMinFilterClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVFilterClass::CreateObject(rCreateStructure);

	if( bOk )
	{
		bOk = ResetObject();
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVMaxMinusMinFilterClass::CloseObject()
{
	m_ProcBufferHandlePtr.reset();
	
	return SVFilterClass::CloseObject();
}

bool SVMaxMinusMinFilterClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_ProcBufferHandlePtr.reset();

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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
bool SVMaxMinusMinFilterClass::onRun( bool First, SVImageBufferHandlePtr RInputImageHandle, SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if( m_pCurrentUIOPL && nullptr != m_ProcBufferHandlePtr && nullptr != RInputImageHandle && nullptr != ROutputImageHandle )
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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

