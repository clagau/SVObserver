//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPImageDataElement
//* .File Name       : $Workfile:   SVIPImageDataElement.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:24  $
//******************************************************************************
#pragma once
#pragma region Includes
//Moved to precompiled header: #include <map>

#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

struct SVIPImageDataElement
{
	SVByteVector m_ImageDIB;
	SVExtentMultiLineStructCArray m_OverlayData;

	SVIPImageDataElement() : m_ImageDIB(), m_OverlayData() {}
};

typedef std::map< SVGUID, SVIPImageDataElement > SVIPImageDataElementMap;

