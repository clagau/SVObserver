//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object application class
//******************************************************************************

#pragma once
#include "IObjectClass.h"

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IObjectAppClass: virtual public IObjectClass
		{
		public:
			/**********
			 The method gets the tool object of this object.
			***********/
			virtual IObjectClass* GetToolInterface() const = 0;

			/**********
			 Call this method at the object owner to create an object.
			 /param rChildObject <in> child object to create.
			 /param context <in>.
			 /return Result value SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED (If the owner object is not created yet)
			 	           DWORD_PTR is needed because return defines are UINT_PTR
			***********/
			virtual DWORD_PTR CreateChildObject(IObjectClass& rChildObject, DWORD context) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;