//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutObjectInfoStruct
//* .File Name       : $Workfile:   SVOutObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOutObjectInfoStruct.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOutObjectInfoStruct::SVOutObjectInfoStruct()
: SVObjectInfoStruct()
{
	
}

SVOutObjectInfoStruct::SVOutObjectInfoStruct( const SVOutObjectInfoStruct& p_rsvValue )
: SVObjectInfoStruct( p_rsvValue )
{
	
}

SVOutObjectInfoStruct::~SVOutObjectInfoStruct()
{
	
}

const SVOutObjectInfoStruct& SVOutObjectInfoStruct::operator=( const SVOutObjectInfoStruct& p_rsvValue )
{
	if( this != &p_rsvValue )
	{
		SVObjectInfoStruct::operator =( p_rsvValue );
	}

	return *this;
}

