//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeSelectorFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include <set>
#include "ObjectInterfaces\IObjectInfoStruct.h"
#include "ObjectSelectorLibrary\SelectorOptions.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class RangeSelectorFilter
		{
		private:
			std::set<SVObjectTypeEnum> m_filter;
			const SvOsl::SelectorOptions& m_rOptions;
			SVString m_excludePath;

		public:
			RangeSelectorFilter( const SvOsl::SelectorOptions& rOptions );
			bool operator()(const SvOi::IObjectInfoStruct& rInfo, int ArrayIndex) const;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;