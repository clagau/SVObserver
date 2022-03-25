//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file IObjectWriter.h
/// All Rights Reserved
//*****************************************************************************
/// This is an interface for the object writer
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvOi //< ObjectInterfaces
{
	class IObjectWriter
	{
	public:
		IObjectWriter() {}
		virtual ~IObjectWriter() {}

		virtual void WriteAttribute(LPCTSTR rName, const _variant_t& value)=0;
		virtual void WriteAttribute(LPCTSTR rName, const std::vector<_variant_t>& rValues)=0;

		virtual void StartElement(LPCTSTR rName)=0;
		virtual void EndElement()=0;
		virtual void ElementAttribute(LPCTSTR rAttrName, const _variant_t& value)=0;
	};
} //namespace SvOi
