//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for filter class
//******************************************************************************

#pragma once

namespace SvOi
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
} //namespace SvOi
