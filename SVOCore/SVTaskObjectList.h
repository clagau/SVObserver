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
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVClassInfoStruct.h"
#include "SVTaskObject.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVTaskObjectListClass : public SVTaskObjectClass, public SvOi::ITaskObjectListClass
{
	SV_DECLARE_CLASS( SVTaskObjectListClass )

public:
#pragma region Constructor
	SVTaskObjectListClass( LPCSTR LPSZObjectName );
	SVTaskObjectListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECTLIST );
	virtual ~SVTaskObjectListClass();
#pragma endregion Constructor

#pragma region public methods
public:
	virtual HRESULT GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const override;

	void AppendInputObjects();
	void RemoveOutputObject( SVOutObjectInfoStruct* pOutObject );

	virtual void GetAllInputObjects() override;
	virtual void Persist(SVObjectWriter& writer) override;

	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;

	virtual HRESULT IsInputImage( SVImageClass* p_psvImage ) override;

	// Routing Version of Validate
	// Validates Local Scope and all owned objects
	virtual BOOL Validate() override;

	// Non Routing Version of Validate
	// validates only Local Scope
	virtual BOOL OnValidate() override;

	virtual BOOL CloseObject() override;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	const SVClock::SVTimeStamp& GetLastListUpdateTimestamp() const;

	int GetSize() const;
	void InsertAt( int nIndex, SVTaskObjectClass* PTaskObject, int nCount = 1 );
	void SetAt( int nIndex, SVTaskObjectClass* PTaskObject );
	SVTaskObjectClass* GetAt( int nIndex ) const;
	void RemoveAt( int nIndex, int nCount = 1 );
	/// Add a task object to the task object list
	/// \param pTaskObject [in] Pointer of the task object
	/// \param atBegin [in] If true it add it at the begin, if false (default) it add it at the end.
	/// \returns int Position of the new task in the list.
	int Add( SVTaskObjectClass* pTaskObject, bool atBegin = false );
	HRESULT RemoveChild( SVTaskObjectClass* pChildObject );	

	virtual BOOL SetObjectDepth( int NewObjectDepth ) override;
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual BOOL SetImageDepth( long lDepth ) override;

	virtual void SetInvalid() override;
	virtual void SetDisabled() override;

	const SVString checkName( LPCTSTR ToolName ) const;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray ) override;

	/// The method destroys a child object. 
	/// \param pTaskObject [in] object to destroy
	/// \param context [in]
	/// \returns bool true if successfully
	bool DestroyChildObject( SVTaskObjectClass* pTaskObject, DWORD context = 0 );

#pragma region virtual methods (ITaskObjectListClass)
	virtual SvUl::NameGuidList GetTaskObjectList( ) const override;
	virtual void Delete(GUID& objectID) override;
	virtual void InsertAt(int index, SvOi::ITaskObject& rObject, int count = 1) override;
	virtual bool DestroyChild(SvOi::ITaskObject& rObject, DWORD context) override;
	virtual bool DestroyFriendObject(IObjectClass& rObject, DWORD context) override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const override;
#pragma endregion virtual methods (ITaskObjectListClass)

#pragma region Methods to replace processMessage
	virtual bool createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void GetInputInterface(SVInputInfoListClass& rInputList, bool bAlsoFriends) const override;
	virtual SvOi::IObjectClass* getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override;
	virtual bool ConnectAllInputs() override;
	virtual bool replaceObject(SVObjectClass* pObject, const GUID& rNewGuid) override;

	BOOL getAvailableObjects( SVClassInfoStructListClass* pList, const SVObjectTypeInfoStruct* pObjectTypeInfo ) const;
	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;
#pragma endregion Methods to replace processMessage
#pragma endregion public methods	

#pragma region protected methods
protected:
	void DeleteAt( int Index, int Count = 1 );
	void DeleteAll();

	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray ) override;

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner ) override;

	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual BOOL Run( SVRunStatusClass& RRunStatus ) override;

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual bool resetAllOutputListObjects( bool shouldNotifyFriends, bool silentReset ) override;

	/// Call method ConnectObject at the child object with a create struct defined in this method.
	/// \param rChildObject [in] Child object
	virtual void connectChildObject( SVTaskObjectClass& rChildObject );
#pragma endregion protected methods	

#pragma region Private Methods
private:
	/// The method destroy a taskObject
	/// \param rTaskObject [in] This object will destroyed.
	/// \param context [in] Bits define action (e.g. SVMFSetDefaultInputs = set default inputs, SVMFResetInspection = reset inspection)
	void DestroyTaskObject(SVTaskObjectClass& rTaskObject, DWORD context);

	SvOi::IObjectClass* getFirstObjectWithRequestor( const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor ) const;
#pragma endregion Private Methods
	
#pragma region Member Variables
protected:
	typedef SVVector< SVTaskObjectClass*, SVTaskObjectClass* > SVTaskObjectPtrVector;
	SVClassInfoStructListClass m_availableChildren; // available children classes (not instantiated)

	SVClock::SVTimeStamp m_LastListUpdateTimestamp;
	SVTaskObjectPtrVector m_aTaskObjects;
#pragma endregion Member Variables
};

