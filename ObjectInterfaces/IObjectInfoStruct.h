//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object info struct
//******************************************************************************

#pragma once
#include "IObjectClass.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IObjectInfoStruct
		{
		public:
			//************************************
			//! Get the object.
			//! \returns IObjectClass*
			//************************************
			virtual IObjectClass* getObject() const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;