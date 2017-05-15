//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ISelectorItemVector.h"
#include "SVContainerLibrary/SVVector.h"
#include "SelectorItem.h"
#pragma endregion Includes

namespace SvOsl
{
	class SelectorItemVector : public SvOi::ISelectorItemVector, public SVVector<SelectorItem> 
	{
	#pragma region Constructor
	public:
		SelectorItemVector() {};

		virtual ~SelectorItemVector() {};
	#pragma endregion Constructor
	};

} //namespace SvOsl
