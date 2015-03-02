//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class custom filter
//******************************************************************************

#pragma once
#include "ISVFilter.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class ICustomFilter: virtual public ISVFilter
		{
		public:
			/**********
			 The method add a input request for the kernel width parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addKernelWidthRequest(long value) = 0;
			/**********
			 The method gets the kernel width value.
			***********/
			virtual long getKernelWidth() const = 0;
			/**********
			 The method add a input request for the kernel height parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addKernelHeightRequest(long value) = 0;
			/**********
			 The method gets the kernel height value.
			***********/
			virtual long getKernelHeight() const = 0;
			/**********
			 The method add a input request for the clipping on parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addClippingOnRequest(bool value) = 0;
			/**********
			 The method gets the clipping on value.
			***********/
			virtual bool isClippingOn() const = 0;
			/**********
			 The method add a input request for the absolute parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addAbsoluteValueRequest(bool value) = 0;
			/**********
			 The method gets the bool-value of absolute value.
			***********/
			virtual bool isAbsoluteValue() const = 0;
			/**********
			 The method add a input request for the transformation factor to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addTransformationFactorRequest(long value) = 0;
			/**********
			 The method gets the transformation factor value.
			***********/
			virtual long getTransformationFactor() const = 0;
			/**********
			 The method add a input request for the value at a requested kernel cell to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param cellIndex <in> position of the rank cell
			 /param value <in> the new value.
			***********/
			virtual HRESULT addKernelCellRequest(int cellIndex, long value) = 0;
			/**********
			 The method gets the value of a requested kernel cell.
			 /param cellIndex <in> position of the kernel cell
			***********/
			virtual long getKernelCellValue(int cellIndex) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;