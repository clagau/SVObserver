//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for object class
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#include "SVObjectTypeInfoStruct.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace SvOi
{
	enum SetAttributeType
	{
		AddAttribute,
		RemoveAttribute,
		OverwriteAttribute
	};

	//this class is a interface. It should only have pure virtual public method and no member variables
	class IObjectClass
	{
	public:
		virtual ~IObjectClass() {}

		/**********
			The method gets the name of this object.
		***********/
		virtual LPCTSTR GetName() const = 0;

		/**********
			This method gets the complete name of this object.
		***********/
		virtual SVString GetCompleteName() const = 0;

		/**********
			This method gets the complete name of this object, to a certain lineage.
			Note: this method builds the name by removing the parts of the ancestry younger than the objectType.
		***********/
		virtual HRESULT GetCompleteNameToType(SVObjectTypeEnum objectType, SVString& rName) const = 0;

		/**********
			This method gets the complete name of this object, to a certain lineage.
			Note: this method builds the name in reverse by ancestry (meaning that the lineage is traversed by youngest to oldest)
		***********/
		virtual SVString GetObjectNameToObjectType(LPCSTR LPSZCompleteName, SVObjectTypeEnum objectTypeToInclude) const = 0;

		/**********
			The method gets the type of this object.
		***********/
		virtual const SVObjectTypeEnum& GetObjectType() const = 0;

		/**********
			The method gets the subtype of this object.
		***********/
		virtual SVObjectSubTypeEnum GetObjectSubType() const = 0;

		/**********
			The method gets the parent or nullptr if no parent.
		***********/
		virtual const SVGUID& GetParentID() const = 0;

		/**********
			The method gets the ancestor with a special type or nullptr if not found.
			/param ancestorObjectType <in> 
		***********/
		virtual IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) = 0;
		virtual const IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) const = 0;

		/**********
		This method gets the attributes allowed for the object.
		***********/
		virtual const UINT& ObjectAttributesAllowed() const = 0;
		virtual const UINT& SetObjectAttributesAllowed( UINT Attributes, SetAttributeType Type ) = 0;

		/**********
		This method gets the attributes set for the object.
		***********/
		virtual const UINT& ObjectAttributesSet(int iIndex=0) const = 0;
		virtual const UINT& SetObjectAttributesSet( UINT Attributes, SetAttributeType Type, int Index=0 ) = 0;

		/*
		This method returns the object's Global Unique Identifier (GUID) of this object instance.
		*/
		virtual const SVGUID& GetUniqueObjectID() const = 0;

		/*
		This method returns the creation state of the object.
		*/
		virtual bool is_Created() const = 0;

		//************************************
		/// This method return a list of name/GUID(classID) pairs of classes which are creatable form this object.
		/// \param pObjectTypeInfo [in] A object type info to filter the result list.
		/// \returns SvUl::NameGuidList A list of Name and ClassID pairs
		//************************************
		virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const = 0;

		/*
		Set user changeable name.
		*/
		virtual void SetName( LPCTSTR StrString ) = 0;

		/// Return the first object which the required object type.
		/// \param rObjectTypeInfo [in] The required object type.
		/// \param useFriends [in] Search also in the friend list for this object. Default is true.
		/// \param pRequestor [in]
		/// \returns IObjectClass* Pointer to the object, if not found nullptr.
		virtual IObjectClass* getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const IObjectClass* pRequestor = nullptr) const = 0;

		/// Reset this object and all children and required all friends.
		/// \param pErrorMessages [in,out] Pointer to an Error Message Container. If the pointer unequal nullptr, an error message will be added if it happens during reset.
		/// \returns bool
		virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages=nullptr) = 0;

		/// Get the object class ID
		virtual SVGUID GetClassID() = 0;
			
		//! Gets the value for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to write the value to
		//! \param Bucket [in] The corresponding bucket index to get, if required
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		virtual HRESULT getValue(double& rValue, int Bucket = -1, int Index = -1) const = 0;

		//! Gets the values for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to double vector to store the values
		//! \param Bucket [in] The corresponding bucket index to get, if required
		//! \returns S_OK if succeeded
		virtual HRESULT getValues(std::vector<double>& rValues, int Bucket = -1) const = 0;
	};
} //namespace SvOi
