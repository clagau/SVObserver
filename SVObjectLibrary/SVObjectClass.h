//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVContainerLibrary/SVThreadSafeList.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#pragma endregion Includes



#pragma region Declarations
struct SVObjectLevelCreateStruct;
struct SVObjectNameInfo;

class SVObjectAttributeClass;

namespace SvIe
{
class RunStatus;
}
namespace SvOi
{
class IObjectWriter;
enum SVResetItemEnum : int;
}
namespace SvPb
{
class FixedInputData;
}

typedef std::vector<SVObjectClass*> SVObjectPtrVector;
typedef std::back_insert_iterator<SVObjectPtrVector> SVObjectPtrVectorInserter;
typedef std::set<SVObjectClass*> SVObjectPtrSet;
#pragma endregion Declarations

/*
This class is the base class to all configuration level objects.  These objects will get created and managed by the object manager object.
*/
class SVObjectClass : public SvOi::IObjectClass
{
	SV_DECLARE_CLASS
	
	enum RefreshObjectType
	{
		None,
		PreRefresh,
		PostRefresh,
	};

public:
	friend class SVObjectManagerClass;	// For access to RemoveObjectConnection

	SVObjectClass();
	explicit SVObjectClass( LPCTSTR ObjectName );
	SVObjectClass( SVObjectClass* pOwner, int StringResourceID );

	virtual ~SVObjectClass();

	
	
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

	/// Write object to XML-file
	/// \param rWriter [in]
	/// \param closeObject [in] normally the object will be closed (true), but if the caller have to add parameter to this object it can force that the object stay open. The caller have to close the object than.
	virtual void Persist( SvOi::IObjectWriter& rWriter, bool closeObject = true ) const;

	void PersistBaseData(SvOi::IObjectWriter& rWriter) const;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	virtual bool connectAllInputs() { return false; };
	void connectObject(uint32_t objectId);
	void disconnectObject(uint32_t objectId);
	virtual void disconnectObjectInput(uint32_t objectId);
	virtual void disconnectAllInputs();

	virtual bool isInputImage(uint32_t ) const { return false; };

	virtual void ResetName();
	void SetObjectName( LPCTSTR ObjectName );

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

	SVObjectClass* GetParent() const { return m_pOwner; };
	
	bool IsCreated() const { return m_isCreated; };

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject );
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject );

	virtual void setEditModeFreezeFlag(bool flag) { m_editModeFreezeFlag = flag; };
	/// Preparing to go offline. Is used e.g. by the Archive Tool and to save images if they are not done in edit mode.
	virtual void goingOffline() {};
	virtual void copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr) {};
	virtual bool isValidDependency(const std::pair<std::string, std::string>&) const { return true; };
	bool isLoopOrGroupTool() const { return (SvPb::LoopToolObjectType == GetObjectSubType() || SvPb::GroupToolObjectType == GetObjectSubType() || SvPb::ModuleToolObjectType == GetObjectSubType()); }

#pragma region virtual method (IObjectClass)
	virtual LPCTSTR GetName() const override;
	virtual std::string GetCompleteName() const override;
	virtual HRESULT GetCompleteNameToType(SvPb::SVObjectTypeEnum objectType, std::string& rName) const override;
	virtual std::string GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVToolSetObjectType, bool withOwnName = true) const override;
	//Get the complete object name before selected SvPb::SVObjectTypeEnum value.
	virtual std::string GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const override;
	virtual const SvPb::SVObjectTypeEnum& GetObjectType() const override { return m_ObjectTypeInfo.m_ObjectType; };
	virtual SvPb::SVObjectSubTypeEnum GetObjectSubType() const override;
	virtual uint32_t GetParentID() const override { return m_pOwner ? m_pOwner->getObjectId() : SvDef::InvalidObjectId; };
	virtual SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) override;
	virtual const SvOi::IObjectClass* GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel = false) const override;
	virtual UINT ObjectAttributesAllowed() const override;
	virtual UINT SetObjectAttributesAllowed( UINT Attributes, SvOi::SetAttributeType Type ) override;
	virtual uint32_t getObjectId() const override {	return m_objectId; };
	void setObjectId(uint32_t objectId);
	/// Move this object data to the other object: switch objectId, copy value and move connection.
	/// \param rNewObject [inout] new object
	virtual void moveObject(SVObjectClass& rNewObject);
	
	///  Move the children data to the other object: switch objectId, copy value and move connection.
	///  Will only do anything if object has children (by now only LinkedValues)
	/// \param rNewObject [inout] new object
	virtual void moveChildObject(SVObjectClass&/* rNewObject*/) {};
	virtual SvPb::EmbeddedIdEnum GetEmbeddedID() const override { return m_embeddedID; };
	virtual bool is_Created() const override;
	virtual SvUl::NameClassIdList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const override;
	virtual SvStl::MessageContainerVector verifyAndSetName(const std::string& newName) override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr, int nResetDepth = 0) override;
	virtual HRESULT getValue(double& , int = 0) const override { return E_NOTIMPL; };
	virtual HRESULT getValues(std::vector<double>& ) const override { return E_NOTIMPL; };
	virtual void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const override;

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const override;
	virtual bool allowExtensionCopy() const override;
	virtual std::string getComment() const override { return m_comment; };
	virtual void setComment(const std::string& rComment) override { m_comment = rComment; };
