//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for linked (value) object.
//******************************************************************************

#pragma once
#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvPb
{
	class LinkedValue;
}
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

		/// Fill the linked Data structure for the protobuf message.
		/// \param rLinkedValue [in,out] protobuf message
		virtual void fillLinkedData(SvPb::LinkedValue& rLinkedValue) const = 0;

		//! Validate the value. If value invalid an exception message will be thrown.
		//! \param rValue [in] The value to validate
		//! /// \returns _variant_t Return the current value if this value will be set.
		virtual _variant_t validateValue(const SvPb::LinkedValue& rLinkedValue) const = 0;

		virtual HRESULT setValue(const SvPb::LinkedValue& rData) = 0;

		/// Old for loading old configs older than 10.20
		/// Set indirect value string to LinkedValue to help to convert LinkedValue from old to new struct.
		virtual HRESULT setIndirectStringForOldStruct(const std::vector<_variant_t>& rValueString) = 0;

		/// Set a list of ids for the children.
		/// \param rObjectIds [in]
		virtual void setChildIds(const std::vector<uint32_t>& rObjectIds) = 0;
	};
} //namespace SvOi
