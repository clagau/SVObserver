//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for filter class
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class ISVFilter
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