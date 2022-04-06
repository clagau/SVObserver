//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectList
//* .File Name       : $Workfile:   SVTaskObjectList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 04:22:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "SVClassInfoStruct.h"
#include "SVTaskObject.h"
#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvIe
{
	class RunStatus;
}

namespace SvIe
{


class SVTaskObjectListClass : public SVTaskObjectClass, public SvOi::ITaskObjectListClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	typedef std::vector<SVTaskObjectClass*> SVTaskObjectPtrVector;

#pragma region Constructor
	explicit SVTaskObjectListClass(LPCSTR LPSZObjectName);
	SVTaskObjectListClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECTLIST );
	virtual ~SVTaskObjectListClass();
#pragma endregion Constructor

#pragma region public methods
public:
	
	virtual void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const override;
	virtual void fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter) override;

	virtual void Persist(SvOi::IObjectWriter& rWriter) const override;

	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool CloseObject() override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	const double& GetLastListUpdateTimestamp() const;

	int GetSize() const;
	//Add a task Object to the taskObject list before the object with rObjectBeforeID.
	//return the index of the new element 
	virtual int  InsertBefore(uint32_t rObjectBeforeID, SVTaskObjectClass* pTaskObject);
	SVTaskObjectClass* GetAt( int nIndex ) const;
	void RemoveAt( int nIndex);
	/// Add a task object to the task object list
	/// \param pTaskObject [in] Pointer of the task object
	/// \returns int Position of the new task in the list.
	int Add( SVTaskObjectClass* pTaskObject, bool atBegin = false );
	HRESULT RemoveChild( SVTaskObjectClass* pChildObject );	

	virtual void SetDisabled() override;

	std::string getUniqueName(const std::string& rToolName, bool adaptEndNumbers = false) const;

	//insensitive compare with name in m_TaskObjectVector
	bool IsNameUnique(LPCSTR  pName, LPCTSTR pExclude = nullptr) const;

	virtual void setEditModeFreezeFlag(bool flag) override;
	virtual void copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr) override;
	virtual void goingOffline() override;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray ) override;
	virtual void collectOverlays(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const override;

	/// The method destroys a child object. 
	/// \param pTaskObject [in] object to destroy
	/// \param context [in]
	/// \returns bool true if successfully
	bool DestroyChildObject( SVTaskObjectClass* pTaskObject, DWORD context = 0 );
	
	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;
	virtual HRESULT ConnectToObject(const std::string& rInputName, uint32_t newID, SvPb::SVObjectTypeEnum objectType = SvPb::SVNotSetObjectType, bool shouldResetObject = false) override;
#pragma region virtual methods (ITaskObjectListClass)
	virtual void Delete(uint32_t objectID) override;
	virtual void InsertBefore(uint32_t objectBeforeID, ITaskObject& rObject) override;
	virtual void InsertAt(int pos, ITaskObject& rObject) override;
	virtual bool DestroyChild(SvOi::ITaskObject& rObject, DWORD context) override;
	virtual SvUl::NameClassIdList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const override;
	virtual void moveTaskObject(uint32_t objectToMoveId, uint32_t preObjectId = SvDef::InvalidObjectId) override;
	virtual void GetTaskObjectListInfo(SvPb::TaskObjectListResponse &rResponse) const override;
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse& rRequest) const override;
#pragma endregion virtual methods (ITaskObjectListClass)

#pragma region Methods to replace processMessage
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
	virtual bool connectAllInputs() override;
	virtual void disconnectAllInputs() override;
	virtual void getInputs(std::back_insert_iterator<std::vector<SvOl::InputObject*>> inserter) const override;
	virtual bool replaceObject(SVObjectClass* pObject, uint32_t newId) override;

	bool getAvailableObjects(SVClassInfoStructVector* pList, const SvDef::SVObjectTypeInfoStruct* pObjectTypeInfo ) const;
	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Methods to replace processMessage

	SVTaskObjectClass* UpdateObject(uint32_t friendId, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner);
#pragma endregion public methods	

#pragma region protected methods
protected:
	void DeleteAt( int Index, int Count = 1 );
	void DeleteAll();

	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	std::string getUniqueNumberedName(const std::string& rToolName) const;

	/// Call method ConnectObject at the child object with a create struct defined in this method.
	/// \param rChildObject [in] Child object
	virtual void connectChildObject( SVTaskObjectClass& rChildObject );
#pragma endregion protected methods	

#pragma region Private Methods
private:
	SvOi::IObjectClass* getFirstObjectWithRequestor( const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor ) const;

	/// Remove the object from the friend-List (don't delete the object itself)
	/// \param friendID [in] ID of the friend to be removed.
	/// \returns bool true if object in the list found and removed. False if object is not in the list.
	bool RemoveFriend(uint32_t friendID);

	/// Remove the object from the TaskObjectVector (don't delete the object itself)
	/// \param objectID [in] ID of the task to be removed.
	/// \returns bool true if object in the list found and removed. False if object is not in the list.
	bool RemoveFromTaskObjectVector(uint32_t objectID);
#pragma endregion Private Methods
	
#pragma region Member Variables
protected:
	SVClassInfoStructVector m_availableChildren; // available children classes (not instantiated)

	double m_LastListUpdateTimestamp;
	SVTaskObjectPtrVector m_TaskObjectVector;
#pragma endregion Member Variables
};

} //namespace SvIe
