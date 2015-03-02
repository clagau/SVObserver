//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for ranking filter class
//******************************************************************************

#pragma once
#include "ISVFilter.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IRankingFilter: virtual public ISVFilter
		{
		public:
			/**********
			 The method add a input request for the width parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addWidthRequest(long value) = 0;
			/**********
			 The method gets the rank width value.
			***********/
			virtual long getWidth() const = 0;
			/**********
			 The method add a input request for the height parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addHeightRequest(long value) = 0;
			/**********
			 The method gets the rank height value.
			***********/
			virtual long getHeight() const = 0;
			/**********
			 The method add a input request for the rank parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addRankValueRequest(long value) = 0;
			/**********
			 The method gets the rank value.
			***********/
			virtual long getRankValue() const = 0;
			/**********
			 The method add a input request for the value at a requested rank cell to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param cellIndex <in> position of the rank cell
			 /param value <in> the new value.
			***********/
			virtual HRESULT addCellValueRequest(int cellIndex, long value) = 0;
			/**********
			 The method gets the value of a requested rank cell.
			 /param cellIndex <in> position of the rank cell
			***********/
			virtual long getCellValue(int cellIndex) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;