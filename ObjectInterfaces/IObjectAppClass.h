//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object application class
//******************************************************************************

#pragma once
#pragma region Includes
#include "IObjectClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IObjectAppClass
		{
		public:
			virtual ~IObjectAppClass() {}

			/**********
			 Call this method at the object owner to create an object.
			 /param rChildObject <in> child object to create.
			 /param context <in>.
			 /return bool
			***********/
			virtual bool CreateChildObject(IObjectClass& rChildObject, DWORD context) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;