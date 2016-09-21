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

#include "ObjectInterfaces/IObjectClass.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVPublicAttributeEntryStruct.h"
#include "SVObjectWriter.h"
#include "SVObjectInfoArrayClass.h"
#include "SVUtilityLibrary/NameGuidList.h"
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

	friend DWORD_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	friend DWORD_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVObjectClass();
	SVObjectClass( LPCSTR ObjectName );
	SVObjectClass( SVObjectClass* pOwner, int StringResourceID );

	virtual ~SVObjectClass();

	template< typename SVObjectVisitor >
	HRESULT Accept( SVObjectVisitor& rVisitor );

	virtual HRESULT ResetObject();

	virtual void ResetPrivateInputInterface();

	int GetObjectDepth() const;
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual long GetImageDepth() const;
	virtual BOOL SetImageDepth( long lDepth );

	virtual BOOL InitObject( SVObjectClass* pObject );
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual HRESULT ConnectObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL IsValid();
	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL SetObjectOwner( SVObjectClass* pNewOwner );
	virtual BOOL SetObjectOwner( const GUID& rNewOwnerGUID );

	virtual HRESULT GetObjectValue( const SVString& rValueName, VARIANT& rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& rValueName, const _variant_t& rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual void SetInvalid();
	virtual void SetDisabled();
	virtual DWORD GetObjectColor() const;
	virtual DWORD GetObjectState() const;

	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistAttributes( SVObjectWriter& rWriter );

	virtual BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	virtual HRESULT ResetObjectInputs();

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	BOOL ConnectObjectInput( SVInObjectInfoStruct* pObjectInInfo );
	BOOL DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo );
	BOOL IsObjectValid( GUID& rValidationReferenceID );
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
	CString GetCompleteObjectName() const;
	void GetCompleteObjectName( CString& rString ) const;

	CString GetCompleteObjectNameToObjectType( LPCSTR CompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;
	LPTSTR GetCompleteObjectName2( LPCTSTR CompleteName = nullptr ) const;
	int GetCompleteObjectNameLength( int Length ) const;
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
	virtual const UINT& ObjectAttributesAllowed() const override;
	virtual UINT& ObjectAttributesAllowedRef() override;
	virtual const UINT& ObjectAttributesSet(int iIndex=0) const override;
	virtual UINT& ObjectAttributesSetRef(int Index=0) override;
	virtual bool IsArray() const;
	virtual int GetArraySize() const;
	virtual const SVGUID& GetUniqueObjectID() const override;
	virtual bool is_Created() const override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& rObjectTypeInfo) const override;
	virtual void SetName( LPCTSTR Name ) override;
#pragma endregion virtual method (IObjectClass)

	const SVObjectInfoStruct& GetOwnerInfo() const;
	const SVObjectInfoStruct& GetObjectInfo() const;
	const SVObjectInfoArrayClass& GetFriendList() const;

protected:
	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual SVObjectClass* UpdateObject( const GUID& rFriendGuid, SVObjectClass* pObject, SVObjectClass* pNewOwner );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void buildCompleteObjectName( LPTSTR CompleteName, int MaxLength );

	virtual HRESULT RemoveObjectConnection( const GUID& rObjectID );

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
	//This attribute holds the validation reference identification number.
	SVGUID m_validationReferenceID;
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

	//user given name
	CString m_Name;

private:
	void init();

	//String resource ID, of NOT user changeable name.
	int m_resourceID;
	//NOT user changeable name
	CString m_ObjectName;
};

#pragma region Declarations
typedef std::vector<SVObjectClass*> SVObjectVector;
typedef std::set<SVObjectClass*> SVStdSetSVObjectClassPtr;
typedef SVVector< SVObjectClass* > SVObjectClassPtrArray;
#pragma region Declarations

#include "SVObjectClass.inl"

