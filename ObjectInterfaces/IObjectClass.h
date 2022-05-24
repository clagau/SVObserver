//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for object class
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#include "SVProtoBuf/SVO-Enum.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvDef
{
struct SVObjectTypeInfoStruct;
}

//! Declaration is in SVStatusLibrary\MessageContainer.h
namespace SvStl
{
	class MessageContainer;
	typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvPb
{
	class TreeItem;
}
#pragma endregion Declarations

namespace SvOi
{
	class IObjectClass;
	typedef std::function<bool(const IObjectClass* pObject, unsigned int Attribute, int ArrayIndex)> IsObjectAllowedFunc;
	
	enum SetAttributeType
	{
		AddAttribute,
		RemoveAttribute,
		OverwriteAttribute
	};

	enum class ObjectNotificationType
	{
		Deleting,
		ObjectIdChange
	};

	using ObjectNotificationFunction = std::function<void(SvOi::ObjectNotificationType, uint32_t)>;
	using ObjectNotificationFunctionPtr = std::shared_ptr<ObjectNotificationFunction>;
	using ObjectNotificationRAIIPtr = std::unique_ptr<void, std::function<void(void*)>>;

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
		virtual std::string GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude, bool withOwnName = true) const = 0;

		//Get the complete object name before selected SVObjectTypeEnum value.
		virtual std::string GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const = 0;

		/**********
			The method gets the type of this object.
		***********/
		virtual const SvPb::SVObjectTypeEnum& GetObjectType() const = 0;

		/**********
			The method gets the subtype of this object.
		***********/
		virtual SvPb::SVObjectSubTypeEnum GetObjectSubType() const = 0;

		/**********
			The method gets the objectId parent or 0 if no parent.
		***********/
		virtual uint32_t GetParentID() const = 0;

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
		This method returns the object's Global Unique Identifier (uint32_t) of this object instance.
		*/
		virtual uint32_t getObjectId() const = 0;

		/// This method returns the embedded ID of the object
		virtual SvPb::EmbeddedIdEnum GetEmbeddedID() const = 0;

		/*
		This method returns the creation state of the object.
		*/
		virtual bool is_Created() const = 0;

		//************************************
		/// This method return a list of name/classID pairs of classes which are creatable form this object.
		/// \param pObjectTypeInfo [in] A object type info to filter the result list.
		/// \returns SvUl::NameClassIdList A list of Name and ClassID pairs
		//************************************
		virtual SvUl::NameClassIdList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const = 0;

		/// Verify the new name and if correct it set the name and notify also the other objects.
		/// \param newName [in]
		/// \returns SvStl::MessageContainerVector ErrorMessage-Vector: If setting error-free, vector is empty.
		virtual SvStl::MessageContainerVector verifyAndSetName(const std::string& newName) = 0;

		/// Return the first object which the required object type.
		/// \param rObjectTypeInfo [in] The required object type.
		/// \param useFriends [in] Search also in the friend list for this object. Default is true.
		/// \param pRequestor [in]
		/// \returns IObjectClass* Pointer to the object, if not found nullptr.
		virtual IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const IObjectClass* pRequestor = nullptr) const = 0;

		/// Reset this object and all children and required all friends.
		/// \param pErrorMessages [in,out] Pointer to an Error Message Container. If the pointer unequal nullptr, an error message will be added if it happens during reset.
		/// \returns bool
		virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages=nullptr) = 0;

		/// Get the object class ID
		virtual SvPb::ClassIdEnum GetClassID() const = 0;
			
		//! Gets the value for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to write the value to
		//! \param Index [in] The corresponding array index to write to, if required
		//! \returns S_OK if succeeded
		virtual HRESULT getValue(double& rValue, int Index = -1) const = 0;

		//! Gets the values for Value object NOTE: This is placed here instead of IValueObject for performance reasons !
		//! \param rValue [out] The reference to double vector to store the values
		//! \returns S_OK if succeeded
		virtual HRESULT getValues(std::vector<double>& rValues) const = 0;

		virtual void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const = 0;

		/// Fill a list with object for selector, which fit requirements.
		/// \param treeInserter [in,out] Back_inserter where the object will be added.
		/// \param pFunctor [in]
		/// \param attribute [in]
		/// \param wholeArray [in]
		/// \param nameToType [in] Define to which type the dotted name should be added.
		/// \param requiredType [in]
		/// \param stopIfClosed [in,out] If true, the search for objects will be stop if the object is closed (by now only possible be GroupTool). Default is false, this means it will be searched to the end of the children.
		/// \param firstObject [in,out] Only used if stopIfClosed is true: If startObject is e.g. GroupTool is should go to there children.
		virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const = 0;

		/// Fill a list with object, which fit requirements.
		/// \param inserter [in,out] Back_inserter where the object will be added.
		/// \param rObjectInfo [in] If the parameter set, only objects will be added the fit to this parameters.
		/// \param addHidden [in] If true, also hidden objects will be added. Default is false, the means hidden objects will not be added.
		/// \param stopIfClosed [in] If true, the search for objects will be stop if the object is closed (by now only possible be GroupTool). Default is false, this means it will be searched to the end of the children.
		/// \param firstObject [in] Only used if stopIfClosed is true: If startObject is e.g. GroupTool is should go to there children.
		virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) = 0;

		/// Get the parent up and return the object id if it closed (e.g. a groupTool). It stop searching if parent have stopSearchAtObjectId.
		/// If no closing object found the return value is nullptr.
		/// \param stopSearchAtObjectId [in]
		/// \returns uint32_t
		virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const = 0;

		virtual bool allowExtensionCopy() const = 0;
	};
} //namespace SvOi
