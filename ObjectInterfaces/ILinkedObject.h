//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for linked (value) object.
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOi
{
	class IObjectClass;

	//this class is a interface. It should only have pure virtual public method and no member variables
	class ILinkedObject
	{
	public:
		virtual ~ILinkedObject() {}

		/// Return the last linked object. ATTENTION: Throw exception if is is a circular reference.
		/// \returns const SvOi::IObjectClass*
		virtual const IObjectClass* getLinkedObject() const = 0;
	};
} //namespace SvOi
