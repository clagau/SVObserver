//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class ring buffer tool
//******************************************************************************

#pragma once
#pragma region Includes
#include "ITaskObjectListClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IRingBufferTool: virtual public ITaskObjectListClass
		{
		public:
			virtual ~IRingBufferTool() {}

			/**********
			 The method set the ring buffer depth with an input request + marker to the IP. 
			 /param value <in> the new value.
			 /return If string is not a valid reference or value, it will return an error, else S_OK.
			***********/
			virtual HRESULT setRingDepth(SVString value) = 0;
			/**********
			 The method gets the ring buffer depth as string.
			***********/
			virtual SVString getRingBufferDepthString() const = 0;

			/**********
			 The method set image index with an input request + marker to the IP. 
			 /param indexNumber <in> which index value will be set.
			 /param value <in> the new value.
			 /return If string is not a valid reference or value, it will return an error, else S_OK.
			***********/
			virtual HRESULT setImageIndex(int indexNumber, SVString value) = 0;
			/**********
			 The method gets the image index value of a required index. If indexNumber invalid, it will return an empty string.
			 /param indexNumber <in> which index value will be get.
			***********/
			virtual SVString getImageIndex(int indexNumber) const = 0;

			static const int m_minRingBufferDepth = 2;
			static const int m_maxRingBufferDepth = 30;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;