//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes


SV_IMPLEMENT_CLASS(SVMaxMinusMinFilterClass, SvPb::MaxMinusMinFilterClassId)

SVMaxMinusMinFilterClass::SVMaxMinusMinFilterClass( SVObjectClass* POwner, int StringResourceID )
							 :SVFilterClass( POwner, StringResourceID ) 
{
	init();
}

void SVMaxMinusMinFilterClass::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVFilterObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVMaxMinusMinFilterObjectType;
}

SVMaxMinusMinFilterClass::~SVMaxMinusMinFilterClass()
{
	SVMaxMinusMinFilterClass::CloseObject();
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

	if( Result && nullptr != getOutputImage() )
	{
		mProcBufferInfo = getOutputImage()->GetImageInfo();

		mProcBufferInfo.SetOwnerImage(SvDef::InvalidObjectId);
		mProcBufferInfo.SetOwner(SvDef::InvalidObjectId);

		if ( S_OK != SvIe::SVImageProcessingClass::CreateImageBuffer( mProcBufferInfo, m_ProcBufferHandlePtr ) )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
bool SVMaxMinusMinFilterClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	if( m_pCurrentUIOPL && nullptr != m_ProcBufferHandlePtr && nullptr != rInputImageHandle && nullptr != rOutputImageHandle)
	{
		HRESULT l_Code;
		// Do Min...
		l_Code = SVMatroxImageInterface::Erode(m_ProcBufferHandlePtr->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			1,
			SVImageGrayScale);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}

			// Signal that something was wrong...
			rRunStatus.SetInvalid();
			return false;
		}

		// Do Max...
		l_Code = SVMatroxImageInterface::Dilate(rOutputImageHandle->GetBuffer(),
			First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
			1,
			SVImageGrayScale);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
				pErrorMessages->push_back(Msg);
			}

			// Signal that something was wrong...
			rRunStatus.SetInvalid();
			return false;
		}

		// Do MaxMinusMin...
		l_Code = SVMatroxImageInterface::Arithmetic(rOutputImageHandle->GetBuffer(),
			rOutputImageHandle->GetBuffer(),
			m_ProcBufferHandlePtr->GetBuffer(),
			SVImageSubAbs);


		if( l_Code != S_OK )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunFilterFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	// Signal that something was wrong...
	rRunStatus.SetInvalid();
	return false;
}

