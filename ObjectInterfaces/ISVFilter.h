//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for filter class
//******************************************************************************

#pragma once
#pragma region Includes
#include "ITaskObject.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class ISVFilter: virtual public ITaskObject
		{
		public:
			virtual ~ISVFilter() {}

			/**********
			 The method gets if the inspection should reset if this filter is destroyed.
			***********/
			virtual bool shouldResetInspection() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;