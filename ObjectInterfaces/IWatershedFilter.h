//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for watershed filter class
//******************************************************************************

#pragma once
#include <vector>
#include "ISVFilter.h"

class SVString;

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IWatershedFilter: virtual public ISVFilter
		{
		public:
			/**********
			 The method add a input request for the control flag parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addControlFlagRequest(long value) = 0;
			/**********
			 The method gets the control flag value.
			***********/
			virtual long getControlFlag() const = 0;
			/**********
			 The method add a input request for the minimum variation parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addMinVariationRequest(long value) = 0;
			/**********
			 The method gets the minimum variation.
			***********/
			virtual long getMinVariation() const = 0;
			/**********
			 The method add a input request for the marker used parameter to the IP. 
			 Add the end of setting the input request, you have to set AddInputRequestMarker.
			 /param value <in> the new value.
			***********/
			virtual HRESULT addMarkerUsedRequest(bool value) = 0;
			/**********
			 The method gets if the marker used.
			***********/
			virtual bool isMarkerUsed() const = 0;

			//************************************
			//! Get the name of the marker image. Only relevant if marker is used.
			//! \returns SVString
			//************************************
			virtual SVString getMarkerImage() const = 0;
			//************************************
			//! Set the marker image for the watershed. Only relevant if marker is used.
			//! \param imageName [in] Name of the image
			//! \returns HRESULT
			//************************************
			virtual HRESULT setMarkerImage(const SVString imageName) = 0;

			//************************************
			//! Return a list of available image to use for marker.
			//! \returns std::vector<SVString>
			//************************************
			virtual std::vector<SVString> getAvailableMarkerImageNames() = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;