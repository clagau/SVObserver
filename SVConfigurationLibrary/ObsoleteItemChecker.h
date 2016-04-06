//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************
#pragma once

namespace Seidenader
{
	namespace Configuration
	{
		template<typename TreeType>
		static HRESULT HasObsoleteItem(TreeType& rTree, CString& rItemType, int& errorCode);
	}
}

#include "ObsoleteItemChecker.inl"

namespace SvOc = Seidenader::Configuration;