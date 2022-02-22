//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

struct SVIPImageDataElement
{
	std::string m_ImageDIB;
	SVExtentMultiLineStructVector m_OverlayData;

	SVIPImageDataElement() : m_ImageDIB(), m_OverlayData() {}
};

typedef std::map< uint32_t, SVIPImageDataElement > SVIPImageDataElementMap;

