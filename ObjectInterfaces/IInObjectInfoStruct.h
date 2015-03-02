//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for class input object info struct
//******************************************************************************

#pragma once
#include "IObjectInfoStruct.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IInObjectInfoStruct : virtual public IObjectInfoStruct
		{
		public:
			//************************************
			//! Is this object connected.
			//! \returns bool
			//************************************
			virtual bool IsConnected() const = 0;

			//************************************
			//! Get the info struct of connected object.
			//! \returns IObjectInfoStruct&
			//************************************
			virtual const IObjectInfoStruct& GetInputObjectInfoInterface() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;