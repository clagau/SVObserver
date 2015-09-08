//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for thinning filter class
//******************************************************************************

#pragma once
#pragma region Includes
#include "ISVFilter.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IThinningFilter: virtual public ISVFilter
		{
		public:
			virtual ~IThinningFilter() {}

			/**********
			 The method add a input request for the gray on parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addGrayOnRequest(bool value) = 0;
			/**********
			 The method gets the gray on value.
			***********/
			virtual bool isGrayOn() const = 0;
			/**********
			 The method add a input request for the iteration parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addIterationRequest(long value) = 0;
			/**********
			 The method gets the iteration value.
			***********/
			virtual long getIteration() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;