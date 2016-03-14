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

#include "stdafx.h"
#include "SVClassRegisterClass.h"

#include "SVObjectLibrary.h"

SVClassRegisterClass::SVClassRegisterClass() 
{ 
}

SVGUID SVClassRegisterClass::GetClassID() 
{ 
	ASSERT( 0 );
	return SVInvalidGUID; 
}

SVObjectClass* SVClassRegisterClass::Construct() 
{ 
	ASSERT( 0 );
	return NULL; 
}

LPCTSTR SVClassRegisterClass::GetClassName()	
{ 
	ASSERT( 0 );
	return _T( "" ); 
}

