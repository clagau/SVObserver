//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManagerClass
//* .File Name       : $Workfile:   SVObjectManagerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   13 Nov 2014 10:01:18  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVObjectReference.h"
#include "SVObserverNotificationFunctor.h"
#include "SVObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
class SVObjectClass;
#pragma endregion Declarations

//
// SVObjectManagerClass
//
class SVObjectManagerClass
{
	friend class SVIPDoc;
	friend class SVIODoc;
	friend class SVToolClass;
	friend class SVObserverApp;

public:
	enum SVObjectManagerStateEnum
	{
		Unknown   = 0,
		ReadOnly  = 1,
		ReadWrite = 2,
	};

	typedef std::map< SVString, SVGUID > RootNameChildMap;

	typedef std::set< SVGUID > GuidSet;
	typedef std::deque< SVString > SVSubjectDataNameDeque;

	static SVObjectManagerClass& Instance();

	virtual ~SVObjectManagerClass();

	SVObjectManagerStateEnum GetState() const;
	HRESULT SetState( SVObjectManagerStateEnum p_State );

	const SVGUID GetChildRootObjectID( const SVString& rRootChild ) const;

	template< typename SVObjectTypeName >
	HRESULT GetRootChildObject( SVObjectTypeName*& rpObject, const SVString& rRootChild );

	//For backward compatibility
	template< typename SVObjectTypeName >
	HRESULT GetConfigurationObject( SVObjectTypeName*& rpObject );

	HRESULT ConstructRootObject( const SVGUID& rClassID );
	HRESULT DestroyRootObject();
	void setRootChildID(const SVString& rRootChild, const SVGUID& rUniqueID);
	void Translation(SVString& Name);

