//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for the Mask Operator class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

namespace SvOi
{
	class IMask
	{
	public:
		virtual ~IMask() {}
		virtual HRESULT Import(const std::string& filename) = 0;
		virtual HRESULT Export(const std::string& filename) = 0;
		virtual HGLOBAL GetMaskData() const = 0;
		virtual bool SetMaskData(HGLOBAL hGlobal) = 0;
	};
} //namespace SvOi
