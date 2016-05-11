//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageListClass
//* .File Name       : $Workfile:   SVImageListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:53:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageListClass.h"
#pragma endregion Includes

SVImageListClass::SVImageListClass()
{
}

SVImageListClass::~SVImageListClass()
{
	RemoveAll();
}
