//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectListClass
//* .File Name       : $Workfile:   SVObjectListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectListClass.h"
#include "SVObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectListClass::SVObjectListClass()
{
	SetSize( 0 );
}

SVObjectListClass::~SVObjectListClass()
{
	RemoveAll();
}
