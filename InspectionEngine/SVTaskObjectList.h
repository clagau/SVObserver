//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "ObjectInterfaces/ObjectInfo.h"
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVClassInfoStruct.h"
#include "SVTaskObject.h"
#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvIe
{


class SVTaskObjectListClass : public SVTaskObjectClass, public SvOi::ITaskObjectListClass
{
	SV_DECLARE_CLASS( SVTaskObjectListClass )

public:
	typedef std::vector<SVTaskObjectClass*> SVTaskObjectPtrVector;

#pragma region Constructor
	SVTaskObjectListClass( LPCSTR LPSZObjectName );
	SVTaskObjectListClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECTLIST );
	virtual ~SVTaskObjectListClass();
#pragma endregion Constructor

#pragma region public methods
public:
	virtual HRESULT GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const override;

	void AppendInputObjects();
	void RemoveOutputObject( SVOutObjectInfoStruct* pOutObject );

	virtual void GetAllInputObjects() override;
	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

	virtual bool isInputImage(const SVGUID& rImageGuid) const override;

	virtual bool CloseObject() override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	const SvTl::SVTimeStamp& GetLastListUpdateTimestamp() const;

	int GetSize() const;
	void InsertAt( int nIndex, SVTaskObjectClass* pTaskObject, int Count = 1 );
	//Add a task Object to the taskObject list after the object with rGuid.
	//return the index of the new element 
	virtual int  InsertAfter(const SVGUID& rGuid, SVTaskObjectClass* pTaskObject);
	SVTaskObjectClass* GetAt( int nIndex ) const;
	void RemoveAt( int nIndex);
	/// Add a task object to the task object list
	/// \param pTaskObject [in] Pointer of the task object
	/// \returns int Position of the new task in the list.
	int Add( SVTaskObjectClass* pTaskObject, bool atBegin = false );
	HRESULT RemoveChild( SVTaskObjectClass* pChildObject );	

	virtual void SetDisabled() override;

	const std::string checkName( LPCTSTR ToolName ) const;
	//insensitive compare with name in m_TaskObjectVector
	bool SVTaskObjectListClass::IsNameUnique(LPCSTR  pName, LPCTSTR pExclude) const;

	const std::string  SVTaskObjectListClass::MakeUniqueToolName(LPCTSTR ToolName) const;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray ) override;

	/// The method destroys a child object. 
	/// \param pTaskObject [in] object to destroy
	/// \param context [in]
	/// \returns bool true if successfully
	bool DestroyChildObject( SVTaskObjectClass* pTaskObject, DWORD context = 0 );

#pragma region virtual methods (ITaskObjectListClass)
	virtual void Delete(const SVGUID& rObjectID) override;
	virtual void InsertAt(int index, SvOi::ITaskObject& rObject, int Count = 1) override;
	virtual void InsertAfter(const SVGUID& rPostObjectId, ITaskObject& rObject) override;
	virtual bool DestroyChild(SvOi::ITaskObject& rObject, DWORD context) override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const override;
	virtual void moveTaskObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId = GUID_NULL) override;
	virtual void GetTaskObjectListInfo(SvPb::TaskObjectListResponse &rResponse) const override;
#pragma endregion virtual methods (ITaskObjectListClass)

#pragma region Methods to replace processMessage
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
	virtual bool ConnectAllInputs() override;
	virtual bool replaceObject(SVObjectClass* pObject, const GUID& rNewGuid) override;

	bool getAvailableObjects(SVClassInfoStructVector* pList, const SvDef::SVObjectTypeInfoStruct* pObjectTypeInfo ) const;
	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Methods to replace processMessage
#pragma endregion public methods	

#pragma region protected methods
protected:
	void DeleteAt( int Index, int Count = 1 );
	void DeleteAll();

	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray ) override;

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner ) override;

	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual bool resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	/// Call method ConnectObject at the child object with a create struct defined in this method.
	/// \param rChildObject [in] Child object
	virtual void connectChildObject( SVTaskObjectClass& rChildObject );
#pragma endregion protected methods	

#pragma region Private Methods
private:
	SvOi::IObjectClass* getFirstObjectWithRequestor( const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor ) const;

	/// Remove the object from the friend-List (don't delete the object itself)
	/// \param rFriendGUID [in] GUID of the friend to be removed.
	/// \returns bool true if object in the list found and removed. False if object is not in the list.
	bool RemoveFriend(const GUID& rFriendGUID);

	/// Remove the object from the TaskObjectVector (don't delete the object itself)
	/// \param rObjectID [in] GUID of the task to be removed.
	/// \returns bool true if object in the list found and removed. False if object is not in the list.
	bool RemoveFromTaskObjectVector(const SVGUID& rObjectID);
#pragma endregion Private Methods
	
#pragma region Member Variables
protected:
	SVClassInfoStructVector m_availableChildren; // available children classes (not instantiated)

	SvTl::SVTimeStamp m_LastListUpdateTimestamp;
	SVTaskObjectPtrVector m_TaskObjectVector;
#pragma endregion Member Variables
};

} //namespace SvIe
