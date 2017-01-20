// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 16:30:20  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVOCore/SVTaskObjectList.h"
#include "SVOCore/SVImageClass.h"
#include "SVWaterShedFilter.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "SVObjectLibrary/SVClsIds.h"

#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVWatershedFilterClass::SVWatershedFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVWatershedFilterClass::~SVWatershedFilterClass()
{
}

void SVWatershedFilterClass::init()
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVWatershedFilterObjectType;

	RegisterEmbeddedObject( &m_lvoMinVariation,		SVWatershedFilterMinVariationGuid, IDS_OBJECTNAME_WATERSHEDFILTER_MINVARIATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_lvoControlFlag,		SVWatershedFilterControlFlagGuid, IDS_OBJECTNAME_WATERSHEDFILTER_CONTROLFLAG, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoUseMarker,        SVWatershedFilterUseMarkerGuid,    IDS_OBJECTNAME_WATERSHEDFILTER_USEMARKER, false, SVResetItemNone );


	// Default Values
	m_bvoUseMarker.SetDefaultValue( FALSE, TRUE);
	m_lvoMinVariation.SetDefaultValue( 1, TRUE );
	m_lvoControlFlag.SetDefaultValue( SVImageWSWatershed + SVImageWSMinimaFill + SVImageWSRegular + SVImage4Connected, TRUE );

	// Set Attributes
	m_bvoUseMarker.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lvoMinVariation.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoControlFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	m_MarkerImageInfo.SetInputObjectType( SVImageObjectType );
	m_MarkerImageInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_MarkerImageInfo, SvOi::WatershedMarkerImageConnectionName );

	
	// Set default inputs and outputs
	addDefaultInputObjects();

}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
BOOL SVWatershedFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	return bOk;
}

BOOL SVWatershedFilterClass::OnValidate()
{
	BOOL bUseMarker;

	BOOL bRetVal = SVFilterClass::OnValidate();

	bRetVal = bRetVal && ( S_OK == m_bvoUseMarker.GetValue(bUseMarker) );

	if( bRetVal && bUseMarker )
	{
		bRetVal = bRetVal && m_MarkerImageInfo.IsConnected();
		bRetVal = bRetVal && nullptr != m_MarkerImageInfo.GetInputObjectInfo().PObject;
	}

	return bRetVal;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVWatershedFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 

	// Force a copy forward to keep the display correct
	m_lvoMinVariation.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoControlFlag.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	long lMinVariation;
	long lControlFlag;
	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	m_lvoMinVariation.GetValue(lMinVariation);
	m_lvoControlFlag.GetValue(lControlFlag);

	SVSmartHandlePointer ImageHandle;

	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		SVMatroxImageInterface::SVStatusCode l_Code;
		if( bUseMarker && m_MarkerImageInfo.IsConnected() )
		{
			SVImageClass* pInputImage = ( SVImageClass* )m_MarkerImageInfo.GetInputObjectInfo().PObject;
			if( pInputImage )
			{
				pInputImage->GetImageHandle( ImageHandle );
				
				SVImageBufferHandleImage l_MilHandle;
				if( !( ImageHandle.empty() ) )
				{
					ImageHandle->GetData( l_MilHandle );
				}

				l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
					( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
					l_MilHandle.GetBuffer(),
					lMinVariation,
					static_cast<SVImageWaterShedEnum>(lControlFlag));
			}

		}
		else
		{
			SVImageBufferHandleImage l_MilHandle;
			if( !( ImageHandle.empty() ) )
			{
				ImageHandle->GetData( l_MilHandle );
			}

			l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				l_MilHandle.GetBuffer(),
				lMinVariation,
				static_cast<SVImageWaterShedEnum>(lControlFlag));
		}

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

