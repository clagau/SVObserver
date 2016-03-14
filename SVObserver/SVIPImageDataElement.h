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

#ifndef SVIPIMAGEDATAELEMENT_H
#define SVIPIMAGEDATAELEMENT_H

#include <map>

#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary/SVGUID.h"

struct SVIPImageDataElement
{
	SVByteVector m_ImageDIB;
	SVExtentMultiLineStructCArray m_OverlayData;

	SVIPImageDataElement() : m_ImageDIB(), m_OverlayData() {}
};

typedef std::map< SVGUID, SVIPImageDataElement > SVIPImageDataElementMap;

#endif

