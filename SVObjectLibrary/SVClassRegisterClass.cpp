//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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

SVGUID SVClassRegisterClass::GetClassID() 
{ 
	assert( 0 );
	return GUID_NULL; 
}

SVObjectClass* SVClassRegisterClass::Construct() 
{ 
	assert( 0 );
	return nullptr; 
}

LPCTSTR SVClassRegisterClass::GetClassName()	
{ 
	assert( 0 );
	return _T( "" ); 
}

