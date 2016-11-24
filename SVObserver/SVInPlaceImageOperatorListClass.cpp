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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVInPlaceImageOperatorListClass, SVInPlaceImageOperatorListClassGuid )

SVInPlaceImageOperatorListClass::SVInPlaceImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							    :SVUnaryImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVInPlaceImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType	= SVInPlaceImageOperatorListObjectType;

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

HRESULT SVInPlaceImageOperatorListClass::IsInputImage( SVImageClass *p_psvImage )
{
	return S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : Validates base class and checks, 
//				: if input image is a physical type.
////////////////////////////////////////////////////////////////////////////////
BOOL SVInPlaceImageOperatorListClass::OnValidate()
{
	if( SVUnaryImageOperatorListClass::OnValidate() )
	{
		if( nullptr != getInputImage() && getInputImage()->GetImageType() == SVImageTypePhysical )
		{
			return true;
		}
	}

	SetInvalid();
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Is doing in place processing on input image!!!
//				:
//				: Special routing for image operators here.
BOOL SVInPlaceImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	SVSmartHandlePointer input;
	SVSmartHandlePointer output;

	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = RRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	// Run yourself...
	BOOL bRetVal = onRun( RRunStatus );

	if( ! getInputImage() )
	{
		// Signal something is wrong...
		bRetVal = false;
	}

	if( bRetVal )
	{
		// Use input image for in- and output.
		// Image must be a Physical type!!! ( Is already checked in OnValidate() )
		getInputImage()->GetImageHandle( input );
		getInputImage()->GetImageHandle( output );
		
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
				if( pOperator->Run( bFirstFlag, input, output, ChildRunStatus ) )
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
				bRetVal = FALSE;

			// Update our Run Status
			if( ChildRunStatus.IsDisabled() )
				RRunStatus.SetDisabled();

			if( ChildRunStatus.IsDisabledByCondition() )
				RRunStatus.SetDisabledByCondition();
			
			if( ChildRunStatus.IsWarned() )
				RRunStatus.SetWarned();

			if( ChildRunStatus.IsFailed() )
				RRunStatus.SetFailed();

			if( ChildRunStatus.IsPassed() )
				RRunStatus.SetPassed();

			if( ChildRunStatus.IsCriticalFailure() )
				RRunStatus.SetCriticalFailure();
		}
  }
	
	if( ! bRetVal )
	{
		// Something was wrong...

		SetInvalid();
		RRunStatus.SetInvalid();
	}

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return bRetVal;
}
