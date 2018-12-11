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
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVProtobuf/SVO-Enum.h"
#include "SVUtilityLibrary/NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVStatusLibrary\MessageContainer.h
namespace SvStl
{
	class MessageContainer;
	typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

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
		virtual std::string GetCompleteName() const = 0;

		/**********
			This method gets the complete name of this object, to a certain lineage.
			Note: this method builds the name by removing the parts of the ancestry younger than the objectType.
		***********/
		virtual HRESULT GetCompleteNameToType(SvPb::SVObjectTypeEnum objectType, std::string& rName) const = 0;

		/**********
			This method gets the complete name of this object, to a certain lineage.
			Note: this method builds the name in reverse by ancestry (meaning that the lineage is traversed by youngest to oldest)
		***********/
		virtual std::string GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const = 0;

		//Get the complete object name before selected SVObjectTypeEnum value.
		virtual std::string GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const = 0;

		// Check if the object if valid.
		virtual bool IsValid() const = 0;

		/**********
			The method gets the type of this object.
		***********/
		virtual const SvPb::SVObjectTypeEnum& GetObjectType() const = 0;

		/**********
			The method gets the subtype of this object.
		***********/
		virtual SvPb::SVObjectSubTypeEnum GetObjectSubType() const = 0;

		/**********
			The method gets the parent or nullptr if no parent.
		***********/
		virtual const SVGUID& GetParentID() const = 0;

		/**********
			The method gets the ancestor with a special type or nullptr if not found.
			/param ancestorObjectType <in> 
		***********/
		virtual IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) = 0;
		virtual const IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) const = 0;

		/**********
		This method gets the attributes allowed for the object.
		***********/
		virtual UINT ObjectAttributesAllowed() const = 0;
		virtual UINT SetObjectAttributesAllowed( UINT Attributes, SetAttributeType Type ) = 0;

		/**********
		This method gets the attributes set for the object.
		***********/
		virtual UINT ObjectAttributesSet(int iIndex=0) const = 0;
		virtual UINT SetObjectAttributesSet( UINT Attributes, SetAttributeType Type, int Index=0 ) = 0;

		/*
		This method returns the object's Global Unique Identifier (GUID) of this object instance.
		*/
		virtual const SVGUID& GetUniqueObjectID() const = 0;

		/// This method returns the embedded ID of the object
		virtual const SVGUID& GetEmbeddedID() const = 0;

		/*
		This method returns the creation state of the object.
		*/
		virtual bool is_Created() const = 0;

		//************************************
		/// This method return a list of name/GUID(classID) pairs of classes which are creatable form this object.
		/// \param pObjectTypeInfo [in] A object type info to filter the result list.
		/// \returns SvUl::NameGuidList A list of Name and ClassID pairs
		//************************************
		virtual SvUl::NameGuidList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const = 0;

		/*
		Set user changeable name.
		*/
		virtual void SetName( LPCTSTR StrString ) = 0;

		/// Return the first object which the required object type.
		/// \param rObjectTypeInfo [in] The required object type.
		/// \param useFriends [in] Search also in the friend list for this object. Default is true.
		/// \param pRequestor [in]
		/// \returns IObjectClass* Pointer to the object, if not found nullptr.
		virtual IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const IObjectClass* pRequestor = nullptr) const = 0;

		/// Move an object in the friend-list to a position in front of another object.
		/// \param objectToMoveId [in] GUID of the object to move.
		/// \param preObjectId [in] GUID of the object in front of the other object should moved. If this GUID is GUID_NULL, the other object will be moved to the end of the list.
		virtual void moveFriendObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId = GUID_NULL) = 0;

		/// Reset this object and all children and required all friends.
		/// \param pErrorMessages [in,out] Pointer to an Error Message Container. If the pointer unequal nullptr, an error message will be added if it happens during reset.
		/// \returns bool
		virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages=nullptr) = 0;

		/// Get the object class ID
		virtual SVGUID GetClassID() const = 0;
			
		//! Gets the value for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to write the value to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		virtual HRESULT getValue(double& rValue, int Index = -1) const = 0;

		//! Gets the values for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to double vector to store the values
		//! \returns S_OK if succeeded
		virtual HRESULT getValues(std::vector<double>& rValues) const = 0;
	};
} //namespace SvOi
