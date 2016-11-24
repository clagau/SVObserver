//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVMainImageClass
//* .File Name       : $Workfile:   SVMainImageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVMainImageClass.h"

#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"

#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVObserver.h"

// Wasn't in old implementation, take back out for now. may be needed in future.
// SV_IMPLEMENT_CLASS( SVMainImageClass, SVMainImageClassGuid );

SVMainImageClass::SVMainImageClass( LPCSTR ObjectName )
: SVCameraImageTemplate( ObjectName )
{
	init();
}

SVMainImageClass::SVMainImageClass( SVObjectClass* POwner, int StringResourceID )
: SVCameraImageTemplate( POwner, StringResourceID )
{
	init();
}

void SVMainImageClass::init()
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVMainImageObjectType;

	m_ImageType = SVImageTypeMain;
}

SVMainImageClass::~SVMainImageClass()
{
}

BOOL SVMainImageClass::SetImageHandleIndex( SVImageIndexStruct svIndex )
{
	bool Result( true );

	m_CurrentIndex = svIndex;

	SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

	while( l_Iter != m_ChildArrays.end() )
	{
		if( nullptr != l_Iter->second.m_pImageHandles )
		{
			Result &= l_Iter->second.m_pImageHandles->SetCurrentIndex( svIndex.m_CameraDMIndexHandle );
		}

		++l_Iter;
	}
	
	return Result;
}

BOOL SVMainImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	BOOL bOk = FALSE;
	
	return bOk;
}

BOOL SVMainImageClass::GetImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = nullptr != GetCameraBufferArrayPtr();
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( m_CurrentIndex.m_CameraDMIndexHandle.GetIndex(), p_rHandlePtr );
	}
	
	return bOk;
}

BOOL SVMainImageClass::GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle )
{
	BOOL bOk = nullptr != GetCameraBufferArrayPtr();
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( svIndex.m_CameraDMIndexHandle.GetIndex(), rHandle );
	}
	
	return bOk;
}

BOOL SVMainImageClass::OnValidate()
{
	if( SVObjectAppClass::OnValidate() )
	{
		m_isObjectValid = !( GetCameraBufferArrayPtr().empty() );
	}

	return m_isObjectValid;
}

const SVClock::SVTimeStamp& SVMainImageClass::GetLastResetTimeStamp() const
{
	if( nullptr != GetCameraBufferArrayPtr() )
	{
		m_LastReset = std::max( m_LastReset, GetCameraBufferArrayPtr()->GetLastResetTimeStamp() );
	}

	return SVCameraImageTemplate::GetLastResetTimeStamp();
}
	
BOOL SVMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr )
{
	BOOL bOk = !( p_ImageArrayPtr.empty() );

	if ( bOk )
	{
		// Get Basic image info from ACB...
		m_ImageInfo = p_rImageInfo;

		// Set up necessary main image info...
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();
	}

	return bOk;      
}

SVImageObjectClassPtr SVMainImageClass::GetBufferArrayPtr() const
{
	return GetCameraBufferArrayPtr();
}

