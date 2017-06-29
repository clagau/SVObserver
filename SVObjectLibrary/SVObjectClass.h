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
#include "SVUtilityLibrary/SVString.h"

#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVPublicAttributeEntryStruct.h"
#include "SVObjectWriter.h"
#include "SVObjectInfoArrayClass.h"
#include "SVUtilityLibrary/NameGuidList.h"
#include "SVInputInfoListClass.h"
#include "SVStatusLibrary/MessageContainer.h"
#pragma endregion Includes

#pragma region Declarations
struct SVInObjectInfoStruct;
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

	int GetObjectDepth() const;
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual long GetImageDepth() const;
	virtual BOOL SetImageDepth( long lDepth );

	virtual BOOL InitObject( SVObjectClass* pObject );
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual BOOL CloseObject();
	virtual bool IsValid() const { return (TRUE == m_isObjectValid); }
	virtual BOOL SetObjectOwner( SVObjectClass* pNewOwner );
	virtual BOOL SetObjectOwner( const GUID& rNewOwnerGUID );

	virtual HRESULT GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const;
	virtual HRESULT SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual void SetInvalid();
	/// Set status color to disabled (in SVTaskObjectClass), but also for all children
	virtual void SetDisabled();
	virtual DWORD GetObjectColor() const;
	virtual DWORD GetObjectState() const;

	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistAttributes( SVObjectWriter& rWriter );

	virtual BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	virtual HRESULT ResetObjectInputs();

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	virtual bool ConnectAllInputs() { return false; };
	bool ConnectObjectInput( SVInObjectInfoStruct* pObjectInInfo );
	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo );
	BOOL IsDescendantOf( SVObjectClass* pAncestorObject );
	BOOL IsDescendantOfType( const SVObjectInfoStruct& rAncestorInfo );

	virtual void ResetName();
	virtual void SetObjectName( LPCTSTR ObjectName );

	void SetObjectEmbedded( const GUID& rEmbeddedID, SVObjectClass* pOwner, LPCTSTR NewObjectName );

	/// Add the object to the friend list.
	/// \param rFriendGUID [in] Guid of the object
	/// \param rAddPreGuid [in] The new object will be added before this object. Default: SV_GUID_NULL This means: it will be added at the end.
	/// \returns BOOL
	BOOL AddFriend( const GUID& rFriendGUID, const GUID& rAddPreGuid = SV_GUID_NULL );
	BOOL RemoveFriend( const GUID& rFriendGUID );
	void DestroyFriends();

	/// Destroy a friend (Disconnect, CloseObject and Destroy his friend), but it must be a taskObject. 
	/// \param pObject [in] object to destroy.
	virtual void DestroyFriend(SVObjectClass* pObject) {};
	//************************************
	//! this function returns a pointer to the friendobject which fit the ObjectType, if any. Otherwise it returns nullptr. 
	//! \param rObjectType [in]
	//! \returns SVObjectClass*
	//************************************
	SVObjectClass* GetFriend( const SVObjectTypeInfoStruct& rObjectType ) const; 
	SVPublicAttributeEntryStruct* GetPublicAttribute();

	//************************************
	// Method:    GetAncestor
	// Description: This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns nullptr.
	// Parameter: SVObjectTypeEnum AncestorObjectType 
	// Returns:   SVObjectClass*
	//************************************
	SVObjectClass* GetAncestor( SVObjectTypeEnum AncestorObjectType ) const;
	int GetNameLength() const;
	LPCTSTR GetObjectName() const;
	int GetObjectNameLength() const;

	SVString GetCompleteObjectNameToObjectType( LPCTSTR CompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;
	SVObjectClass* GetOwner() const;
	SVOutObjectInfoStruct& GetObjectOutputInfo();
	
	const SVGUID& GetEmbeddedID() const;
	const SVGUID& GetOwnerID() const;
	BOOL IsCreated() const;

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject );
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject );

#pragma region virtual method (IObjectClass)
	virtual LPCTSTR GetName() const override;
	virtual SVString GetCompleteName() const override;
	virtual HRESULT GetCompleteNameToType(SVObjectTypeEnum objectType, SVString& rName) const override;
	virtual SVString GetObjectNameToObjectType(LPCSTR CompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType) const override;
	virtual const SVObjectTypeEnum& GetObjectType() const override;
	virtual SVObjectSubTypeEnum GetObjectSubType() const override;
	virtual const SVGUID& GetParentID() const override;
	virtual SvOi::IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) override;
	virtual const SvOi::IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) const override;
	virtual UINT ObjectAttributesAllowed() const override;
	virtual UINT SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type ) override;
	virtual UINT ObjectAttributesSet(int iIndex=0) const override;
	virtual UINT SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int iIndex=0 ) override;
	virtual const SVGUID& GetUniqueObjectID() const override;
	virtual bool is_Created() const override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& rObjectTypeInfo) const override;
	virtual void SetName( LPCTSTR Name ) override;
	virtual SvOi::IObjectClass* getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override { return ResetObject(pErrorMessages); };
	virtual HRESULT getValue(double& rValue, int Index = -1, int Bucket = -1) const override { return E_NOTIMPL; };
	virtual HRESULT getValues(std::vector<double>& rValues, int Bucket = -1) const override { return E_NOTIMPL; };
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
	virtual void GetInputInterface(SVInputInfoListClass& rInputList, bool bAlsoFriends) const {};

	/// Will be called, if an object was renamed.
	/// \param rRenamedObject [in] Reference to the renamed object.
	/// \param rOldName [in] Old name of the object.
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) {};

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

public:

	void SetDefaultObjectAttributesSet(UINT uAttributes);

protected:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_ObjectAttributesAllowed;
	//This attribute holds the array enumerated bits of set object attributes based on object array depth.
	std::vector<UINT> m_ObjectAttributesSet;
	//This attribute holds the enumerated bits of default object attributes.
	UINT m_DefaultObjectAttributesSet;

	//This attribute holds the data depth of the embedded data elements.
	int m_objectDepth;

	//This attribute holds the image depth of the embedded image elements.
	long m_lImageDepth;

	//This attribute holds the validity state of the object.
	mutable BOOL m_isObjectValid;
	// Refer to IsCreated()
	BOOL m_isCreated;
	//If object is embedded, set this ID
	SVGUID m_embeddedID;
	//Owner Info
	SVObjectInfoStruct m_ownerObjectInfo;
	//Contains the object info and could also be used as task out info.
	SVOutObjectInfoStruct m_outObjectInfo;
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVObjectInfoArrayClass m_friendList;
	//Output table
	SVPublicAttributeEntryStruct m_publicAttribute;

private:
	void init();

	int m_resourceID;		//String resource ID, of NOT user changeable name.
	SVString m_ObjectName;	//NOT user changeable name
	SVString m_Name;			//user given name
};

#pragma region Declarations
typedef std::vector<SVObjectClass*> SVObjectPtrVector;
typedef std::back_insert_iterator<SVObjectPtrVector> SVObjectPtrVectorInserter;
typedef std::set<SVObjectClass*> SVObjectPtrSet;
#pragma region Declarations

#include "SVObjectClass.inl"