	HRESULT ConstructObject( const SVGUID& rClassID, GUID& rObjectID );
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectClass*& rpObject );

	template< typename SVObjectTypeName >
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectTypeName*& rpObject );

	void DestructObject( const SVGUID& rClassID );

	HRESULT GetObjectByIdentifier( const SVGUID& rObjectID, SVObjectClass*& rpObject ) const;

	HRESULT GetObjectByDottedName( const SVString& rFullName, GUID& rObjectID ) const;
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectClass*& rpObject ) const;
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectReference& rReference ) const;

	template< typename SVObjectTypeName >
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectTypeName*& rpObject );

	BOOL CreateUniqueObjectID( SVObjectClass* PObject );
	BOOL OpenUniqueObjectID( SVObjectClass* PObject );
	BOOL CloseUniqueObjectID( SVObjectClass* PObject );
	BOOL ChangeUniqueObjectID( SVObjectClass* PObject, const SVGUID& RNewGuid );

	SVObjectClass* GetObject( const SVGUID& RGuid ) const;
	SVObjectClass* GetObjectCompleteName( LPCTSTR tszName );
	SVObjectReference GetObjectReference( const CString& strName );

	SVGUID GetObjectIdFromCompleteName( const SVString& rName );

	SVString GetCompleteObjectName( const SVGUID& RGuid );

	HRESULT SubmitCommand( const SVGUID& rObjectID, const SVCommandTemplatePtr& rCommandPtr );
	HRESULT SubmitCommand( SVObjectClass& rObject, const SVCommandTemplatePtr& rCommandPtr );

	template< typename SVNotifyData >
	HRESULT Notify( const SVGUID& rObjectID, SVNotifyData& rData );

	template< typename ObjectVisitor >
	HRESULT VisitElements( ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID=GUID_NULL );

	HRESULT InsertObserver( SVObserverNotificationFunctorPtr p_FunctorPtr, long& rCookie );
	HRESULT EraseObserver( long p_Cookie );

	HRESULT GetObserverDataNames( long p_Cookie, SVSubjectDataNameDeque& rSubjectDataNames ) const;
	HRESULT GetObserverDataNames( const SVGUID& rObserverID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObserverSubject( const SVString& rSubjectDataName, const SVGUID& rObserverID, GUID& rSubjectID ) const;
	HRESULT GetObserverSubject( const SVString& rSubjectDataName, long p_Cookie, GUID& rSubjectID ) const;

	HRESULT GetObserverIds( const SVString& rSubjectDataName, const SVGUID& rSubjectID, GuidSet& rObserverIds );

	HRESULT AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );
	HRESULT DetachObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID );

	HRESULT DetachSubjectsAndObservers( const SVGUID& rObjectID );

	template< typename SVDataType >
	HRESULT UpdateObserver( const SVGUID& rObserverID, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObserver( long p_Cookie, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVDataType& rData );

	HRESULT DisconnectObjects( const SVGUID& rSource, const SVGUID& rDestination );

	SVIPDoc* GetIPDoc( const SVGUID& rIPGuid ) const;

	HRESULT RegisterIPDoc( const SVGUID& rIPGuid, SVIPDoc* p_pIPDoc );
	HRESULT UnregisterIPDoc( const SVGUID& rIPGuid );

	SVIODoc* GetIODoc( const SVGUID& rIPGuid ) const;

	HRESULT RegisterIODoc( const SVGUID& rIOGuid, SVIODoc* p_pIODoc );
	HRESULT UnregisterIODoc( const SVGUID& rIOGuid );

	long GetShortPPQIndicator() const;
	void ClearShortPPQIndicator();
	void IncrementShortPPQIndicator();
	void DecrementShortPPQIndicator();

	long GetProductIndicator() const;
	void ClearProductIndicator();
	void IncrementProductIndicator();
	void DecrementProductIndicator();
	void AdjustProductIndicator( long p_Amount );

	long GetPendingImageIndicator() const;
	void ClearPendingImageIndicator();
	void IncrementPendingImageIndicator();
	void DecrementPendingImageIndicator();
	void AdjustPendingImageIndicator( long p_Amount );

	long GetInspectionIndicator() const;
	void ClearInspectionIndicator();
	void IncrementInspectionIndicator();
	void DecrementInspectionIndicator();
	void AdjustInspectionIndicator( long p_Amount );

	long GetNextFrameRate( long p_LastFrameRate );

	long GetFileSequenceNumber() const;

	HRESULT getTreeList(const SVString& rPath, SVObjectReferenceVector& rObjectList, UINT AttributesAllowedFilter) const;

protected:
	typedef std::map< SVString, SVGUID > SVSubjectDataNameSubjectIDMap;

	struct SVCookieEntryStruct
	{
		long m_Cookie;
		SVObserverNotificationFunctorPtr m_FunctorPtr;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;

		SVCookieEntryStruct()
		: m_Cookie( 0 ), m_FunctorPtr( NULL ), m_SubjectIDs() {}

		SVCookieEntryStruct( long p_Cookie, SVObserverNotificationFunctorPtr p_FunctorPtr )
		: m_Cookie( p_Cookie ), m_FunctorPtr( p_FunctorPtr ), m_SubjectIDs() {}
	};

	typedef SVSharedPtr< SVCookieEntryStruct > SVCookieEntryStructPtr;
	typedef std::map< long, SVCookieEntryStructPtr > SVCookieEntryMap;

	typedef std::set< SVGUID > SVSubjectObserverSet;
	typedef std::set< long > SVSubjectCookieSet;

	typedef std::map< SVGUID, long > SVSubjectEnabledObserverMap;
	typedef std::map< long, long > SVSubjectEnabledCookieMap;

	struct SVSubjectObserverStruct
	{
		SVSubjectEnabledObserverMap m_SubjectObservers;
		SVSubjectEnabledCookieMap m_SubjectCookies;

		SVSubjectObserverStruct()
			: m_SubjectObservers(), m_SubjectCookies() {}
	};

	typedef std::map< SVString, SVSubjectObserverStruct > SVSubjectDataNameObserverMap;

	struct SVUniqueObjectEntryStruct
	{
		SVGUID ObjectUID;
		SVObjectClass* PObject;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;
		SVSubjectDataNameObserverMap m_DataNameSubjectObservers;

		SVUniqueObjectEntryStruct()
		: ObjectUID(), PObject( NULL ), m_SubjectIDs(), m_DataNameSubjectObservers() {}
	};

	typedef SVSharedPtr< SVUniqueObjectEntryStruct > SVUniqueObjectEntryStructPtr;
	typedef std::map< GUID, SVUniqueObjectEntryStructPtr > SVUniqueObjectEntryMap;
	typedef std::map< GUID, SVIPDoc* > SVIPDocMap;
	typedef std::map< GUID, SVIODoc* > SVIODocMap;

	SVObjectManagerClass();

	void Shutdown();

	HRESULT GetSubjectDataNames( const SVGUID& rSubjectID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers );
	HRESULT GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies );

	SVCookieEntryStructPtr GetCookieEntry( long p_Cookie ) const;

	SVGUID GetSubjectID( const SVString& rSubjectDataName, SVUniqueObjectEntryStructPtr p_ObjectEntryPtr ) const;
	SVGUID GetSubjectID( const SVString& rSubjectDataName, SVCookieEntryStructPtr p_CookieEntryPtr ) const;

	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const SVGUID& RGuid ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const CString& sName ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntryCompleteObjectName( const CString& sName ) const;

	HRESULT DetachSubjects( long p_Cookie );
	HRESULT DetachSubjects( const SVGUID& rObserverID );

	HRESULT DetachObservers( const SVGUID& rSubjectID );

	long m_State;

	mutable SVCriticalSection m_Lock;

	SVCookieEntryMap		m_CookieEntries;
	SVUniqueObjectEntryMap	m_UniqueObjectEntries;
	SVIPDocMap				m_IPDocs;
	SVIODocMap				m_IODocs;
	RootNameChildMap		m_RootNameChildren;
	TranslateMap			m_TranslationMap;

	long m_ShortPPQIndicator;
	long m_ProductIndicator;
	long m_PendingImageIndicator;
	long m_InspectionIndicator;

	long m_LastFrameRate;

	long m_ObserverCookie;

	long m_FileSequenceNumber;

private:
	//Do not implement this method
	SVObjectManagerClass( const SVObjectManagerClass& rObject );

	//Do not implement this method
	const SVObjectManagerClass& operator=( const SVObjectManagerClass& rObject );
};

#include "SVObjectManagerClass.inl"

