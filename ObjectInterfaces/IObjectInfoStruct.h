//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object info struct
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost\function.hpp>
#include "IObjectClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		class IObjectInfoStruct
		{
		public:
			virtual ~IObjectInfoStruct() {}

			//************************************
			//! Get the object.
			//! \returns IObjectClass*
			//************************************
			virtual IObjectClass* getObject() const = 0;
		};

		typedef boost::function<bool (const IObjectInfoStruct& rInfo, int ArrayIndex)> IsObjectInfoAllowed;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;