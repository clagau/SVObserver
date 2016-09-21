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

	virtual void GetAllInputObjects();
	virtual void Persist(SVObjectWriter& writer);

	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );

	virtual HRESULT IsInputImage( SVImageClass* p_psvImage );

	// Routing Version of Validate
	// Validates Local Scope and all owned objects
	virtual BOOL Validate();

	// Non Routing Version of Validate
	// validates only Local Scope
	virtual BOOL OnValidate();

	virtual BOOL CloseObject();

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

	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual BOOL SetImageDepth( long lDepth );

	virtual void SetInvalid();
	virtual void SetDisabled();

	const SVString checkName( LPCTSTR ToolName ) const;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

#pragma region virtual methods (ITaskObjectListClass)
	virtual SvUl::NameGuidList GetTaskObjectList( ) const override;
	virtual void Delete(GUID& objectID) override;
	virtual void InsertAt(int index, SvOi::ITaskObject& rObject, int count = 1) override;
	virtual DWORD_PTR DestroyChild(SvOi::ITaskObject& rObject, DWORD context) override;
	virtual bool DestroyFriendObject(IObjectClass& rObject, DWORD context) override;
	virtual SvUl::NameGuidList GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const override;
#pragma endregion virtual methods (ITaskObjectListClass)
#pragma endregion public methods	

#pragma region protected methods
protected:
	virtual void DeleteAt( int Index, int Count = 1 );
	void DeleteAll();

	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner );
	BOOL getAvailableObjects( SVClassInfoStructListClass* pList, const SVObjectTypeInfoStruct* pObjectTypeInfo ) const;

	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	// He will call OnValidate() for you!
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR	processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual DWORD_PTR	OutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual DWORD_PTR	ChildrenOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
#pragma endregion protected methods	

#pragma region Private Methods
private:
	/**********
	  The method destroys a child object. 
	  /param pTaskObject <in> object to destroy.
	  /param context <in>.
	***********/
	DWORD_PTR DestroyChildObject(SVTaskObjectClass* pTaskObject, DWORD context);

	/// The method destroy a taskObject
	/// \param rTaskObject [in] This object will destroyed.
	/// \param context [in] Bits define action (e.g. SVMFSetDefaultInputs = set default inputs, SVMFResetInspection = reset inspection)
	void DestroyTaskObject(SVTaskObjectClass& rTaskObject, DWORD context);
#pragma endregion Private Methods
	
#pragma region Member Variables
protected:
	typedef SVVector< SVTaskObjectClass*, SVTaskObjectClass* > SVTaskObjectPtrVector;
	SVClassInfoStructListClass m_availableChildren; // available children classes (not instantiated)

	SVClock::SVTimeStamp m_LastListUpdateTimestamp;
	SVTaskObjectPtrVector m_aTaskObjects;
#pragma endregion Member Variables
};

