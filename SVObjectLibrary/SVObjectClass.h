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
#include "SVStatusLibrary/SVErrorClass.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVObjectScriptUsage.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVPublicAttributeEntryStruct.h"
#include "SVObjectAttributeShim.h"
#include "SVObjectWriter.h"
#include "SVObjectInfoArrayClass.h"
#include "SVUtilityLibrary/NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
struct SVInObjectInfoStruct;
struct SVObjectLevelCreateStruct;
struct SVObjectNameInfo;

class SVDrawContext;
class SVObjectAttributeClass;
#pragma endregion Declarations

/*
This class is the base class to all configuration level objects.  These objects will get created and managed by the object manager object.
*/
class SVObjectClass : virtual public SvOi::IObjectClass
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

	friend class SVObjectManagerClass;
	friend class SVPPQObject;
	friend class SVConfigurationObject;

	friend DWORD_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	friend DWORD_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVObjectClass();
	SVObjectClass( LPCSTR ObjectName );
	SVObjectClass( SVObjectClass *POwner, int StringResourceID );

	virtual ~SVObjectClass();

	template< typename SVObjectVisitor >
	HRESULT Accept( SVObjectVisitor& p_rVisitor );

	virtual HRESULT ResetObject();

	virtual void ResetPrivateInputInterface();

	int GetObjectDepth() const;
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual long GetImageDepth() const;
	virtual BOOL SetImageDepth( long lDepth );

	virtual BOOL InitObject( SVObjectClass* PObject );
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ConnectObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL IsValid();
	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual BOOL SetObjectOwner( const GUID& RNewOwnerGUID );

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );
	virtual void SetInvalid();
	virtual void SetDisabled();
	virtual DWORD GetObjectColor() const;
	virtual DWORD GetObjectState() const;

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectSubScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
	virtual void PutAttributesInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void PutPQDataLinkInfoInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void PutFriendGuidsInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void MakeUniqueFriendAlias( CString& RStrScript );
#endif
	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistAttributes( SVObjectWriter& rWriter );

	virtual BOOL ReinitObjectInfos();

	virtual BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;

	virtual HRESULT ResetObjectInputs();

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	BOOL ConnectObjectInput( SVInObjectInfoStruct* PObjectInInfo );
	BOOL DisconnectObjectInput( SVInObjectInfoStruct* PObjectInInfo );
	BOOL IsObjectValid( GUID& RValidationReferenceID );
	BOOL IsDescendantOf( SVObjectClass* PAncestorObject );
	BOOL IsDescendantOfType( const SVObjectInfoStruct& rAncestorInfo );

	virtual void ResetName();
	virtual void SetName( LPCTSTR StrString );
	virtual void SetObjectName( int StringResourceID );

	void SetObjectEmbedded( const GUID& REmbeddedID, SVObjectClass* POwner, int NewStringResourceID );

	BOOL AddFriend( const GUID& RFriendGUID );
	BOOL RemoveFriend( const GUID& RFriendGUID );
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

	CString GetCompleteObjectNameToObjectType( LPCSTR LPSZCompleteName = NULL, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType ) const;
	LPTSTR GetCompleteObjectName2( LPCTSTR LPSZCompleteName = NULL ) const;
	int GetCompleteObjectNameLength( int Length ) const;
	int GetResourceID() const;
	SVObjectClass* GetOwner() const;
	SVOutObjectInfoStruct& GetObjectOutputInfo();
	
	const SVGUID& GetEmbeddedID() const;
	const SVGUID& GetOwnerID() const;
	BOOL IsCreated() const;

#pragma region virtual method (IObjectClass)
	virtual LPCTSTR GetName() const override;
	virtual SVString GetCompleteName() const override;
	virtual HRESULT GetCompleteNameToType(SVObjectTypeEnum objectType, SVString& rName) const override;
	virtual SVString GetObjectNameToObjectType(LPCSTR LPSZCompleteName = nullptr, SVObjectTypeEnum objectTypeToInclude = SVToolSetObjectType) const override;
	virtual const SVObjectTypeEnum& GetObjectType() const override;
	virtual SVObjectSubTypeEnum GetObjectSubType() const override;
	virtual const SVGUID& GetParentID() const override;
	virtual SvOi::IObjectClass* GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) override;
	virtual const SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SVObjectTypeEnum ancestorObjectType) const override;
	virtual SvOi::IObjectClass* GetFirstObject(const SVObjectTypeInfoStruct& type) override;
	virtual const UINT ObjectAttributesAllowed() const override;
	virtual const UINT ObjectAttributesSet(int iIndex=0) const override;
	virtual bool IsArray() const;
	virtual int GetArraySize() const;
	virtual const SVGUID& GetUniqueObjectID() const override;
	virtual bool is_Created() const override { return IsCreated() ? true : false; }
	virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const override;
#pragma endregion virtual method (IObjectClass)

	const SVObjectInfoStruct& GetOwnerInfo() const;
	const SVObjectInfoStruct& GetObjectInfo() const;
	const SVObjectInfoArrayClass& GetFriendList() const;

protected:
	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual SVObjectClass *UpdateObject( const GUID &friendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void buildCompleteObjectName( LPTSTR LPSZCompleteName, int MaxLength );

	virtual HRESULT RemoveObjectConnection( const GUID& p_rObjectID );

public:
	//This attribute holds the object level error information.
	SVErrorClass msvError;

	UINT& ObjectAttributesAllowedRef();
	SVObjectAttributeShim ObjectAttributesSetRef(int iIndex=0);
	void SetObjectAttributesSet(UINT uAttributes, int iIndex=0, UINT WhichBits = SET_ALL_BITS);
	void SetDefaultObjectAttributesSet(UINT uAttributes);

protected:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_uObjectAttributesAllowed;
	//This attribute holds the enumerated bits of set object attributes.
	UINT m_uObjectAttributesSet;
	//This attribute holds the array enumerated bits of set object attributes based on object array depth.
	std::vector<UINT> m_auObjectAttributesSet;
	//This attribute holds the enumerated bits of default object attributes.
	UINT m_uDefaultObjectAttributesSet;

	//This attribute holds the data depth of the embedded data elements.
	int objectDepth;

	//This attribute holds the image depth of the embedded image elements.
	long mlImageDepth;

	//This attribute holds the validity state of the object.
	mutable BOOL isObjectValid;
	// Refer to IsCreated()
	BOOL isCreated;
	//This attribute holds the validation reference identification number.
	SVGUID validationReferenceID;
	//If object is embedded, set this ID
	SVGUID embeddedID;
	//Owner Info
	SVObjectInfoStruct ownerObjectInfo;
	//Contains the object info and could also be used as task out info.
	SVOutObjectInfoStruct outObjectInfo;
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVObjectInfoArrayClass friendList;
	//Output table
	SVPublicAttributeEntryStruct publicAttribute;

	//user given name
	CString strName;

private:
	void init();

	//String resource ID, of NOT user changeable name.
	int resourceID;
	//NOT user changeable name
	CString strObjectName;
};

#pragma region Declarations
typedef std::vector<SVObjectClass*> SVObjectVector;
typedef std::set<SVObjectClass*> SVStdSetSVObjectClassPtr;
typedef SVVector< SVObjectClass* > SVObjectClassPtrArray;
#pragma region Declarations

#include "SVObjectClass.inl"

