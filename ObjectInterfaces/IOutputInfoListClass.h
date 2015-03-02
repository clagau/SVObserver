//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for output info list class
//******************************************************************************

#pragma once
#include "IObjectInfoStruct.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IOutputInfoListClass
		{
		public:
			//************************************
			//! Return the size of the list.
			//! \returns int
			//************************************
			virtual int GetSize() const = 0;

			//************************************
			//! Get the output interface at index position.
			//! \param index [in,out]
			//! \returns IObjectInfoStruct*
			//************************************
			virtual IObjectInfoStruct* GetInterfaceAt(int index) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;