//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for output info list class
//******************************************************************************

#pragma once
#pragma region Includes
#include "ISelectorItem.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

namespace SvOi
{
	class ISelectorItemVector
	{
	public:
		virtual ~ISelectorItemVector() {}

	};
	typedef SVSharedPtr<ISelectorItemVector> ISelectorItemVectorPtr;

} //namespace SvOi
