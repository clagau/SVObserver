//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectClass
//* .File Name       : $Workfile:   SVObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 02:45:14  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <iterator>

#include "ObjectInterfaces/IObjectClass.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVObjectInfoArrayClass.h"
#include "SVUtilityLibrary/NameGuidList.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvOi
{
class IObjectWriter;
}
namespace SvOl
{
struct SVInObjectInfoStruct;
class SVInputInfoListClass;
}
struct SVObjectLevelCreateStruct;
struct SVObjectNameInfo;

class SVObjectAttributeClass;
#pragma endregion Declarations

/*
This class is the base class to all configuration level objects.  These objects will get created and managed by the object manager object.
*/
class SVObjectClass : public SvOi::IObjectClass
{
	SV_DECLARE_CLASS( SVObjectClass );
	
	enum RefreshObjectType
	{
		None,
		PreRefresh,
		PostRefresh,
	};

public:
	typedef std::deque< SVObjectClass* > SVObjectPtrDeque;

	friend class SVObjectManagerClass;	// @TODO - This needs to go - For access to m_outObjectInfo to assignUnique GUIDs on loading
	friend class SVConfigurationObject; // @TODO - This needs to go - For access to m_outObjectInfo to assignUnique GUIDs on loading

	SVObjectClass();
	SVObjectClass( LPCTSTR ObjectName );
	SVObjectClass( SVObjectClass* pOwner, int StringResourceID );

	virtual ~SVObjectClass();

	template< typename SVObjectVisitor >
	HRESULT Accept( SVObjectVisitor& rVisitor );

	/*
	This method is a placeholder for the object reset functionality.  This method will be overridden by derived classes.
	*/
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	virtual void ResetPrivateInputInterface();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual bool CloseObject();
	virtual bool SetObjectOwner( SVObjectClass* pNewOwner );
	virtual bool SetObjectOwner( const GUID& rNewOwnerGUID );

	virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const;
	virtual HRESULT SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	/// Set status color to disabled (in SVTaskObjectClass), but also for all children
	virtual void SetDisabled();
	virtual DWORD GetObjectColor() const;

	virtual void Persist( SvOi::IObjectWriter& rWriter );

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	virtual HRESULT ResetObjectInputs();

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	virtual bool ConnectAllInputs() { return false; };
	bool ConnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo );
	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo );

	virtual bool isInputImage(const SVGUID& rImageGuid) const { return false; };

	virtual void ResetName();
	virtual void SetObjectName( LPCTSTR ObjectName );

	void SetObjectEmbedded( const GUID& rEmbeddedID, SVObjectClass* pOwner, LPCTSTR NewObjectName );

	/// Add the object to the friend list.
	/// \param rFriendGUID [in] Guid of the object
	/// \param rAddPreGuid [in] The new object will be added before this object. Default: GUID_NULL This means: it will be added at the end.
	/// \returns bool
	bool AddFriend( const GUID& rFriendGUID, const GUID& rAddPreGuid = GUID_NULL );
	void DestroyFriends();

	/// Destroy a friend (Disconnect, CloseObject and Destroy his friend), but it must be a taskObject. 
	/// \param pObject [in] object to destroy.
	virtual void DestroyFriend(SVObjectClass* pObject) {};
	//************************************
	//! this function returns a pointer to the friendobject which fit the ObjectType, if any. Otherwise it returns nullptr. 
	//! \param rObjectType [in]
	//! \returns SVObjectClass*
	//************************************
	SVObjectClass* GetFriend( const SvDef::SVObjectTypeInfoStruct& rObjectType ) const; 

	//************************************
	// Method:    GetAncestor
	// Description: This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns nullptr.
	// Parameter: SVObjectTypeEnum AncestorObjectType 
	// Parameter: topLevel If true searches further if another parent is of object type
	// Returns:   SVObjectClass*
	//************************************
	SVObjectClass* GetAncestor( SvPb::SVObjectTypeEnum AncestorObjectType, bool topLevel=false ) const;
	LPCTSTR GetObjectName() const;

	SVObjectClass* GetParent() const;
	SVOutObjectInfoStruct& GetObjectOutputInfo();
	
	bool IsCreated() const;

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject );
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject );

