//******************************************************************************
/// COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for object application class
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvOi
{
class IObjectClass;
}

namespace SvOi
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
} //namespace SvOi
