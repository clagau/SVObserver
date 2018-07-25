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
	m_ImageType = SvDef::SVImageTypeEnum::SVImageTypeMain;
}

SVMainImageClass::~SVMainImageClass()
{
}

bool SVMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo)
{
	// Get Basic image info from ACB...
	m_ImageInfo = p_rImageInfo;

	// Set up necessary main image info...
	m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

	m_LastUpdate = SvTl::GetTimeStamp();
     
	return true;
}