#pragma endregion virtual method (IObjectClass)

	virtual void SetName(LPCTSTR Name);
	virtual void fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter);
	virtual void changeSource(const SVObjectReference& /*rOldObject*/, SVObjectClass& /*rNewObject*/) { Log_Assert(false); };
	const SvDef::SVObjectTypeInfoStruct& getObjectTypeInfo() const { return m_ObjectTypeInfo; };
	bool isViewable() const; ///< Check if Viewable-attribute is set by this object and also by its owner.

	virtual SvOi::IValueObject* getValueObjectPtr()  override { return nullptr; };
	virtual SvOi::ITool* getToolPtr() override { return nullptr; } ;

#pragma region Methods to replace processMessage
	/// Call the method createObject for all children and itself.
	/// \param rCreateStructure [in]
	/// \returns bool
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure );
	virtual void refreshAllObjects(const SVObjectLevelCreateStruct& rCreateStructure);

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

	virtual bool isCorrectType(SvPb::ObjectSelectorType requiredType) const;
	bool checkIfValidDependency(const SVObjectClass* pObject) const;

	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset, bool isExternallySettable);
	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR strName, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset, bool isExternallySettable);

	/// Old for loading old configs older than 10.20
	/// Set indirect value string to LinkedValue to help to convert LinkedValue from old to new struct.
	HRESULT setIndirectStringToObject(SvPb::EmbeddedIdEnum embeddedId, const std::vector<_variant_t>& rValueString);

	SvOi::ObjectNotificationRAIIPtr registerNotification(SvOi::ObjectNotificationFunctionPtr pFunc);
	virtual bool areImagesNeededInTRC() const; //<This method is initially called from images, to check if the image needs to be in TRC.
	void SetLateReset() { m_LateReset = true; }
	bool GetLateReset() const { return m_LateReset; } 
	void sendChangeNotification(SvOi::ObjectNotificationType type, uint32_t objectId) const;

protected:
	/// Convert a string (dotted name) to an object.
	/// \param rValue [in] Input string
	/// \returns SVObjectReference A reference to the found object. 
	SVObjectReference GetObjectReferenceForDottedName(const std::string& rValue) const;

	virtual HRESULT RemoveObjectConnection(uint32_t objectID );

	bool RegisterEmbeddedObjectAsClass(SVObjectClass* PEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newObjectName);
	void AddEmbeddedObject(SVObjectClass* pObject);
	void disableEmbeddedObject(SVObjectClass* pObjectToDisable);
	void RemoveEmbeddedObject(SVObjectClass* pObjectToRemove);
	bool createEmbeddedChildren();
	void setEmbeddedId(SvPb::EmbeddedIdEnum id) { m_embeddedID = id; };

	/// Call method createAllObjects for the child object with the right create struct.
	/// \param rChildObject [in]
	/// \returns bool
	virtual bool createAllObjectsFromChild( SVObjectClass& ) { return false; };

	bool isUsed() const { return 0 < m_connectedSet.size(); };
	const auto& getConnectedSet() const { return m_connectedSet; };

private:
	void init();
	void PersistEmbeddeds(SvOi::IObjectWriter& rWriter) const;
	
protected:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_ObjectAttributesAllowed;

	// Refer to IsCreated()
	bool m_isCreated {false};
	//Owner Info
	SVObjectClass* m_pOwner = nullptr;
	SvDef::SVObjectTypeInfoStruct m_ObjectTypeInfo{};

	SVObjectPtrVector m_embeddedList;
	

	bool m_editModeFreezeFlag = false;
	bool m_LateReset = false;
private:
	//ATTENTION: order of the parameter (especially m_ObjectName before m_Name) is important, because it is needed for the constructors.
	int m_resourceID;		//String resource ID, of NOT user changeable name.
	uint32_t m_objectId = SvDef::InvalidObjectId;
	SvPb::EmbeddedIdEnum m_embeddedID; //If object is embedded, set this ID
	std::string m_ObjectName;	//NOT user changeable name
	std::string m_Name;			//user given name
	std::string m_comment;	//
	std::set <uint32_t> m_connectedSet;
	std::mutex m_inputMutex;
	mutable bool m_CircularReference_AreImagesNeededInTRC {false};

	SVThreadSafeList<SvOi::ObjectNotificationFunctionPtr> m_notificationList;
};