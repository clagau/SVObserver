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

#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#pragma endregion Includes


#pragma region Declarations
namespace SvOi
{
class IObjectWriter;
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
	friend class SVObjectManagerClass;	// @TODO - This needs to go - For access to m_outObjectInfo to assignUnique id on loading
	friend class SVConfigurationObject; // @TODO - This needs to go - For access to m_outObjectInfo to assignUnique id on loading

	SVObjectClass();
	explicit SVObjectClass( LPCTSTR ObjectName );
	SVObjectClass( SVObjectClass* pOwner, int StringResourceID );

	virtual ~SVObjectClass();

	/*
	This method is a placeholder for the object reset functionality.  This method will be overridden by derived classes.
	*/
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual bool CloseObject();
	bool SetObjectOwner( SVObjectClass* pNewOwner );
	bool SetObjectOwner(uint32_t newOwnerID );

	virtual HRESULT SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject);
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	/// Set status color to disabled (in SVTaskObjectClass), but also for all children
	virtual void SetDisabled();
	virtual DWORD GetObjectColor() const;

	virtual void Persist( SvOi::IObjectWriter& rWriter );

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	virtual bool connectAllInputs() { return false; };
	void connectObject(uint32_t objectId);
	void disconnectObject(uint32_t objectId);
	virtual void disconnectObjectInput(uint32_t objectId);
	virtual void disconnectAllInputs();

	virtual bool isInputImage(uint32_t ) const { return false; };

	virtual void ResetName();
	virtual void SetObjectName( LPCTSTR ObjectName );

	void SetObjectEmbedded(SvPb::EmbeddedIdEnum embeddedID, SVObjectClass* pOwner, LPCTSTR NewObjectName);

	//@TODO[MZA][10.10][06.08.2020] SVTaskObjectClass has only friends. Other object has to do nothing. Maybe the method can be moved from here to SVTaskObjectClass later.
	virtual void DestroyFriends() {};

	//************************************
	// Method:    GetAncestor
	// Description: This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns nullptr.
	// Parameter: SVObjectTypeEnum AncestorObjectType 
	// Parameter: topLevel If true searches further if another parent is of object type
	// Returns:   SVObjectClass*
	//************************************
	SVObjectClass* GetAncestor( SvPb::SVObjectTypeEnum AncestorObjectType, bool topLevel=false ) const;
	LPCTSTR GetObjectName() const;

	SVObjectClass* GetParent() const { return m_ownerObjectInfo.getObject(); };
	SVOutObjectInfoStruct& GetObjectOutputInfo() { return m_outObjectInfo; };
	
	bool IsCreated() const { return m_isCreated; };

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject );
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject );

	virtual void setEditModeFreezeFlag(bool flag) { m_editModeFreezeFlag = flag; };
	/// Preparing to go offline. Is used e.g. by the Archive Tool and to save images if they are not done in edit mode.
	virtual void goingOffline() {};
	virtual void copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr) {};

#pragma region virtual method (IObjectClass)
	virtual LPCTSTR GetName() const override;
	virtual std::string GetCompleteName() const override;
	virtual HRESULT GetCompleteNameToType(SvPb::SVObjectTypeEnum objectType, std::string& rName) const override;
	virtual std::string GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVToolSetObjectType, bool withOwnName = true) const override;
	//Get the complete object name before selected SvPb::SVObjectTypeEnum value.
	virtual std::string GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const override;
	virtual const SvPb::SVObjectTypeEnum& GetObjectType() const override { return m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType; };
	virtual SvPb::SVObjectSubTypeEnum GetObjectSubType() const override;
	virtual uint32_t GetParentID() const override { return m_ownerObjectInfo.getObjectId(); };
	virtual SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) override;
	virtual const SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) const override;
	virtual UINT ObjectAttributesAllowed() const override;
	virtual UINT SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type ) override;
	virtual UINT ObjectAttributesSet(int iIndex=0) const override;
	virtual UINT SetObjectAttributesSet( UINT Attributes, SvOi::SetAttributeType Type, int iIndex=0 ) override;
	virtual uint32_t getObjectId() const override {	return m_outObjectInfo.getObjectId(); };
	virtual SvPb::EmbeddedIdEnum GetEmbeddedID() const override { return m_embeddedID; };
	virtual bool is_Created() const override;
	virtual SvUl::NameClassIdList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const override;
	virtual void SetName( LPCTSTR Name ) override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override { return ResetObject(pErrorMessages); };
	virtual HRESULT getValue(double& , int = 0) const override { return E_NOTIMPL; };
	virtual HRESULT getValues(std::vector<double>& ) const override { return E_NOTIMPL; };

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const override;
#pragma endregion virtual method (IObjectClass)

	const SVObjectInfoStruct& GetOwnerInfo() const { return m_ownerObjectInfo; };
	const SVObjectInfoStruct& GetObjectInfo() const { return m_outObjectInfo; };

