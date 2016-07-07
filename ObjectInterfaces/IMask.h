//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the Mask Operator class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "ObjectInterfaces\ISVImage.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		class IMask
		{
		public:
			virtual ~IMask() {}
			virtual MatroxImageSmartHandlePtr GetReferenceImage() const = 0;
			virtual MatroxImageSmartHandlePtr GetMaskImage() const = 0;
			virtual HRESULT Import(const SVString& filename) = 0;
			virtual HRESULT Export(const SVString& filename) = 0;
			virtual HGLOBAL GetMaskData() const = 0;
			virtual bool SetMaskData(HGLOBAL hGlobal) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;