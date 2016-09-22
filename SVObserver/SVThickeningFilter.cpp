// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilter
// * .File Name       : $Workfile:   SVThickeningFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:36:40  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVThickeningFilter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageProcessingClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVThickeningFilterClass, SVThickeningFilterClassGuid )

SVThickeningFilterClass::SVThickeningFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVThickeningFilterClass::~SVThickeningFilterClass()
{
}

void SVThickeningFilterClass::init()
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVThickeningFilterObjectType;

	RegisterEmbeddedObject( &m_lvoItterations, SVThickeningFilterItterationsGuid, IDS_OBJECTNAME_THICKENINGFILTER_ITTERATIONS, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoGrayOn, SVThickeningFilterGrayOnGuid, IDS_OBJECTNAME_THICKENINGFILTER_GRAYON, false, SVResetItemNone );

	m_lvoItterations.SetDefaultValue( 1L, true );
	m_bvoGrayOn.SetDefaultValue( false, true );


	m_lvoItterations.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_bvoGrayOn.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	// Set default inputs and outputs
	addDefaultInputObjects();

}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVThickeningFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVThickeningFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 
	// Force a copy forward to keep the display correct
	m_lvoItterations.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_bvoGrayOn.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	long lItterations;
	long lMode;
	BOOL bGrayOn;

	m_bvoGrayOn.GetValue(bGrayOn);

	m_lvoItterations.GetValue(lItterations);

	lMode = bGrayOn ? SVValueGrayscale : SVValueBinary;

	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Thick( l_OutMilHandle.GetBuffer(), 
			( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(), 
						lItterations,
						static_cast<SVImageOperationTypeEnum>(lMode));	

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

