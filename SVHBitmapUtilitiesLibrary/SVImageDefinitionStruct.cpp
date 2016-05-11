// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vistion Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageDefinitionStruct
// * .File Name       : $Workfile:   SVImageDefinitionStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:11:08  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImageDefinitionStruct.h"


// SVImageDefinitionStruct
SVImageDefinitionStruct::SVImageDefinitionStruct( )
{
	Init();
}


SVImageDefinitionStruct::SVImageDefinitionStruct(const SVImageDefinitionStruct& rhs )
{
	Init();
	*this = rhs;
}

SVImageDefinitionStruct::~SVImageDefinitionStruct( )
{
	Clear();
}

const SVImageDefinitionStruct& SVImageDefinitionStruct::operator = (const SVImageDefinitionStruct& rhs )
{

	if( this != &rhs )
	{
		Clear();
		lHeight = rhs.lHeight;
		lWidth = rhs.lWidth;
		eImageFormat = rhs.eImageFormat;
		bstrDisplayName = SysAllocString(rhs.bstrDisplayName);
	}
	return *this;
}

void SVImageDefinitionStruct::Clear( )
{
	lHeight = 0;
	lWidth = 0;
	eImageFormat = SVImageFormatUnknown;
	if(bstrDisplayName)
		::SysFreeString(bstrDisplayName);

}

void SVImageDefinitionStruct::Init( )
{
	lHeight = 0;
	lWidth = 0;
	eImageFormat = SVImageFormatUnknown;
	bstrDisplayName = nullptr;
}