#pragma region Methods to replace processMessage
	/// Call the method createObject for all children and itself.
	/// \param rCreateStructure [in]
	/// \returns bool
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure );

	/// Call this method at the object owner to create an object
	/// \param pChildObject [in] child object to create
	/// \param context [in]
	/// \returns bool
	virtual bool CreateChildObject( SVObjectClass* , DWORD = 0) { return false; };

	/// Overwrite ID of an embedded object.
	/// \param rUniqueID [in] New id of the object
	/// \param embeddedID [in] Embedded Id of the object to overwrite
	/// \returns SVObjectClass* Pointer to the overwritten object, nullptr if not found.
	virtual SVObjectClass* OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID);

	virtual SVObjectClass* overwriteInputObject(uint32_t, SvPb::EmbeddedIdEnum) { return nullptr; };

	/// Will be called, if an object was renamed.
	/// \param rRenamedObject [in] Reference to the renamed object.
	/// \param rOldName [in] Old name of the object.
	virtual void OnObjectRenamed(const SVObjectClass& , const std::string& ) {};

	/// Replace the current object with new ids etc.
	/// \param pObject [in,out] Object t be removed.
	/// \param rNewid [in] The mew ID of the object
	/// \returns bool
	virtual bool replaceObject(SVObjectClass* , uint32_t ) { return false; };
#pragma endregion Methods to replace processMessage

	virtual SVObjectClass* UpdateObject(uint32_t friendId, SVObjectClass* pObject, SVObjectClass* pNewOwner);
	virtual bool isCorrectType(SvPb::ObjectSelectorType requiredType, const SVObjectClass* pTestObject = nullptr) const;

protected:
	/// Convert a string (dotted name) to an object.
	/// \param rValue [in] Input string
	/// \returns SVObjectReference A reference to the found object. 
	SVObjectReference ConvertStringInObject(const std::string& rValue) const;

	virtual HRESULT RemoveObjectConnection(uint32_t objectID );

	/// Call method createAllObjects for the child object with the right create struct.
	/// \param rChildObject [in]
	/// \returns bool
	virtual bool createAllObjectsFromChild( SVObjectClass& ) { return false; };

	bool isUsed() const { return 0 < m_connectedSet.size(); };

protected:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_ObjectAttributesAllowed;
	//This attribute holds the array enumerated bits of set object attributes based on object array depth.
	std::vector<UINT> m_ObjectAttributesSet;

	// Refer to IsCreated()
	bool m_isCreated;
	//If object is embedded, set this ID
	SvPb::EmbeddedIdEnum m_embeddedID;
	//Owner Info
	SVObjectInfoStruct m_ownerObjectInfo;
	//Contains the object info and could also be used as task out info.
	SVOutObjectInfoStruct m_outObjectInfo;

	bool m_editModeFreezeFlag = false;

private:
	void init();
	
	//ATTENTION: order of the parameter (especially m_ObjectName before m_Name) is important, because it is needed for the constructors.
	int m_resourceID;		//String resource ID, of NOT user changeable name.
	std::string m_ObjectName;	//NOT user changeable name
	std::string m_Name;			//user given name
	std::set <uint32_t> m_connectedSet;
	std::mutex m_inputMutex;
};

#pragma region Declarations
typedef std::vector<SVObjectClass*> SVObjectPtrVector;
typedef std::back_insert_iterator<SVObjectPtrVector> SVObjectPtrVectorInserter;
typedef std::set<SVObjectClass*> SVObjectPtrSet;
#pragma region Declarations
