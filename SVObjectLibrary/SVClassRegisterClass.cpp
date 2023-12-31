//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassRegisterClass
//* .File Name       : $Workfile:   SVClassRegisterClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVClassRegisterClass.h"

#include "SVObjectLibrary.h"
#pragma endregion Includes

SVClassRegisterClass::SVClassRegisterClass() 
{ 
}

SVClassRegisterClass::~SVClassRegisterClass()
{
}

SvPb::ClassIdEnum SVClassRegisterClass::GetClassID() const
{ 
	Log_Assert( 0 );
	return SvPb::NoObjectClassId;
}

SVObjectClass* SVClassRegisterClass::Construct() 
{ 
	Log_Assert( 0 );
	return nullptr; 
}

LPCTSTR SVClassRegisterClass::GetClassName() const
{ 
	Log_Assert( 0 );
	return _T( "" ); 
}