#pragma region virtual method (IObjectClass)
	virtual LPCTSTR GetName() const override;
	virtual std::string GetCompleteName() const override;
	virtual HRESULT GetCompleteNameToType(SvPb::SVObjectTypeEnum objectType, std::string& rName) const override;
	virtual std::string GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVToolSetObjectType) const override;
	//Get the complete object name before selected SvPb::SVObjectTypeEnum value.
	virtual std::string GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const override;
	virtual const SvPb::SVObjectTypeEnum& GetObjectType() const override;
	virtual SvPb::SVObjectSubTypeEnum GetObjectSubType() const override;
	virtual const SVGUID& GetParentID() const override;
	virtual SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) override;
	virtual const SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) const override;
	virtual UINT ObjectAttributesAllowed() const override;
	virtual UINT SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type ) override;
	virtual UINT ObjectAttributesSet(int iIndex=0) const override;
	virtual UINT SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int iIndex=0 ) override;
	virtual const SVGUID& GetUniqueObjectID() const override;
	virtual const SVGUID& GetEmbeddedID() const override;
	virtual bool is_Created() const override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const override;
	virtual void SetName( LPCTSTR Name ) override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void moveFriendObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId = GUID_NULL) override;
	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override { return ResetObject(pErrorMessages); };
	virtual HRESULT getValue(double& rValue, int Index = -1) const override { return E_NOTIMPL; };
	virtual HRESULT getValues(std::vector<double>& rValues) const override { return E_NOTIMPL; };
#pragma endregion virtual method (IObjectClass)

	const SVObjectInfoStruct& GetOwnerInfo() const;
	const SVObjectInfoStruct& GetObjectInfo() const;
	const SVObjectInfoArrayClass& GetFriendList() const;

#pragma region Methods to replace processMessage
	/// Call the method createObject for all children and itself.
	/// \param rCreateStructure [in]
	/// \returns bool
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure );

	/// Call this method at the object owner to create an object
	/// \param pChildObject [in] child object to create
	/// \param context [in]
	/// \returns bool
	virtual bool CreateChildObject( SVObjectClass* pChildObject, DWORD context = 0 ) { return false; };

	/// Overwrite GUID of an embedded object.
	/// \param rUniqueID [in] New Guid of the object
	/// \param embeddedID [in] Embedded Id of the object to overwrite
	/// \returns SVObjectClass* Pointer to the overwritten object, nullptr if not found.
	virtual SVObjectClass* OverwriteEmbeddedObject(const GUID& rUniqueID, const GUID& rEmbeddedID);

	/// Get the input list combined also from children and if required from friends.
	/// \param inputList [in,out] Add the new input object to the end of the list.
	/// \param bAlsoFriends [in] If true, it adds also the inputs of the friend.
	virtual void GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const {};

	/// Will be called, if an object was renamed.
	/// \param rRenamedObject [in] Reference to the renamed object.
	/// \param rOldName [in] Old name of the object.
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) {};

	/// Replace the current object with new guids etc.
	/// \param pObject [in,out] Object t be removed.
	/// \param rNewGuid [in] The mew GUID of the object
	/// \returns bool
	virtual bool replaceObject(SVObjectClass* pObject, const GUID& rNewGuid) { return false; };
#pragma endregion Methods to replace processMessage

protected:
	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual SVObjectClass* UpdateObject( const GUID& rFriendGuid, SVObjectClass* pObject, SVObjectClass* pNewOwner );

	virtual HRESULT RemoveObjectConnection( const GUID& rObjectID );

	/// Call method createAllObjects for the child object with the right create struct.
	/// \param rChildObject [in]
	/// \returns bool
	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) { return false; };

protected:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_ObjectAttributesAllowed;
	//This attribute holds the array enumerated bits of set object attributes based on object array depth.
	std::vector<UINT> m_ObjectAttributesSet;

	// Refer to IsCreated()
	bool m_isCreated;
	//If object is embedded, set this ID
	SVGUID m_embeddedID;
	//Owner Info
	SVObjectInfoStruct m_ownerObjectInfo;
	//Contains the object info and could also be used as task out info.
	SVOutObjectInfoStruct m_outObjectInfo;
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVObjectInfoArrayClass m_friendList;

private:
	void init();

	//ATTENTION: order of the parameter (especially m_ObjectName before m_Name) is important, because it is needed for the constructors.
	int m_resourceID;		//String resource ID, of NOT user changeable name.
	std::string m_ObjectName;	//NOT user changeable name
	std::string m_Name;			//user given name
};

#pragma region Declarations
typedef std::vector<SVObjectClass*> SVObjectPtrVector;
typedef std::back_insert_iterator<SVObjectPtrVector> SVObjectPtrVectorInserter;
typedef std::set<SVObjectClass*> SVObjectPtrSet;
#pragma region Declarations

#include "SVObjectClass.inl"

