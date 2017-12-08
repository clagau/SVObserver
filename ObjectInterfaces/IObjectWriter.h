//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file IObjectWriter.h
/// All Rights Reserved
//*****************************************************************************
/// This is an interface for the object writer
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVVariantList.h"
#pragma endregion Includes

namespace SvOi
{
	class IObjectWriter
	{
	public:
		IObjectWriter() {}
		virtual ~IObjectWriter() {}

		virtual void WriteAttribute(LPCTSTR rName, const _variant_t& value)=0;
		virtual void WriteAttribute(LPCTSTR rName, const SVVariantList& rValues)=0;

		virtual void StartElement(LPCTSTR rName)=0;
		virtual void EndElement()=0;
		virtual void ElementAttribute(LPCTSTR rAttrName, const _variant_t& value)=0;
	};
} //namespace SvOi
