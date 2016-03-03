//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the blob analyzer
//******************************************************************************
#pragma once

#pragma region Includes
#include "NameValueList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		class IBlobAnalyzer
		{
		public:
			virtual ~IBlobAnalyzer() {}

			virtual NameValueList getFeatureList(bool isSelected) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;