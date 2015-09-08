///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
///******************************************************************************
/// This class provide the interface to get the image data of a matrox image.
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVUtilityLibrary\SVBitmapInfo.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//this class is a interface. It should only have pure virtual public method and new member parameter
		class IMatroxImageData
		{
		public:
			virtual ~IMatroxImageData() {}

			//************************************
			//! Return true if data container empty.
			//! \returns bool
			//************************************
			virtual bool empty() const = 0;

			//************************************
			//! Get bitmap info. If empty() == true, empty bitmap info is returned.
			//! \returns SVBitmapInfo
			//************************************
			virtual SVBitmapInfo getBitmapInfo() const = 0;

			//************************************
			//! get buffer address. If empty()==true, return value is nullptr.
			//! \returns BYTE*
			//************************************
			virtual BYTE* getBufferAddress() const = 0;

			//************************************
			//! Return a matrox buffer
			//! \param buffer [out] The matrox buffer
			//! \returns bool If matrox buffer is set, return value is true.
			//************************************
			virtual bool GetBuffer(SVMatroxBuffer& buffer) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;