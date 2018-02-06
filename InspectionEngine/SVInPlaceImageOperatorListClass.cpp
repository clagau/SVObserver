//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the InplaceImageOperator List
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInplaceImageOperatorListClass.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVInPlaceImageOperatorListClass, SVInPlaceImageOperatorListClassGuid )

SVInPlaceImageOperatorListClass::SVInPlaceImageOperatorListClass( SVObjectClass* POwner, int StringResourceID )
							    :SVUnaryImageOperatorListClass( POwner, StringResourceID ) 
{
	init();
}

void SVInPlaceImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SvDef::SVInPlaceImageOperatorListObjectType;

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVInPlaceImageOperatorListClass::~SVInPlaceImageOperatorListClass() 
{ 
	CloseObject();
}

bool SVInPlaceImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);

	if( nullptr == getInputImage() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}
	else if( getInputImage()->GetImageType() != SvDef::SVImageTypeEnum::SVImageTypePhysical )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotImageTypePhysical, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	return Valid;
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

	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = rRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

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
		SvOi::SVImageBufferHandlePtr input;
		SvOi::SVImageBufferHandlePtr output;
		// Use input image for in- and output.
		// Image must be a Physical type!!! ( Is already checked in ResetObject() )
		getInputImage(true)->GetImageHandle( input );
		getInputImage(true)->GetImageHandle( output );
		
		bool bFirstFlag = true;

		// Run children...
		for( int i = 0; i < GetSize(); i++ )
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVObjectClass* pObject = GetAt( i );
			
			// Do safe type casting...
			SVUnaryImageOperatorClass* pOperator = dynamic_cast< SVUnaryImageOperatorClass* >( pObject );

			if( pOperator )
			{
				if( pOperator->Run( bFirstFlag, input, output, ChildRunStatus, &m_RunErrorMessages ) )
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
	}

	if (bRetVal && !m_RunErrorMessages.empty())
	{
		bRetVal = false;
	}
	
	if( ! bRetVal )
	{
		// Something was wrong...
		SetInvalid();
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
