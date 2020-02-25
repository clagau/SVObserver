//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the InplaceImageOperator List
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInPlaceImageOperatorListClass.h"

#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUnaryImageOperatorClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVInPlaceImageOperatorListClass, SvPb::InPlaceImageOperatorListClassId)

SVInPlaceImageOperatorListClass::SVInPlaceImageOperatorListClass( SVObjectClass* POwner, int StringResourceID )
							    :SVUnaryImageOperatorListClass( POwner, StringResourceID ) 
{
	init();
}

void SVInPlaceImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVInPlaceImageOperatorListObjectType;

	// Register Embedded Objects

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVInPlaceImageOperatorListClass::~SVInPlaceImageOperatorListClass() 
{ 
	SVInPlaceImageOperatorListClass::CloseObject();
}

bool SVInPlaceImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if( nullptr == getInputImage() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}
	else if( getInputImage()->GetImageType() != SvDef::SVImageTypeEnum::SVImageTypePhysical )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotImageTypePhysical, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Is doing in place processing on input image!!!
//				:
//				: Special routing for image operators here.
bool SVInPlaceImageOperatorListClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	clearRunErrorMessages();

	bool childUpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun( rRunStatus, &m_RunErrorMessages );

	if( nullptr == getInputImage(true) )
	{
		// Signal something is wrong...
		bRetVal = false;
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		m_RunErrorMessages.push_back(Msg);
	}

	if( bRetVal )
	{
		//The input image will be needed but will be overridden, because in SVImageArithmeticClass it was already called the write-method and this image will be used for both task as output.
		SvTrc::IImagePtr pImageBuffer = getInputImage(true)->getImageReadOnly(rRunStatus.m_triggerRecord.get());
		SvOi::SVImageBufferHandlePtr imageHandle;
		if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
		{
			imageHandle = pImageBuffer->getHandle();
		}
	
		bool bFirstFlag = true;
		SVRunStatusClass ChildRunStatus;
		ChildRunStatus.m_UpdateCounters = childUpdateCounters;
		ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
		// Run children...
		for( int i = 0; i < GetSize(); i++ )
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVObjectClass* pObject = GetAt( i );
			
			// Do safe type casting...
			SVUnaryImageOperatorClass* pOperator = dynamic_cast< SVUnaryImageOperatorClass* >( pObject );

			if( pOperator )
			{
				if( pOperator->Run( bFirstFlag, imageHandle, imageHandle, ChildRunStatus, &m_RunErrorMessages ) )
				{
					// Switch first flag off ( FALSE )...
					bFirstFlag = false;

					// NOTE:
					// If operator returns FALSE, he was not running ( may be deactivated )
					// So, don't switch first flag off, so that a following operator knows
					// he is the first one or the 'no operator was running' check can do his job!!!
					// RO_22Mar2000

					// WARNING:
					// Do not set bRetVal automatically to FALSE, if operator was not running !!!
					// ChildRunStatus keeps information about, if an error occured while running !!!

					// NOTE: 
					// Since we do here an in place process, that means input and output image is
					// the same, bFirstFlag is not really necessary, but supported for sanity.
					// But if no operator exists, we don't have to copy input to output anymore!
				}
			}
			else
			{
				bRetVal = false;
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				m_RunErrorMessages.push_back(Msg);
			}

			// Update our Run Status
			if( ChildRunStatus.IsDisabled() )
				rRunStatus.SetDisabled();

			if( ChildRunStatus.IsDisabledByCondition() )
				rRunStatus.SetDisabledByCondition();
			
			if( ChildRunStatus.IsWarned() )
				rRunStatus.SetWarned();

			if( ChildRunStatus.IsFailed() )
				rRunStatus.SetFailed();

			if( ChildRunStatus.IsPassed() )
				rRunStatus.SetPassed();

			if( ChildRunStatus.IsCriticalFailure() )
				rRunStatus.SetCriticalFailure();
		}
		rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);
	}

	if (bRetVal && !m_RunErrorMessages.empty())
	{
		bRetVal = false;
	}
	
	if( ! bRetVal )
	{
		// Something was wrong...
		rRunStatus.SetInvalid();
	}

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

} //namespace SvOp
