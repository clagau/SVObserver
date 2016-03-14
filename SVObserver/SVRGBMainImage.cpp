//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRGBMainImage
//* .File Name       : $Workfile:   SVRGBMainImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:43:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVRGBMainImage.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVImageObjectClass.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"

SVRGBMainImageClass::SVRGBMainImageClass( LPCSTR ObjectName )
: SVCameraImageTemplate( ObjectName )
{
	outObjectInfo.ObjectTypeInfo.SubType = SVRGBMainImageObjectType;

	m_ImageType = SVImageTypeRGBMain;

	// Set up default for Band Number...
	m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );	// RGB Image
}

SVRGBMainImageClass::SVRGBMainImageClass( SVObjectClass* POwner, int StringResourceID )
: SVCameraImageTemplate( POwner, StringResourceID )
{
	outObjectInfo.ObjectTypeInfo.SubType = SVRGBMainImageObjectType;

	m_ImageType = SVImageTypeRGBMain;

	// Set up default for Band Number...
	m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );	// RGB Image
}

SVRGBMainImageClass::~SVRGBMainImageClass()
{
}

HRESULT SVRGBMainImageClass::ResetObject()
{
	HRESULT l_hrOk = SVCameraImageTemplate::ResetObject();

	if( ! UpdateBuffer() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVRGBMainImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	return SetImageHandleIndex( svIndex );
}

BOOL SVRGBMainImageClass::SetImageHandleIndex(SVImageIndexStruct svIndex)
{
	BOOL bOk = SVCameraImageTemplate::SetImageHandleIndex( svIndex );

	m_CurrentIndex = svIndex;

	bOk &= UpdateBuffer();

	return bOk;
}

BOOL SVRGBMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr )
{
	BOOL bOk = !( p_ImageArrayPtr.empty() );

	long l_lDepth = 0;

	bOk = bOk && SetImageDepth( GetImageDepth() );

	if ( bOk )
	{
		long l_lBandNumber = 1;

		m_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );

		// Get Basic image info from ACB...
		m_ImageInfo = p_rImageInfo;

		// Set up necessary main image info...
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	
	return bOk;      
}

BOOL SVRGBMainImageClass::GetCameraImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( m_CurrentIndex.m_CameraDMIndexHandle.GetIndex(), p_rHandlePtr );
	}
	
	return bOk;
}

BOOL SVRGBMainImageClass::GetCameraImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( svIndex.m_CameraDMIndexHandle.GetIndex(), rHandle );
	}
	
	return bOk;
}

BOOL SVRGBMainImageClass::UpdateBuffer()
{
	BOOL bOk = TRUE;

	// Check for Dependents
	SVSmartHandlePointer MainHandle;
	SVSmartHandlePointer CameraHandle;

	if( GetImageHandle( MainHandle ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		MainHandle->GetData( l_MilHandle );

		if( GetCameraImageHandle( CameraHandle ) )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVImageBufferHandleImage l_CameraMilHandle;
			CameraHandle->GetData( l_CameraMilHandle );

			// Converts it to Hue, Luminance, Saturation (HLS).
			l_Code = SVMatroxImageInterface::Convert( l_MilHandle.GetBuffer(), l_CameraMilHandle.GetBuffer(), SVImageRGBToHLS );
		}
		else
		{
			SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0 );
		}
	}
	else
	{
		bOk = FALSE;
	}

	return bOk;
}

