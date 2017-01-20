//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectWriter
//* .File Name       : $Workfile:   SVObjectWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/SVNameVariantList.h"
#pragma endregion Includes

class SVObjectWriter
{
public:
	SVObjectWriter() {}
	virtual ~SVObjectWriter() {}

	virtual void WriteAttribute(LPCTSTR rName, const _variant_t& value)=0;
	virtual void WriteAttribute(LPCTSTR rName, const SVVariantList& rValues)=0;

	virtual void StartElement(LPCTSTR rName)=0;
	virtual void EndElement()=0;
	virtual void ElementAttribute(LPCTSTR rAttrName, const _variant_t& value)=0;
};

