//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVMainImageClass
//* .File Name       : $Workfile:   SVMainImageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMainImageClass.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVMainImageClass, SVMainImageClassGuid );

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
	m_ImageType = SvPb::SVImageTypeEnum::SVImageTypeMain;
}

SVMainImageClass::~SVMainImageClass()
{
}

bool SVMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo)
{
	// Get Basic image info from ACB...
	m_ImageInfo = p_rImageInfo;

	// Set up necessary main image info...
	m_ImageInfo.SetOwnerImage( getObjectId() );

	m_LastUpdate = SvUl::GetTimeStamp();
     
	return true;
}

} //namespace SvIe
